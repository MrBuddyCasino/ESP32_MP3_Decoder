/*
 * libfaad_decoder.c
 *
 * based on the Rockbox aac.c decoder by Dave Chapman
 *
 *  Created on: 26.04.2017
 *      Author: michaelboeckling
 */

#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "driver/i2s.h"

#include "neaacdec.h"
//#include "../libfaad/common.h"
//typedef float real_t;
//typedef real_t complex_t[2];
//#include "../libfaad/structs.h"

#include "common_buffer.h"
#include "m4a.h"
#include "audio_renderer.h"
#include "audio_player.h"
#include "spiram_fifo.h"

#define CODEC_ERROR -1
#define FAAD_BYTE_BUFFER_SIZE (2048-12)
#define TAG "libfaad_dec"

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

void print_buffer(buffer_t *buf)
{
    size_t data_left = buf->write_pos - buf->read_pos;
    size_t space_left = buf->len - data_left;
    ESP_LOGI(TAG, "fill %d capacity %d", data_left, space_left);
}


typedef enum {
    CONTAINER_RAW = 1,
    CONTAINER_MP4 = 2
} container_format_t;


void print_frame_info(NeAACDecFrameInfo *frame_info)
{
    if ((frame_info->error == 0) && (frame_info->samples > 0)) {
      printf("decoded %lu samples\n", frame_info->samples);
      printf("  bytesconsumed: %lu\n", frame_info->bytesconsumed);
      printf("  channels: %d\n", frame_info->channels);
      printf("  samplerate: %lu\n", frame_info->samplerate);
      printf("  sbr: %u\n", frame_info->sbr);
      printf("  object_type: %u\n", frame_info->object_type);
      printf("  header_type: %u\n", frame_info->header_type);
      printf("  num_front_channels: %u\n", frame_info->num_front_channels);
      printf("  num_side_channels: %u\n", frame_info->num_side_channels);
      printf("  num_back_channels: %u\n", frame_info->num_back_channels);
      printf("  num_lfe_channels: %u\n", frame_info->num_lfe_channels);
      printf("  ps: %u\n", frame_info->ps);
      printf("\n");
    }
}


