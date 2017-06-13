/*
 * mp3_decoder.c
 *
 *  Created on: 13.03.2017
 *      Author: michaelboeckling
 */

#include <stdlib.h>
#include <string.h>

#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "../mad/mad.h"
#include "../mad/stream.h"
#include "../mad/frame.h"
#include "../mad/synth.h"

#include "driver/i2s.h"
#include "audio_renderer.h"
#include "audio_player.h"
#include "spiram_fifo.h"
#include "mp3_decoder.h"
#include "common_buffer.h"

#define TAG "mad_decoder"

// The theoretical maximum frame size is 2881 bytes,
// MPEG 2.5 Layer II, 8000 Hz @ 160 kbps, with a padding slot plus 8 byte MAD_BUFFER_GUARD.
#define MAX_FRAME_SIZE (2889)

// The theoretical minimum frame size of 24 plus 8 byte MAD_BUFFER_GUARD.
#define MIN_FRAME_SIZE (32)

static long buf_underrun_cnt;

/* default MAD buffer format */
pcm_format_t mad_buffer_fmt = {
    .sample_rate = 44100,
    .bit_depth = I2S_BITS_PER_SAMPLE_16BIT,
    .num_channels = 2,
    .buffer_format = PCM_LEFT_RIGHT
};

static enum mad_flow input(struct mad_stream *stream, buffer_t *buf, player_t *player)
{
    int bytes_to_read;

    // next_frame is the position MAD is interested in resuming from
    uint32_t bytes_consumed  = stream->next_frame - stream->buffer;
    buf_seek_rel(buf, bytes_consumed);

    while (1) {

        // stop requested, terminate immediately
        if(player->decoder_command == CMD_STOP) {
            return MAD_FLOW_STOP;
        }

        // Calculate amount of bytes we need to fill buffer.
        bytes_to_read = min(buf_free_capacity_after_purge(buf), spiRamFifoFill());

        // Can't take anything?
        if (bytes_to_read == 0) {

            // EOF reached, stop decoder when all frames have been consumed
            if(player->media_stream->eof) {
                return MAD_FLOW_STOP;
            }

            //Wait until there is enough data in the buffer. This only happens when the data feed
            //rate is too low, and shouldn't normally be needed!
            ESP_LOGE(TAG, "Buffer underflow, need %d bytes.", buf_free_capacity_after_purge(buf));
            buf_underrun_cnt++;
            //We both silence the output as well as wait a while by pushing silent samples into the i2s system.
            //This waits for about 200mS
            renderer_zero_dma_buffer();
        } else {
            //Read some bytes from the FIFO to re-fill the buffer.
            fill_read_buffer(buf);

            // break the loop if we have at least enough to decode a few of the smallest possible frame
            if(buf_data_unread(buf) >= (MIN_FRAME_SIZE * 4))
                break;
        }
    }

    // Okay, let MAD decode the buffer.
    mad_stream_buffer(stream, (unsigned char*) buf->read_pos, buf_data_unread(buf));
    return MAD_FLOW_CONTINUE;
}


//Routine to print out an error
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame) {
    ESP_LOGE(TAG, "dec err 0x%04x (%s)", stream->error, mad_stream_errorstr(stream));
    return MAD_FLOW_CONTINUE;
}



//This is the main mp3 decoding task. It will grab data from the input buffer FIFO in the SPI ram and
//output it to the I2S port.
void mp3_decoder_task(void *pvParameters)
{
    player_t *player = pvParameters;

    int ret;
    struct mad_stream *stream;
    struct mad_frame *frame;
    struct mad_synth *synth;

    //Allocate structs needed for mp3 decoding
    stream = malloc(sizeof(struct mad_stream));
    frame = malloc(sizeof(struct mad_frame));
    synth = malloc(sizeof(struct mad_synth));
    buffer_t *buf = buf_create(MAX_FRAME_SIZE);

    if (stream==NULL) { ESP_LOGE(TAG, "malloc(stream) failed\n"); return; }
    if (synth==NULL) { ESP_LOGE(TAG, "malloc(synth) failed\n"); return; }
    if (frame==NULL) { ESP_LOGE(TAG, "malloc(frame) failed\n"); return; }
    if (buf==NULL) { ESP_LOGE(TAG, "buf_create() failed\n"); return; }

    buf_underrun_cnt = 0;

    ESP_LOGI(TAG, "decoder start");

    //Initialize mp3 parts
    mad_stream_init(stream);
    mad_frame_init(frame);
    mad_synth_init(synth);


    while(1) {

        // calls mad_stream_buffer internally
        if (input(stream, buf, player) == MAD_FLOW_STOP ) {
            break;
        }

        // decode frames until MAD complains
        while(1) {

            if(player->decoder_command == CMD_STOP) {
                goto abort;
            }

            // returns 0 or -1
            ret = mad_frame_decode(frame, stream);
            if (ret == -1) {
                if (!MAD_RECOVERABLE(stream->error)) {
                    //We're most likely out of buffer and need to call input() again
                    break;
                }
                error(NULL, stream, frame);
                continue;
            }
            mad_synth_frame(synth, frame);
        }
        // ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());
    }

    abort:
    // avoid noise
    renderer_zero_dma_buffer();

    free(synth);
    free(frame);
    free(stream);
    buf_destroy(buf);

    // clear semaphore for reader task
    spiRamFifoReset();

    player->decoder_status = STOPPED;
    player->decoder_command = CMD_NONE;
    ESP_LOGI(TAG, "decoder stopped");

    ESP_LOGI(TAG, "MAD decoder stack: %d\n", uxTaskGetStackHighWaterMark(NULL));
    vTaskDelete(NULL);
}

/* Called by the NXP modifications of libmad. Sets the needed output sample rate. */
void set_dac_sample_rate(int rate)
{
    mad_buffer_fmt.sample_rate = rate;
}

/* render callback for the libmad synth */
void render_sample_block(short *sample_buff_ch0, short *sample_buff_ch1, int num_samples, unsigned int num_channels)
{
    mad_buffer_fmt.num_channels = num_channels;
    uint32_t len = num_samples * sizeof(short) * num_channels;
    render_samples((char*) sample_buff_ch0, len, &mad_buffer_fmt);
    return;
}

