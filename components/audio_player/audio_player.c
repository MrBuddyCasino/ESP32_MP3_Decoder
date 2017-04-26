/*
 * audio_player.c
 *
 *  Created on: 12.03.2017
 *      Author: michaelboeckling
 */


#include <stdlib.h>
#include "freertos/FreeRTOS.h"

#include "audio_player.h"
#include "spiram_fifo.h"
#include "freertos/task.h"
#include "mp3_decoder.h"
#include "aac_decoder.h"
#include "controls.h"

#define PRIO_MAD configMAX_PRIORITIES - 2


static int t;
static bool decoder_started = false;
/* pushes bytes into the FIFO queue, starts decoder task if necessary */
int audio_stream_consumer(const char *recv_buf, ssize_t bytes_read, void *user_data)
{
    player_t *player = user_data;

    // don't bother consuming bytes if stopped
    if(player->state == STOPPED) {
        // TODO: add proper synchronization, this is just an assumption
        decoder_started = false;
        return -1;
    }

    if (bytes_read > 0) {
        spiRamFifoWrite(recv_buf, bytes_read);
    }

    int bytes_in_buf = spiRamFifoFill();
    uint8_t fill_level = (bytes_in_buf * 100) / spiRamFifoLen();

    // seems 4k is enough to prevent initial buffer underflow
    bool buffer_ok = (player->buffer_pref == FAST) ? (fill_level > 20) : (fill_level > 90);
    if (!decoder_started && player->state == STARTED && buffer_ok)
    {
        decoder_started = true;
        //Buffer is filled. Start up the MAD task.
        // TODO: 6300 not enough?
        // if (xTaskCreatePinnedToCore(&mp3_decoder_task, "mp3_decoder_task", 8000, player, PRIO_MAD, NULL, 1) != pdPASS)
        if (xTaskCreatePinnedToCore(&aac_decoder_task, "aac_decoder_task", 8000, player, PRIO_MAD, NULL, 1) != pdPASS)
        {
            printf("ERROR creating decoder task! Out of memory?\n");
        } else {
            printf("created decoder task\n");
        }
    }


    t = (t+1) & 255;
    if (t == 0) {
        // printf("Buffer fill %d, buff underrun ct %d\n", spiRamFifoFill(), (int)bufUnderrunCt);
        printf("Buffer fill %u%%, %d bytes\n", fill_level, bytes_in_buf);
    }

    return 0;
}

void audio_player_init(player_t *player)
{
    // initialize I2S
    audio_renderer_init(player->renderer_config);
}

void audio_player_destroy(player_t *player)
{
    // halt I2S
    audio_renderer_destroy(player->renderer_config);
}

void audio_player_start(player_t *player)
{
    audio_renderer_start(player->renderer_config);
    player->state = STARTED;
}

void audio_player_stop(player_t *player)
{
    audio_renderer_stop(player->renderer_config);
    player->state = STOPPED;
}
