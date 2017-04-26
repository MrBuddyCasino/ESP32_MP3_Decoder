/*
	Arduino Audiocodecs

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

 /* The Helix-Library is modified for Teensy 3.1 */

 // Total RAM Usage: 31 KB

#include "play_sd_aac.h"
#include "common/assembly.h"

#define AAC_SD_BUF_SIZE	3072 								//Enough space for a complete stereo frame
//#define AAC_SD_BUF_SIZE	2560 								//Enough space for a complete stereo frame
//#define AAC_SD_BUF_SIZE	(1536 + 768)
#define AAC_BUF_SIZE	(AAC_MAX_NCHANS * AAC_MAX_NSAMPS)	//AAC output buffer

#define DECODE_NUM_STATES 2									//How many steps in decode() ?
#define CODEC_DEBUG

static AudioPlaySdAac 	*aacobjptr;
void decodeAac(void);

void AudioPlaySdAac::stop(void)
{
	NVIC_DISABLE_IRQ(IRQ_AUDIOCODEC);

	playing = codec_stopped;
	if (buf[1]) {free(buf[1]);buf[1] = NULL;}
	if (buf[0]) {free(buf[0]);buf[0] = NULL;}
	freeBuffer();
	if (hAACDecoder) {AACFreeDecoder(hAACDecoder);hAACDecoder=NULL;};
	fclose();
}

uint32_t AudioPlaySdAac::lengthMillis(void)
{
	if (duration)
		return duration;
	else
		return AudioCodec::lengthMillis();
}

//read big endian 16-Bit from fileposition(position)
uint16_t AudioPlaySdAac::fread16(size_t position)
{
	uint16_t tmp16;
	fseek(position);
	fread((uint8_t *) &tmp16, sizeof(tmp16));
	return REV16(tmp16);
}

//read big endian 32-Bit from fileposition(position)
uint32_t AudioPlaySdAac::fread32(size_t position)
{
	uint32_t tmp32;
	fseek(position);
	fread((uint8_t *) &tmp32, sizeof(tmp32));
	return REV32(tmp32);
}

_ATOM AudioPlaySdAac::findMp4Atom(const char *atom, const uint32_t posi, const bool loop = true)
{

	bool r;
	_ATOM ret;
	_ATOMINFO atomInfo;

	ret.position = posi;
	do
	{
		r = fseek(ret.position);
		fread((uint8_t *) &atomInfo, sizeof(atomInfo));
		ret.size = REV32(atomInfo.size);
		//ret.size = atomInfo.size;
		if (strncmp(atom, atomInfo.name, 4)==0){
			return ret;
		}
		ret.position += ret.size ;
	} while (loop && r);
	ret.position = 0;
	ret.size = 0;
	return ret;

}

bool AudioPlaySdAac::setupMp4(void)
{
	_ATOM ftyp = findMp4Atom("ftyp",0, false);
	if (!ftyp.size)
		return false; //no mp4/m4a file

	//go through the boxes to find the interesting atoms:
	uint32_t moov = findMp4Atom("moov", 0).position;
	uint32_t trak = findMp4Atom("trak", moov + 8).position;
	uint32_t mdia = findMp4Atom("mdia", trak + 8).position;

	//determine duration:
	uint32_t mdhd = findMp4Atom("mdhd", mdia + 8).position;
	uint32_t timescale = fread32(mdhd + 8 + 0x0c);
	duration = 1000.0 * ((float)fread32(mdhd + 8 + 0x10) / (float)timescale);

	//MP4-data has no aac-frames, so we have to set the parameters by hand.
	uint32_t minf = findMp4Atom("minf", mdia + 8).position;
	uint32_t stbl = findMp4Atom("stbl", minf + 8).position;
	//stsd sample description box: - infos to parametrize the decoder
	_ATOM stsd = findMp4Atom("stsd", stbl + 8);
	if (!stsd.size)
		return false; //something is not ok

	uint16_t channels = fread16(stsd.position + 8 + 0x20);
	//uint16_t channels = 1;
	//uint16_t bits		= fread16(stsd.position + 8 + 0x22); //not used
	uint16_t samplerate = fread32(stsd.position + 8 + 0x26);

	setupDecoder(channels, samplerate, AAC_PROFILE_LC);

	//stco - chunk offset atom:
	uint32_t stco = findMp4Atom("stco", stbl + 8).position;

	//number of chunks:
	uint32_t nChunks = fread32(stco + 8 + 0x04);
	//first entry from chunk table:
	firstChunk = fread32(stco + 8 + 0x08);
	//last entry from chunk table:
	lastChunk = fread32(stco + 8 + 0x04 + nChunks * 4);

	if (nChunks == 1) {
		_ATOM mdat =  findMp4Atom("mdat", 0);
		lastChunk = mdat.size;
	}

#if 0
	Serial.print("mdhd duration=");
	Serial.print(duration);
	Serial.print(" ms, stsd: chan=");
	Serial.print(channels);
	Serial.print(" samplerate=");
	Serial.print(samplerate);
	Serial.print(" nChunks=");
	Serial.print(nChunks);
	Serial.print(" firstChunk=");
	Serial.println(firstChunk, HEX);
	Serial.print(" lastChunk=");
	Serial.println(lastChunk, HEX);
#endif

	return true;
}

