/******************************************************************************
 * Copyright 2013-2015 Espressif Systems
 *
 * FileName: user_main.c
 *
 * Description: Routines to use a SPI RAM chip as a big FIFO buffer. Multi-
 * thread-aware: the reading and writing can happen in different threads and
 * will block if the fifo is empty and full, respectively.
 *
 * Modification history:
 *     2015/06/02, v1.0 File created.
 *     2019/08/14, clean code for icy-meta-parser
 *******************************************************************************/
#include "esp_system.h"
#include "string.h"
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "http.h"
#include "spiram_fifo.h"
#include "spiram.h"
#include "playerconfig.h"

#define SPIREADSIZE 64

static int fifoRpos;
static int fifoWpos;
static int fifoFill;
static xSemaphoreHandle semCanRead;
static xSemaphoreHandle semCanWrite;
static xSemaphoreHandle mux;
static long fifoOvfCnt, fifoUdrCnt;

//Low watermark where we restart the reader thread.
#define FIFO_LOWMARK (112*1024)

#ifdef FAKE_SPI_BUFF
//Re-define a bunch of things so we use the internal buffer
#undef SPIRAMSIZE
//allocate enough for about one mp3 frame
//#define SPIRAMSIZE 1850
#define SPIRAMSIZE 32000
static char fakespiram[SPIRAMSIZE];
#define spiRamInit() while(0)
#define spiRamTest() 1
#define spiRamWrite(pos, buf, n) memcpy(&fakespiram[pos], buf, n)
#define spiRamRead(pos, buf, n) memcpy(buf, &fakespiram[pos], n)
#endif

//Initialize the FIFO
int spiRamFifoInit() {
	fifoRpos=0;
	fifoWpos=0;
	fifoFill=0;
	fifoOvfCnt=0;
	fifoUdrCnt=0;
	vSemaphoreCreateBinary(semCanRead);
	vSemaphoreCreateBinary(semCanWrite);
	mux=xSemaphoreCreateMutex();
	spiRamInit();
	return (spiRamTest());
}

void spiRamFifoReset() {
	xSemaphoreTake(mux, portMAX_DELAY);
	fifoRpos=0;
	fifoWpos=0;
	fifoFill=0;
	fifoOvfCnt=0;
	fifoUdrCnt=0;
	xSemaphoreGive(semCanWrite);
	xSemaphoreTake(semCanRead, portMAX_DELAY);
	xSemaphoreGive(mux);
}

//Read bytes from the FIFO
void spiRamFifoRead(char *buff, int len) {
	int n;
	while (len > 0) {
		n = len;
		if (n>SPIREADSIZE) n=SPIREADSIZE;			//don't read more than SPIREADSIZE
		if (n>(SPIRAMSIZE-fifoRpos)) n = SPIRAMSIZE - fifoRpos; //don't read past end of buffer
		xSemaphoreTake(mux, portMAX_DELAY);
		if (fifoFill < n) {
//			printf("FIFO empty.\n");
			//Drat, not enough data in FIFO. Wait till there's some written and try again.
			fifoUdrCnt++;
			xSemaphoreGive(mux);
			if (fifoFill < FIFO_LOWMARK) xSemaphoreTake(semCanRead, portMAX_DELAY);
		} else {
			//Read the data.
			spiRamRead(fifoRpos, buff, n);
			buff += n;
			len -= n;
			fifoFill -= n;
			fifoRpos += n;
			if (fifoRpos>=SPIRAMSIZE) fifoRpos=0;
			xSemaphoreGive(mux);
			xSemaphoreGive(semCanWrite); //Indicate writer thread there's some free room in the fifo
		}
	}
}

/*
                Data                    Meta                Data
    |--------------------------------|---|----|--------------------------------|
    '------ icy_meta_interval -------'        '------ icy_meta_interval -------'
                                     '-.-'--.-'
    metadata_len_byte / 16:------------'    ;
    metadata_content :---------------------'-----> StreamTitle='Song title';

*/

enum FIFO_FLAG { DATA, ICY_META_LENGTH, ICY_META_CONTENT };
enum FIFO_FLAG fifoFlag = DATA;

unsigned int metaLength = 0, readMetaBytes = 0, readDataBytes = 0;

void resetReader() {
	fifoFlag = DATA;
	readDataBytes = 0;
}

void resetMetaData(bool resetText) {
	metaLength = 0;
	readMetaBytes = 0;

	if (resetText)
		icymeta_text[0] = 0;
}

