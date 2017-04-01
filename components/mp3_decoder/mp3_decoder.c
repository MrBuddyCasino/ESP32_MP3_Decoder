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

#define TAG "decoder"

// The theoretical maximum frame size is 2881 bytes,
// MPEG 2.5 Layer II, 8000 Hz @ 160 kbps, with a padding slot plus 8 byte MAD_BUFFER_GUARD.
#define MAX_FRAME_SIZE (2889)

// The theoretical minimum frame size of 24 plus 8 byte MAD_BUFFER_GUARD.
#define MIN_FRAME_SIZE (32)

// read buffer
static char read_buf[MAX_FRAME_SIZE];

static long buf_underrun_cnt;


static enum mad_flow input(struct mad_stream *stream, player_t *player) {
    int readable_bytes, fifo_fill;
    int rem;
    //Shift remaining contents of buf to the front
    rem = stream->bufend - stream->next_frame;
    memmove(read_buf, stream->next_frame, rem);

    //while (rem < sizeof(read_buf)) {
    while (1) {

        // stop requested, terminate immediately
        if(player->state == STOPPED)
            return MAD_FLOW_STOP;

        readable_bytes = (sizeof(read_buf) - rem);    // Calculate amount of bytes we need to fill buffer.
        fifo_fill = spiRamFifoFill();
        if (fifo_fill < readable_bytes)
            readable_bytes = fifo_fill;                 // If the fifo can give us less, only take that amount

        // Can't take anything?
        if (readable_bytes == 0) {

            // EOF reached, stop decoder when all frames have been consumed
            if(player->state == FINISHED) {
                // clear the buffer
                i2s_zero_dma_buffer(player->renderer_config->i2s_num);
                return MAD_FLOW_STOP;
            }

            //Wait until there is enough data in the buffer. This only happens when the data feed
            //rate is too low, and shouldn't normally be needed!
            printf("Buffer underflow, need %d bytes.\n", sizeof(read_buf) - rem);
            buf_underrun_cnt++;
            //We both silence the output as well as wait a while by pushing silent samples into the i2s system.
            //This waits for about 200mS
            i2s_zero_dma_buffer(player->renderer_config->i2s_num);
        } else {
            //Read some bytes from the FIFO to re-fill the buffer.
            spiRamFifoRead(&read_buf[rem], readable_bytes);
            rem += readable_bytes;

            // break the loop if we have at least enough to decode the smallest possible frame
            if(rem >= MIN_FRAME_SIZE)
                break;
        }
    }

    // Okay, let MAD decode the buffer.
    mad_stream_buffer(stream, (unsigned char*) read_buf, sizeof(read_buf));
    return MAD_FLOW_CONTINUE;
}


//Routine to print out an error
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame) {
    printf("dec err 0x%04x (%s)\n", stream->error, mad_stream_errorstr(stream));
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

    if (stream==NULL) { printf("MAD: malloc(stream) failed\n"); return; }
    if (synth==NULL) { printf("MAD: malloc(synth) failed\n"); return; }
    if (frame==NULL) { printf("MAD: malloc(frame) failed\n"); return; }

    buf_underrun_cnt = 0;

    printf("MAD: Decoder start.\n");

    //Initialize mp3 parts
    mad_stream_init(stream);
    mad_frame_init(frame);
    mad_synth_init(synth);

    while(player->state != STOPPED) {

        // calls mad_stream_buffer internally
        if (input(stream, player) == MAD_FLOW_STOP ) {
            break;
        }

        // decode frames until MAD complains
        while(player->state != STOPPED) {
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
        // ESP_LOGI(TAG, "MAD decoder stack: %d\n", uxTaskGetStackHighWaterMark(NULL));
        // ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());
    }

    free(synth);
    free(frame);
    free(stream);

    // clear semaphore for reader task
    spiRamFifoReset();

    printf("MAD: Decoder stopped.\n");
    vTaskDelete(NULL);
}