void AudioPlaySdAac::setupDecoder(int channels, int samplerate, int profile)
{
	memset(&aacFrameInfo, 0, sizeof(AACFrameInfo));
	aacFrameInfo.nChans = channels;
	//aacFrameInfo.bitsPerSample = bits; not used
	aacFrameInfo.sampRateCore = samplerate;
	aacFrameInfo.profile = AAC_PROFILE_LC;
	AACSetRawBlockParams(hAACDecoder, 0, &aacFrameInfo);
}

int AudioPlaySdAac::play(void){
	lastError = ERR_CODEC_NONE;
	initVars();
	sd_buf = allocBuffer(AAC_SD_BUF_SIZE);
	if (!sd_buf) return ERR_CODEC_OUT_OF_MEMORY;

	aacobjptr = this;

	buf[0] = (short *) malloc(AAC_BUF_SIZE * sizeof(int16_t));
	buf[1] = (short *) malloc(AAC_BUF_SIZE * sizeof(int16_t));

	hAACDecoder = AACInitDecoder();

	if (!buf[0] || !buf[1] || !hAACDecoder)
	{
		lastError = ERR_CODEC_OUT_OF_MEMORY;
		stop();
		return lastError;
	}

	isRAW = true;
	duration = 0;
	sd_left = 0;

	sd_p = sd_buf;

	if (setupMp4()) {
		fseek(firstChunk);
		sd_left = 0;
		isRAW = false;
		//Serial.print("mp4");
	}
	else { //NO MP4. Do we have an ID3TAG ?

		fseek(0);
		//Read-ahead 10 Bytes to detect ID3
		sd_left = fread(sd_buf, 10);
		//Skip ID3, if existent
		uint32_t skip = skipID3(sd_buf);
		if (skip) {
			size_id3 = skip;
			int b = skip & 0xfffffe00;
			fseek(b);
			sd_left = 0;
			//Serial.print("ID3");
		} else size_id3 = 0;
	}

	//Fill buffer from the beginning with fresh data
	sd_left = fillReadBuffer(file, sd_buf, sd_buf, sd_left, AAC_SD_BUF_SIZE);

	if (!sd_left) {
		lastError = ERR_CODEC_FILE_NOT_FOUND;
		stop();
		return lastError;
	}

	_VectorsRam[IRQ_AUDIOCODEC + 16] = &decodeAac;
	initSwi();

	decoded_length[0] = 0;
	decoded_length[1] = 0;
	decoding_state = 0;
	decoding_block = 0;

	for (int i=0; i< DECODE_NUM_STATES; i++) decodeAac();

	if((aacFrameInfo.sampRateOut != AUDIOCODECS_SAMPLE_RATE ) || (aacFrameInfo.nChans > 2)) {
		//Serial.println("incompatible AAC file.");
		lastError = ERR_CODEC_FORMAT;
		stop();
		return lastError;
	}

	decoding_block = 1;

	playing = codec_playing;
	
#ifdef CODEC_DEBUG
//	Serial.printf("RAM: %d\r\n",ram-freeRam());
#endif
    return lastError;
}

