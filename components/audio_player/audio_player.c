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

#include "esp_system.h"
#include "esp_log.h"

#include "libfaad_decoder.h"
#include "mp3_decoder.h"
#include "controls.h"

#define TAG "audio_player"
#define PRIO_MAD configMAX_PRIORITIES - 2

static int start_decoder_task(player_t *player)
{
    TaskFunction_t task_func;
    char * task_name;
    uint16_t stack_depth;

    ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());

    switch (player->content_type) {
        case AUDIO_MPEG:
            task_func = mp3_decoder_task;
            task_name = "mp3_decoder_task";
            stack_depth = 8000;
            break;

        case AUDIO_AAC:
        case AUDIO_MP4:
            task_func = libfaac_decoder_task;
            task_name = "libfaac_decoder_task";
            stack_depth = 55000;
            break;

        default:
            ESP_LOGE(TAG, "unknown mime type: %d", player->content_type)
            ;
            return -1;
    }

    if (xTaskCreatePinnedToCore(task_func, task_name, stack_depth, player,
    PRIO_MAD, NULL, 1) != pdPASS) {
        ESP_LOGE(TAG, "ERROR creating decoder task! Out of memory?");
        return -1;
    }

    ESP_LOGI(TAG, "created decoder task: %s", task_name);

    return 0;
}

static int t;
static bool decoder_started = false;
/* pushes bytes into the FIFO queue, starts decoder task if necessary */
int audio_stream_consumer(const char *recv_buf, ssize_t bytes_read,
        void *user_data)
{
    player_t *player = user_data;

    // don't bother consuming bytes if stopped
    if (player->state == STOPPED) {
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
    uint8_t min_fill_lvl = player->buffer_pref == FAST ? 20 : 90;
    bool buffer_ok = fill_level > min_fill_lvl;
    if (!decoder_started && player->state == STARTED && buffer_ok) {

        // buffer is filled, start decoder
        if (start_decoder_task(player) != 0) {
            return -1;
        }

        decoder_started = true;
    }

    t = (t + 1) & 255;
    if (t == 0) {
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
