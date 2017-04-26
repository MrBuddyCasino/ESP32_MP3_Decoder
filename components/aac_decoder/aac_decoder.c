/*
 * aac_decoder.c
 *
 *  Created on: 25.04.2017
 *      Author: michaelboeckling
 */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "esp_log.h"

#include "audio_renderer.h"
#include "aacdec_port.h"

#include "driver/i2s.h"
#include "spiram_fifo.h"

#define TAG "aac_decoder"

/* Enough space for a complete stereo frame */
#define AAC_SD_BUF_SIZE 3072

/* AAC output buffer */
#define AAC_BUF_SIZE    (AAC_MAX_NCHANS * AAC_MAX_NSAMPS)

uint8_t *sd_buf;
uint8_t *sd_pos;
int sd_left;

short *buf[2];
size_t decoded_length[2];
size_t decoding_block;

bool isRAW;     //true AAC(streamable)
uintptr_t play_pos;
size_t size_id3;
/* for MP4/M4A TODO: use for ID3 too */
uint32_t firstChunk;
uint32_t lastChunk;
unsigned duration;

HAACDecoder hAACDecoder;
AACFrameInfo aacFrameInfo;

/* DUMMY */
uint16_t fread16(size_t position)
{
    return 0;
}
uint32_t fread32(size_t position)
{
    return 0;
}
//Reverse byte order (32 bit) //FB
static inline unsigned int REV32(unsigned int value)
{
    return (((value >> 24) & 0xff) | // move byte 3 to byte 0
            ((value << 8) & 0xff0000) | // move byte 1 to byte 2
            ((value >> 8) & 0xff00) | // move byte 2 to byte 1
            ((value << 24) & 0xff000000)); // byte 0 to byte 3
}

typedef struct
{
    unsigned int position;
    unsigned int size;
} _ATOM;

typedef struct
{
    uint32_t size;
    char name[4];
} _ATOMINFO;

_ATOM findMp4Atom(const char *atom, const uint32_t posi, const bool loop)
{

    bool r;
    _ATOM ret;
    _ATOMINFO atomInfo;

    ret.position = posi;
    do {
        // init r as workaround
        r = 0;
        // r = fseek(ret.position);
        // fread((uint8_t *) &atomInfo, sizeof(atomInfo));
        ret.size = REV32(atomInfo.size);
        //ret.size = atomInfo.size;
        if (strncmp(atom, atomInfo.name, 4) == 0) {
            return ret;
        }
        ret.position += ret.size;
    } while (loop && r);
    ret.position = 0;
    ret.size = 0;
    return ret;
}

void setupDecoder(int channels, int samplerate, int profile)
{
    memset(&aacFrameInfo, 0, sizeof(AACFrameInfo));
    aacFrameInfo.nChans = channels;
    //aacFrameInfo.bitsPerSample = bits; not used
    aacFrameInfo.sampRateCore = samplerate;
    aacFrameInfo.profile = AAC_PROFILE_LC;
    AACSetRawBlockParams(hAACDecoder, 0, &aacFrameInfo);
}

bool setupMp4(void)
{
    _ATOM ftyp = findMp4Atom("ftyp", 0, false);
    if (!ftyp.size) return false; //no mp4/m4a file

    //go through the boxes to find the interesting atoms:
    uint32_t moov = findMp4Atom("moov", 0, true).position;
    uint32_t trak = findMp4Atom("trak", moov + 8, true).position;
    uint32_t mdia = findMp4Atom("mdia", trak + 8, true).position;

    //determine duration:
    uint32_t mdhd = findMp4Atom("mdhd", mdia + 8, true).position;
    uint32_t timescale = fread32(mdhd + 8 + 0x0c);
    duration = 1000.0 * ((float) fread32(mdhd + 8 + 0x10) / (float) timescale);

    //MP4-data has no aac-frames, so we have to set the parameters by hand.
    uint32_t minf = findMp4Atom("minf", mdia + 8, true).position;
    uint32_t stbl = findMp4Atom("stbl", minf + 8, true).position;
    //stsd sample description box: - infos to parametrize the decoder
    _ATOM stsd = findMp4Atom("stsd", stbl + 8, true);
    if (!stsd.size) return false; //something is not ok

    uint16_t channels = fread16(stsd.position + 8 + 0x20);
    //uint16_t channels = 1;
    //uint16_t bits     = fread16(stsd.position + 8 + 0x22); //not used
    uint16_t samplerate = fread32(stsd.position + 8 + 0x26);

    setupDecoder(channels, samplerate, AAC_PROFILE_LC);

    //stco - chunk offset atom:
    uint32_t stco = findMp4Atom("stco", stbl + 8, true).position;

    //number of chunks:
    uint32_t nChunks = fread32(stco + 8 + 0x04);
    //first entry from chunk table:
    firstChunk = fread32(stco + 8 + 0x08);
    //last entry from chunk table:
    lastChunk = fread32(stco + 8 + 0x04 + nChunks * 4);

    if (nChunks == 1) {
        _ATOM mdat = findMp4Atom("mdat", 0, true);
        lastChunk = mdat.size;
    }

#if 1
    printf("mdhd duration=%"PRIu32, duration);
    printf(" ms, stsd: chan=%"PRIu16, channels);
    printf(" samplerate=%"PRIu32, samplerate);
    printf(" nChunks=%"PRIu32, nChunks);
    printf(" firstChunk=%"PRIu32"\n", firstChunk);
    printf(" lastChunk=%"PRIu32"\n", lastChunk);
#endif

    return true;
}