//runs in ISR
void AudioPlaySdAac::update(void)
{
	audio_block_t	*block_left;
	audio_block_t	*block_right;

	//paused or stopped ?
	if (playing  != codec_playing) return;

	//chain decoder-interrupt.
	//to give the user-sketch some cpu-time, only chain
	//if the swi is not active currently.
	//In addition, check before if there waits work for it.
	int db = decoding_block;
	if (!NVIC_IS_ACTIVE(IRQ_AUDIOCODEC))
		if (decoded_length[db]==0)
			NVIC_TRIGGER_INTERRUPT(IRQ_AUDIOCODEC);

	//determine the block we're playing from
	int playing_block = 1 - db;
	if (decoded_length[playing_block] <= 0)
		{
			stop();
			return;
		}

	// allocate the audio blocks to transmit
	block_left = allocate();
	if (block_left == NULL) return;

	int pl = play_pos;

	if (aacFrameInfo.nChans == 2) {
		// if we're playing stereo, allocate another
		// block for the right channel output
		block_right = allocate();
		if (block_right == NULL) {
			release(block_left);
			return;
		}

		memcpy_frominterleaved(block_left->data, block_right->data, buf[playing_block] + pl);

		pl += AUDIO_BLOCK_SAMPLES * 2 ;
		transmit(block_left, 0);
		transmit(block_right, 1);
		release(block_right);
		decoded_length[playing_block] -= AUDIO_BLOCK_SAMPLES * 2;

	} else
	{
		// if we're playing mono, no right-side block
		// let's do a (hopefully good optimized) simple memcpy
		memcpy(block_left->data, buf[playing_block] + pl, AUDIO_BLOCK_SAMPLES * sizeof(short));

		pl += AUDIO_BLOCK_SAMPLES;
		transmit(block_left, 0);
		transmit(block_left, 1);
		decoded_length[playing_block] -= AUDIO_BLOCK_SAMPLES;

	}

	samples_played += AUDIO_BLOCK_SAMPLES;

	release(block_left);

	//Switch to the next block if we have no data to play anymore:
	if ((decoded_length[playing_block] == 0) )
	{
		decoding_block = playing_block;
		play_pos = 0;
	} else
	play_pos = pl;

}

//decoding-interrupt
void decodeAac(void)
{
	AudioPlaySdAac *o = aacobjptr;
	int db = o->decoding_block;
	if (o->decoded_length[db]) return; //this block is playing, do NOT fill it

	uint32_t cycles = ARM_DWT_CYCCNT;
	int eof = false;

	switch (o->decoding_state) {

	case 0:
		{

			o->sd_left = o->fillReadBuffer( o->file, o->sd_buf, o->sd_p, o->sd_left, AAC_SD_BUF_SIZE);
			if (!o->sd_left) { eof = true; goto aacend; }
			o->sd_p = o->sd_buf;

			uint32_t cycles_rd = ARM_DWT_CYCCNT - cycles;
			if (cycles_rd > o->decode_cycles_max_read ) o->decode_cycles_max_read = cycles_rd;
			break;
		}

	case 1:
		{
			if (o->isRAW) {

				// find start of next AAC frame - assume EOF if no sync found
				int offset = AACFindSyncWord(o->sd_p, o->sd_left);

				if (offset < 0) {
					eof = true;
					goto aacend;
				}

				o->sd_p += offset;
				o->sd_left -= offset;

			}

			int decode_res = AACDecode(o->hAACDecoder, &o->sd_p, (int*)&o->sd_left, o->buf[db]);

			if (!decode_res) {
				AACGetLastFrameInfo(o->hAACDecoder, &o->aacFrameInfo);
				o->decoded_length[db] = o->aacFrameInfo.outputSamps;
			} else {
				AudioPlaySdAac::lastError = decode_res;
				eof = true;
				//goto aacend;
			}

			//if (!o->isRAW && (o->fposition() > o->lastChunk)) eof = true;

			cycles = ARM_DWT_CYCCNT - cycles;
			if (cycles > o->decode_cycles_max ) o->decode_cycles_max = cycles;
		}
	} //switch

aacend:

	o->decoding_state++;
	if (o->decoding_state >= DECODE_NUM_STATES) o->decoding_state = 0;

	if (eof) o->stop();

}