// returns bytes remaining to be processed
int processMetaData(const char *buff, int buffLen) {
	switch (fifoFlag) {
	case ICY_META_LENGTH:
		if (buffLen <= 0) {
			return 0;
		}

		metaLength = buff[0] * 16;

		// Cleanup for meta content
		++buff;
		--buffLen;
		readMetaBytes = 0;
		fifoFlag = ICY_META_CONTENT;

		return processMetaData(buff, buffLen);

	case ICY_META_CONTENT:;
		bool dataInSegment = buffLen > (metaLength - readMetaBytes);

		while (1) {
			// Finished reading meta, switch to data
			if (readMetaBytes == metaLength) {
				// Persist previous meta if no meta content received
				if (metaLength > 0) {
					printf("d/ decoded meta %s\n", icymeta_text);
				}

				icymeta_text[readMetaBytes < ICY_META_BUFF_LEN - 1
								 ? readMetaBytes
								 : ICY_META_BUFF_LEN - 1] = 0;

				// Cleanup
				resetReader();
				resetMetaData(false);
				return buffLen;
			}

			if (buffLen <= 0) {
				break;
			}

			if (readMetaBytes < ICY_META_BUFF_LEN - 1) {
				// Metadata content can be added to meta buffer
				icymeta_text[readMetaBytes] = (*buff);
				++buff;
				--buffLen;
				++readMetaBytes;
				continue;
			} else {
				// Metadata content exceeds metadata buffer limit, consume all
				// metadata possible
				icymeta_text[ICY_META_BUFF_LEN - 1] = 0;
				int bytesToRead =
					dataInSegment ? metaLength - readMetaBytes : buffLen;

				buff += bytesToRead;
				buffLen -= bytesToRead;
				readMetaBytes += bytesToRead;
				return processMetaData(buff, buffLen);
			}
		}

	default:
		return 0;
	}
}

//Write bytes to the FIFO
void spiRamFifoWrite(const char *buff, int buffLen) {

	if (buffLen <= 0) {
		return;
	}

	if (newHttpRequest) {
		newHttpRequest = false;
		resetReader();
		resetMetaData(true);
	}

	// ================ Icy MetaData Processing ================

	int endMetaLength = 0;
	bool metaInBuffSegment = false;

	if (icymeta_interval > 0) {

		switch (fifoFlag) {
		case DATA:
			metaInBuffSegment = (icymeta_interval - readDataBytes) < buffLen;

			if (metaInBuffSegment) {
				endMetaLength = buffLen - (icymeta_interval - readDataBytes);
				buffLen = min(buffLen, icymeta_interval - readDataBytes);
			}
			break;

		default:;
			int metaBytesProcessed = buffLen - processMetaData(buff, buffLen);
			buff += metaBytesProcessed;
			buffLen -= metaBytesProcessed;
		}
	}

	// ================ Stream Data Processing ================

	int n;
	readDataBytes += buffLen;
	while (buffLen > 0) {
		n = buffLen;

		// don't read more than SPIREADSIZE
		if (n > SPIREADSIZE) n = SPIREADSIZE;

		// don't read past end of buffer
		if (n > (SPIRAMSIZE - fifoWpos)) {
			n = SPIRAMSIZE - fifoWpos;
		}

		xSemaphoreTake(mux, portMAX_DELAY);
		if ((SPIRAMSIZE - fifoFill) < n) {
			// printf("FIFO full.\n");
			// Drat, not enough free room in FIFO. Wait till there's some read and try again.
			fifoOvfCnt++;
			xSemaphoreGive(mux);
			xSemaphoreTake(semCanWrite, portMAX_DELAY);
			taskYIELD();
		} else {
			// Write the data.
			spiRamWrite(fifoWpos, buff, n);
			buff += n;
			buffLen -= n;
			fifoFill += n;
			fifoWpos += n;
			if (fifoWpos >= SPIRAMSIZE) fifoWpos = 0;
			xSemaphoreGive(mux);
			xSemaphoreGive(semCanRead); // Tell reader thread there's some data in the fifo.
		}
	}

	// ================ Icy MetaData Processing ================

	if (metaInBuffSegment && endMetaLength > 0) {
		fifoFlag = ICY_META_LENGTH;
		spiRamFifoWrite(buff, endMetaLength);
	}
}

//Get amount of bytes in use
int spiRamFifoFill() {
	int ret;
	xSemaphoreTake(mux, portMAX_DELAY);
	ret=fifoFill;
	xSemaphoreGive(mux);
	return ret;
}

int spiRamFifoFree() {
	return (SPIRAMSIZE-spiRamFifoFill());
}

int spiRamFifoLen() {
	return SPIRAMSIZE;
}

long spiRamGetOverrunCt() {
	long ret;
	xSemaphoreTake(mux, portMAX_DELAY);
	ret=fifoOvfCnt;
	xSemaphoreGive(mux);
	return ret;
}

long spiRamGetUnderrunCt() {
	long ret;
	xSemaphoreTake(mux, portMAX_DELAY);
	ret=fifoUdrCnt;
	xSemaphoreGive(mux);
	return ret;
}

