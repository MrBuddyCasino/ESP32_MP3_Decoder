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

#define TAG "decoder"
//The mp3 read buffer size. 2106 bytes should be enough for up to 48KHz mp3s according to the sox sources. Used by libmad.
#define READBUFSZ (2106)
static char readBuf[READBUFSZ];

static long bufUnderrunCt;
const uint32_t zero_sample[2] = { 0 };


static enum mad_flow input(struct mad_stream *stream, player_t *player) {
    int n, i;
    int rem;
    //Shift remaining contents of buf to the front
    rem = stream->bufend - stream->next_frame;
    memmove(readBuf, stream->next_frame, rem);

    while (rem < sizeof(readBuf)) {
        n = (sizeof(readBuf) - rem);    //Calculate amount of bytes we need to fill buffer.
        i = spiRamFifoFill();
        if (i < n) n=i;                 //If the fifo can give us less, only take that amount
        if (n==0) {                 //Can't take anything?
            //Wait until there is enough data in the buffer. This only happens when the data feed
            //rate is too low, and shouldn't normally be needed!
//          printf("Buf uflow, need %d bytes.\n", sizeof(readBuf)-rem);
            bufUnderrunCt++;
            //We both silence the output as well as wait a while by pushing silent samples into the i2s system.
            //This waits for about 200mS
            //for (n=0; n<441*2; n++) {
                // i2s_push_sample(I2S_NUM_0, (const char *) &zero_sample, portMAX_DELAY);
            // }
            i2s_zero_dma_buffer(I2S_NUM_0);
            if(player->state == STOPPED)
                return MAD_FLOW_STOP;
        } else {
            //Read some bytes from the FIFO to re-fill the buffer.
            spiRamFifoRead(&readBuf[rem], n);
            rem += n;
        }
    }

    //Okay, let MAD decode the buffer.
    mad_stream_buffer(stream, (unsigned char*) readBuf, sizeof(readBuf));
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

    int r;
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

    bufUnderrunCt = 0;

    printf("MAD: Decoder start.\n");

    //Initialize mp3 parts
    mad_stream_init(stream);
    mad_frame_init(frame);
    mad_synth_init(synth);

    while(player->state != STOPPED) {
        input(stream, player); //calls mad_stream_buffer internally
        while(player->state != STOPPED) {
            r = mad_frame_decode(frame, stream);
            if (r==-1) {
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

