/*
	Helix library Arduino Audio Library MP3/AAC objects

	Copyright (c) 2014 Frank Bösing

	This library is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this library.  If not, see <http://www.gnu.org/licenses/>.

	The helix decoder itself as a different license, look at the subdirectories for more info.

	Diese Bibliothek ist freie Software: Sie können es unter den Bedingungen
	der GNU General Public License, wie von der Free Software Foundation,
	Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
	veröffentlichten Version, weiterverbreiten und/oder modifizieren.

	Diese Bibliothek wird in der Hoffnung, dass es nützlich sein wird, aber
	OHNE JEDE GEWÄHRLEISTUNG, bereitgestellt; sogar ohne die implizite
	Gewährleistung der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
	Siehe die GNU General Public License für weitere Details.

	Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
	Programm erhalten haben. Wenn nicht, siehe <http://www.gnu.org/licenses/>.

	Der Helixdecoder selbst hat eine eigene Lizenz, bitte für mehr Informationen
	in den Unterverzeichnissen nachsehen.

 */

#if !defined(__MK20DX256__) && !defined(__MK64FX512__) && !defined(__MK66FX1M0__)
#error	This platform is not supported.
#endif

#ifndef codecs_h_
#define codecs_h_

#include <AudioStream.h>
#include <spi_interrupt.h>
#include <SD.h>

#define ERR_CODEC_NONE				0
#define ERR_CODEC_FILE_NOT_FOUND    1
#define ERR_CODEC_OUT_OF_MEMORY     2
#define ERR_CODEC_FORMAT			3	//File is not 44.1 KHz, 16Bit mono or stereo

#define IRQ_AUDIO			IRQ_SOFTWARE	// see AudioStream.cpp
#define IRQ_AUDIOCODEC		55				// use a "reserved" (free) interrupt vector
#define IRQ_AUDIOCODEC_PRIO	240				// lowest priority

#define AUDIOCODECS_SAMPLE_RATE			(((int)(AUDIO_SAMPLE_RATE / 100)) * 100) //44100

#define NVIC_STIR			(*(volatile uint32_t *)0xE000EF00) //Software Trigger Interrupt Register
#define NVIC_TRIGGER_INTERRUPT(x)    NVIC_STIR=(x)
#define NVIC_IS_ACTIVE(n)	(*((volatile uint32_t *)0xE000E300 + ((n) >> 5)) & (1 << ((n) & 31)))

#define PATCH_PRIO 			{if (NVIC_GET_PRIORITY(IRQ_AUDIO) == IRQ_AUDIOCODEC_PRIO) {NVIC_SET_PRIORITY(IRQ_AUDIO, IRQ_AUDIOCODEC_PRIO-16);}}

#define SERFLASH_CS 				6	//Chip Select W25Q128FV SPI Flash
#define SPICLOCK 			30000000

extern "C" { void memcpy_frominterleaved(int16_t *dst1, int16_t *dst2, int16_t *src); }
size_t skipID3(uint8_t *sd_buf);

enum codec_filetype {codec_none, codec_file, codec_flash, codec_serflash};
enum codec_playstate {codec_stopped, codec_playing, codec_paused};

class CodecFile
{
public:

	bool fopen(const char *filename) {ftype=codec_file; AudioStartUsingSPI(); fptr=NULL; file=SD.open(filename); _fsize=file.size(); _fposition=0; return file != 0;} //FILE
	bool fopen(const uint8_t*p, const size_t size) {ftype=codec_flash; fptr=(uint8_t*)p; _fsize=size; _fposition=0; return true;} //FLASH
	bool fopen(const size_t p, const size_t size) {ftype=codec_serflash; offset=p; _fsize=size; _fposition=0; AudioStartUsingSPI(); serflashinit(); return true;} //SERIAL FLASH
	void fclose(void)
	{
		_fsize=_fposition=0; fptr=NULL;
		if (ftype==codec_file) {file.close(); AudioStopUsingSPI();}
		else
		if (ftype==codec_serflash) {AudioStopUsingSPI();}
		ftype=codec_none;
	}
	bool f_eof(void) {return _fposition >= _fsize;}
	bool fseek(const size_t position) {_fposition=position;if (ftype==codec_file) return file.seek(_fposition)!=0; else return _fposition <= _fsize;}
	size_t fposition(void) {return _fposition;}
	size_t fsize(void) {return _fsize;}
	size_t fread(uint8_t buffer[],size_t bytes);

	uint8_t *allocBuffer(size_t size) { rdbufsize = size;  bufptr = (uint8_t *) calloc(size,1); return bufptr;}
	void freeBuffer(void){ if (bufptr !=NULL) {free(bufptr);bufptr = NULL; } rdbufsize = 0;}
	size_t fillReadBuffer(File file, uint8_t *sd_buf, uint8_t *data, size_t dataLeft, size_t sd_bufsize);
	//size_t fillReadBuffer(uint8_t *data, size_t dataLeft);

protected:
//private:

	void serflashinit(void);
	void readserflash(uint8_t* buffer, const size_t position, const size_t bytes);

	SPISettings spisettings;

	codec_filetype ftype;

	File file;
	union {
		uint8_t* fptr;
		size_t offset;
	};

	size_t _fsize;
	size_t _fposition;

	uint8_t* bufptr;
	size_t rdbufsize;
};

class AudioCodec : public AudioStream, protected CodecFile
{
public:

	AudioCodec(void) : AudioStream(0, NULL) {initVars();}
	bool pause(const bool paused);
	bool isPlaying(void) {return playing > 0;}
	unsigned positionMillis(void) { return (AUDIO_SAMPLE_RATE_EXACT / 1000) * samples_played;}
	unsigned lengthMillis(void) {return max(fsize() / (bitrate / 8 ) * 1000,  positionMillis());} //Ignores VBR
	int channels(void) {return _channels;}
	int bitRate(void) {return bitrate;}
	void processorUsageMaxResetDecoder(void){__disable_irq();decode_cycles_max = decode_cycles_max_read = 0;__enable_irq();}
	int freeRam(void);

	static short	lastError;

	static unsigned decode_cycles;
	static unsigned	decode_cycles_read;
	static unsigned decode_cycles_max;
	static unsigned decode_cycles_max_read;

protected:

	unsigned		samples_played;

	unsigned short	_channels;
	unsigned short	bitrate;

	volatile codec_playstate playing;

	void initVars(void) {samples_played=_channels=bitrate=decode_cycles=decode_cycles_read=decode_cycles_max=decode_cycles_max_read = 0;playing=codec_stopped;}
	void initSwi(void) {PATCH_PRIO;NVIC_SET_PRIORITY(IRQ_AUDIOCODEC, IRQ_AUDIOCODEC_PRIO);NVIC_ENABLE_IRQ(IRQ_AUDIOCODEC);}

};

#endif
