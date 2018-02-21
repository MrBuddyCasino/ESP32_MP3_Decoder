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
 *     2018/02/16, icy-meta-parser in spiRamFifoWrite created by reini
*******************************************************************************/
#include "esp_system.h"
#include "string.h"
#include <stdio.h>
#include "http.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

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
int flagfifo=0,rambytes=0,metamax=0,it;//sync icy-metaint:
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

//Write bytes to the FIFO and pars icy-metadata
void spiRamFifoWrite(const char *buff, int buffLen) {
      int n,fimax,i=0,r1=0,r2=0,r3=1,text=0;
          n = buffLen;
          fimax=buffLen;
          buffLen=0;

              if(httpmeta==1) {   //if new http-request
                   metamax=icymeta;
                   httpmeta=0;
                   rambytes=0;
                   flagfifo=0;
                   it=-1;
                   icymeta_text[0]=0;
                   icymeta_text[62]=0;
               }
                      if(metamax>0) {  // if icy-metaint: Data available
                         r1=metamax-rambytes;
                          if(flagfifo==0&&(r1-n)<0) {
                            buffLen=r1;
                            i=r1+1;
                            r2=buff[r1];
                            if(r2!=0) {
                               flagfifo=2;
                               //printf("StreamTitle_found \n");//only for testing
                               it=-1;
                             }   else  {
                                   //printf("Stream-sync ok \n");
                                   r3=0;
                                   goto next1;
                                }
                           }

           rambytes +=n;
               
              if(flagfifo>0) {
                   while (buff[i]!=0&&i<fimax&&flagfifo==2) {
                      //printf("%c",buff[i]);         //Monitor StreamTitle only for testing
                      if(it>=0&&it<59) {
                         if(buff[i]==0x3b)  //first ";" set End of text
                            icymeta_text[it] =0;
                              else 
                                icymeta_text[it] =buff[i];
                                it++;
                      }         if(it<0&&buff[i]==0x27) //start with '
                                  it++;
                                  i++;
                   }
                                    if(i<fimax)
                                      flagfifo=1;

                                      if(flagfifo==1) {
                                        icymeta_text[63]=0;      //set End of text
                                        while(it<62) {icymeta_text[it]=0x20;it++;};       
                                        icymeta_text[62]=0xff; //flag for text available
                                       }  

                 while (buff[i]==0&&i<fimax&&flagfifo==1) {i++;if(buff[i]!=0) flagfifo=0;};
                 next1:;
                 rambytes=n-i;
              }

       fimax =fimax-i;
       text =i-buffLen;

       if(buffLen==0) {
          if(fimax<=0)    {
             buff += n;
             goto ende1;  };
            
             buffLen=fimax;
             fimax=0;
             buff += text;
        }
                     }
                         else             { //if no icy-metaint: , normal fifo mode
                                text=1;
                                buffLen=n;}

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
      if(buffLen <=0&&metamax!=0)   {
         if((text<=1||fimax<=1)&&r3>0)
            break;       
         buff +=text;
         buffLen=fimax;
         text=1;
         r3++;
       }
	}
ende1:;
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