void libfaac_decoder_task(void *pvParameters)
{

    player_t *player = pvParameters;
    /* Note that when dealing with QuickTime/MPEG4 files, terminology is
     * a bit confusing. Files with sound are split up in chunks, where
     * each chunk contains one or more samples. Each sample in turn
     * contains a number of "sound samples" (the kind you refer to with
     * the sampling frequency).
     */
    size_t n;
    demux_res_t demux_res;
    stream_t input_stream;
    uint32_t sound_samples_done;
    uint32_t elapsed_time;
    int file_offset;
    int framelength;
    int lead_trim = 0;
    unsigned int frame_samples;
    unsigned int i;
    unsigned char* buffer;
    NeAACDecFrameInfo frame_info;
    NeAACDecHandle decoder;
    int err;
    uint32_t seek_idx = 0;
    unsigned long samp_rate = 0;
    uint32_t sbr_fac = 1;
    unsigned char chan = 0;
    void *ret;
    // enum codec_command_action action;
    intptr_t param;
    bool empty_first_frame = false;


    buffer_t buf = {
        .len=FAAD_BYTE_BUFFER_SIZE
    };
    buf.base = calloc(FAAD_BYTE_BUFFER_SIZE, sizeof(uint8_t));
    buf.read_pos = buf.base;
    buf.write_pos = buf.base;

    /* Clean and initialize decoder structures */
    memset(&demux_res, 0, sizeof(demux_res));

    stream_create(&input_stream, &buf);
    fill_read_buffer(&buf);

    //for(uint8_t *i = buf.read_pos; i < buf.fill_pos; i++)
    //    printf("%X", (*i));

    content_type_t content_type =  player->media_stream->content_type;
    ESP_LOGI(TAG, "content_type: %d", content_type);

    if(content_type == AUDIO_MP4) {
        /* if qtmovie_read returns successfully, the stream is up to
         * the movie data, which can be used directly by the decoder */
         if (!qtmovie_read(&input_stream, &demux_res)) {
             ESP_LOGE(TAG, "FAAD: File init error\n");
             // return CODEC_ERROR;
             return;
         } else {
             ESP_LOGI(TAG, "qtmovie_read success");
         }
    } else if(content_type == AUDIO_AAC || content_type == OCTET_STREAM) {
        memcpy(demux_res.codecdata, buf.read_pos, 64);
        demux_res.codecdata_len = 64;
    } else {
        ESP_LOGE(TAG, "unsupported content-type: %d", content_type);
        return;
    }

    /* initialise the sound converter */
    decoder = NeAACDecOpen();
    if (!decoder) {
        ESP_LOGE(TAG, "FAAD: Decode open error");
        return ;
    }

    NeAACDecConfigurationPtr conf = NeAACDecGetCurrentConfiguration(decoder);
    renderer_config_t *renderer = renderer_get();
    switch(renderer->bit_depth) {
        case I2S_BITS_PER_SAMPLE_8BIT:
        case I2S_BITS_PER_SAMPLE_16BIT:
            conf->outputFormat = FAAD_FMT_16BIT;
            break;
        case I2S_BITS_PER_SAMPLE_24BIT:
            conf->outputFormat = FAAD_FMT_24BIT;
            break;
        case I2S_BITS_PER_SAMPLE_32BIT:
            conf->outputFormat = FAAD_FMT_32BIT;
            break;
    }
    conf->defObjectType = LC;
    conf->defSampleRate = 44100;
    NeAACDecSetConfiguration(decoder, conf);


    if(content_type == AUDIO_MP4) {
        err = NeAACDecInit2(decoder, demux_res.codecdata, demux_res.codecdata_len, &samp_rate, &chan);
    } else {
        err = NeAACDecInit(decoder, demux_res.codecdata, demux_res.codecdata_len, &samp_rate, &chan);
    }

    if (err) {
        //LOGF("FAAD: DecInit: %d, %d\n", err, decoder->object_type);
        ESP_LOGE(TAG, "FAAD: DecInit: %d", err);
        return ;
    }

#ifdef SBR_DEC
    /* Check for need of special handling for seek/resume and elapsed time. */
    /*
     if (ci->id3->needs_upsampling_correction) {
     sbr_fac = 2;
     } else {
     sbr_fac = 1;
     }
     */
#endif

    pcm_format_t pcm_fmt = {
        .sample_rate = samp_rate,
        .bit_depth = I2S_BITS_PER_SAMPLE_16BIT,
        .num_channels = chan,
        .buffer_format = PCM_INTERLEAVED
    };

    ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());

    while (!player->media_stream->eof) {

        /* Request the required number of bytes from the input buffer */
        fill_read_buffer(&buf);

        /* Decode one block - returned samples will be host-endian */
        ret = NeAACDecDecode(decoder, &frame_info, buf.read_pos,
                buf.write_pos - buf.read_pos);

        /* NeAACDecDecode may sometimes return NULL without setting error. */
        if (ret == NULL || frame_info.error > 0) {
            printf("FAAD: decode error '%s'\n",
                    NeAACDecGetErrorMessage(frame_info.error));
            return;
        }

        /* Advance codec buffer (no need to call set_offset because of this) */
        buf_seek_rel(&buf, frame_info.bytesconsumed);

        /* Output the audio */
        frame_samples = frame_info.samples >> 1;
        framelength = frame_samples - lead_trim;

        char *pcm_buf = ret;
        render_samples(pcm_buf, frame_info.samples * 2, &pcm_fmt);

        // ESP_LOGI(TAG, "stack: %d\n", uxTaskGetStackHighWaterMark(NULL));
    }

    NeAACDecClose(decoder);
}

