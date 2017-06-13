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

#include "fdk_aac_decoder.h"
#include "libfaad_decoder.h"
#include "mp3_decoder.h"
#include "controls.h"

#define TAG "audio_player"
#define PRIO_MAD configMAX_PRIORITIES - 2

static player_t *player_instance = NULL;
static component_status_t player_status = UNINITIALIZED;

static int start_decoder_task(player_t *player)
{
    TaskFunction_t task_func;
    char * task_name;
    uint16_t stack_depth;

    ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());

    switch (player->media_stream->content_type)
    {
        case AUDIO_MPEG:
            task_func = mp3_decoder_task;
            task_name = "mp3_decoder_task";
            stack_depth = 8448;
            break;

        case AUDIO_MP4:
            task_func = libfaac_decoder_task;
            task_name = "libfaac_decoder_task";
            stack_depth = 55000;
            break;

        case AUDIO_AAC:
        case OCTET_STREAM: // probably .aac
            task_func = fdkaac_decoder_task;
            task_name = "fdkaac_decoder_task";
            stack_depth = 6144;
            break;

        default:
            ESP_LOGE(TAG, "unknown mime type: %d", player->media_stream->content_type);
            return -1;
    }

    if (xTaskCreatePinnedToCore(task_func, task_name, stack_depth, player,
    PRIO_MAD, NULL, 1) != pdPASS) {
        ESP_LOGE(TAG, "ERROR creating decoder task! Out of memory?");
        return -1;
    } else {
        player->decoder_status = RUNNING;
    }

    ESP_LOGI(TAG, "created decoder task: %s", task_name);

    return 0;
}

static int t;

/* Writes bytes into the FIFO queue, starts decoder task if necessary. */
int audio_stream_consumer(const char *recv_buf, ssize_t bytes_read,
        void *user_data)
{
    player_t *player = user_data;

    // don't bother consuming bytes if stopped
    if(player->command == CMD_STOP) {
        player->decoder_command = CMD_STOP;
        player->command = CMD_NONE;
        return -1;
    }

    if (bytes_read > 0) {
        spiRamFifoWrite(recv_buf, bytes_read);
    }

    int bytes_in_buf = spiRamFifoFill();
    uint8_t fill_level = (bytes_in_buf * 100) / spiRamFifoLen();

    // seems 4k is enough to prevent initial buffer underflow
    uint8_t min_fill_lvl = player->buffer_pref == BUF_PREF_FAST ? 20 : 90;
    bool enough_buffer = fill_level > min_fill_lvl;

    bool early_start = (bytes_in_buf > 1028 && player->media_stream->eof);
    if (player->decoder_status != RUNNING && (enough_buffer || early_start)) {

        // buffer is filled, start decoder
        if (start_decoder_task(player) != 0) {
            ESP_LOGE(TAG, "failed to start decoder task");
            return -1;
        }
    }

    t = (t + 1) & 255;
    if (t == 0) {
        ESP_LOGI(TAG, "Buffer fill %u%%, %d bytes", fill_level, bytes_in_buf);
    }

    return 0;
}

void audio_player_init(player_t *player)
{
    player_instance = player;
    player_status = INITIALIZED;
}

void audio_player_destroy()
{
    renderer_destroy();
    player_status = UNINITIALIZED;
}

void audio_player_start()
{
    renderer_start();
    player_status = RUNNING;
}

void audio_player_stop()
{
    renderer_stop();
    player_instance->command = CMD_STOP;
    // player_status = STOPPED;
}

component_status_t get_player_status()
{
    return player_status;
}

