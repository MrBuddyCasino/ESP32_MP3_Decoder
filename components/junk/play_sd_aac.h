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

#ifndef play_sd_aac_h_
#define play_sd_aac_h_

#include "codecs.h"
#include "AudioStream.h"
#include "spi_interrupt.h"
#include "aac/aacdec.h"

struct _ATOM		{unsigned int position;unsigned int size;};
struct _ATOMINFO	{uint32_t size;char name[4];};

//class AudioPlaySdAac : public AudioStream
class AudioPlaySdAac : public AudioCodec
{
public:
	//AudioPlaySdAac(void) : AudioStream(0, NULL) {}
	int play(const char *filename) {stop();if (!fopen(filename)) return ERR_CODEC_FILE_NOT_FOUND; return play();}
	int play(const size_t p, const size_t size) {stop();if (!fopen(p,size)) return ERR_CODEC_FILE_NOT_FOUND; return play();}
	int play(const uint8_t*p, const size_t size) {stop();if (!fopen(p,size))  return ERR_CODEC_FILE_NOT_FOUND; return play();}
	void stop(void);

	uint32_t lengthMillis(void);

protected:

	uint8_t			*sd_buf;
	uint8_t			*sd_p;
	int				sd_left;

	short			*buf[2];
	size_t			decoded_length[2];
	size_t			decoding_block;
	unsigned int	decoding_state; //state 0: read sd, state 1: decode

	bool isRAW;		//true AAC(streamable)
	uintptr_t		play_pos;
	size_t 			size_id3;
	uint32_t 		firstChunk, lastChunk;	//for MP4/M4A //TODO: use for ID3 too
	unsigned		duration;

	HAACDecoder		hAACDecoder;
	AACFrameInfo	aacFrameInfo;

	int play(void);
	uint16_t fread16(size_t position);
	uint32_t fread32(size_t position);
	void setupDecoder(int channels, int samplerate, int profile);
	_ATOM findMp4Atom(const char *atom, const uint32_t posi, const bool loop);
	bool setupMp4(void);
	void update(void);
	friend void decodeAac(void);
};


#endif