//Skip ID3-Tags at the beginning of the file.
//http://id3.org/id3v2.4.0-structure
size_t skipID3(uint8_t *sd_buf)
{
    if (sd_buf[0] == 'I' && sd_buf[1] == 'D' && sd_buf[2] == '3'
            && sd_buf[3] < 0xff && sd_buf[4] < 0xff && sd_buf[6] < 0x80
            && sd_buf[7] < 0x80 && sd_buf[8] < 0x80 && sd_buf[9] < 0x80) {
        // bytes 6-9:offset of maindata, with bit.7=0:
        int ofs = ((sd_buf[6] & 0x7f) << 21) | ((sd_buf[7] & 0x7f) << 14)
                | ((sd_buf[8] & 0x7f) << 7) | (sd_buf[9] & 0x7f);
        return ofs;

    } else
        return 0;
}

void stop(void)
{
    ESP_LOGI(TAG, "stopping aac decoder");

    // playing = codec_stopped;
    if (buf[1]) {
        free(buf[1]);
        buf[1] = NULL;
    }
    if (buf[0]) {
        free(buf[0]);
        buf[0] = NULL;
    }
    // freeBuffer();
    if (hAACDecoder) {
        AACFreeDecoder(hAACDecoder);
        hAACDecoder = NULL;
    };
}

size_t fill_read_buffer(uint8_t *sd_buf, uint8_t *data, size_t data_left,
        size_t sd_bufsize)
{
    ESP_LOGI(TAG, "sd_buf %p sd_buf %p data_left %d", sd_buf, data, data_left);
    memmove(sd_buf, data, data_left);

    size_t space_left = sd_bufsize - data_left;
    size_t read = data_left;

    ESP_LOGI(TAG, "space_left %d", space_left);

    if (space_left > 0) {

        while (1) {
            size_t bytes_to_read = space_left;

            int fifo_fill = spiRamFifoFill();
            if (fifo_fill < space_left) bytes_to_read = fifo_fill;

            if (bytes_to_read == 0) {
                printf("Buffer underflow, need %d bytes.\n", space_left);
            } else {
                spiRamFifoRead((char *) sd_buf + data_left, bytes_to_read);
                data_left += bytes_to_read;
                read += bytes_to_read;

                if (bytes_to_read < space_left) { //Rest mit 0 füllen (EOF)
                    memset(sd_buf + data_left, 0, sd_bufsize - data_left);
                }

                if (data_left >= sd_bufsize) {
                    break;
                }
            }
        }

    }

    return read;
}

static int convert_16bit_stereo_to_16bit_stereo(short left, short right)
{
    unsigned int sample = (unsigned short) left;
    sample = (sample << 16 & 0xffff0000) | ((unsigned short) right);
    return sample;
}

//decoding-interrupt
void decodeAac(void)
{
    int db = decoding_block;
    int eof = false;

    // max delay: 50ms instead of portMAX_DELAY
    TickType_t delay = 50 / portTICK_PERIOD_MS;

    bool first_run = true;
    while (1) {

        sd_left = fill_read_buffer(sd_buf, sd_pos, sd_left, AAC_SD_BUF_SIZE);

        // skip headers
        if (first_run) {
            char *hdr_offfset = strstr((char*) sd_buf, "\r\n\r\n");
            if (hdr_offfset) {
                hdr_offfset += 4;
                uint8_t *sd_pos_old = sd_pos;
                sd_pos = (uint8_t*) hdr_offfset;
                sd_left -= (sd_pos - sd_pos_old);
                ESP_LOGI(TAG, "hdr_offfset %p sd_pos_old %p sd_pos %p sd_left %d", hdr_offfset, sd_pos_old, sd_pos, sd_left);
            }
            first_run = false;
        }

        if (isRAW) {
            // find start of next AAC frame - assume EOF if no sync found
            int offset = AACFindSyncWord(sd_pos, sd_left);
            ESP_LOGI(TAG, "AACFindSyncWord offset %d", offset);

            if (offset < 0) {
                eof = true;
                goto aacend;
            }

            sd_pos += offset;
            sd_left -= offset;
        }

        int decode_res = AACDecode(hAACDecoder, &sd_pos, (int*) &sd_left,
                buf[db]);

        ESP_LOGI(TAG, "AACDecode decode_res %d", decode_res);

        if (!decode_res) {
            AACGetLastFrameInfo(hAACDecoder, &aacFrameInfo);
            decoded_length[db] = aacFrameInfo.outputSamps;

            // render
            for (int i = 0; i < aacFrameInfo.outputSamps; i = i + 2) {

                // uint32_t samp16 = buf[db][i];
                uint32_t samp16 = convert_16bit_stereo_to_16bit_stereo(
                        buf[db][i], buf[db][i + 1]);

                int bytes_pushed = i2s_push_sample(I2S_NUM_0, (char *) &samp16,
                        delay);

                // DMA buffer full - retry
                if (bytes_pushed == 0) {
                    i = i - 2;
                }
            }

        } else {
            eof = true;
        }
    }

    aacend: if (eof) stop();
}

void aac_decoder_task(void *pvParameters)
{
    sd_buf = calloc(AAC_SD_BUF_SIZE, sizeof(uint8_t));

    buf[0] = (short *) malloc(AAC_BUF_SIZE * sizeof(int16_t));
    buf[1] = (short *) malloc(AAC_BUF_SIZE * sizeof(int16_t));

    hAACDecoder = AACInitDecoder();

    isRAW = true;
    duration = 0;
    sd_left = 0;
    sd_pos = sd_buf;

    /*
     if (setupMp4()) {
     fseek(firstChunk);
     sd_left = 0;
     isRAW = false;
     ESP_LOGI(TAG, "mp4");
     } else {
     ESP_LOGI(TAG, "raw");
     //NO MP4. Do we have an ID3TAG ?

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
     ESP_LOGI(TAG, "ID3");
     } else {
     size_id3 = 0;
     }
     }
     */

    decodeAac();
}

