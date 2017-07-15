/*
 * web_radio.c
 *
 *  Created on: 13.03.2017
 *      Author: michaelboeckling
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include "vector.h"
#include "web_radio.h"
#include "http.h"
#include "url_parser.h"
#include "controls.h"
#include "playlist.h"

#define TAG "web_radio"

typedef enum
{
    HDR_CONTENT_TYPE = 1
} header_field_t;

static header_field_t curr_header_field = 0;
static content_type_t content_type = 0;
static bool headers_complete = false;

static int on_header_field_cb(http_parser *parser, const char *at, size_t length)
{
    // convert to lower case
    unsigned char *c = (unsigned char *) at;
    for (; *c; ++c)
        *c = tolower(*c);

    curr_header_field = 0;
    if (strstr(at, "content-type")) {
        curr_header_field = HDR_CONTENT_TYPE;
    }

    return 0;
}

static int on_header_value_cb(http_parser *parser, const char *at, size_t length)
{
    if (curr_header_field == HDR_CONTENT_TYPE) {
        if (strstr(at, "application/octet-stream")) content_type = OCTET_STREAM;
        if (strstr(at, "audio/aac")) content_type = AUDIO_AAC;
        if (strstr(at, "audio/mp4")) content_type = AUDIO_MP4;
        if (strstr(at, "audio/x-m4a")) content_type = AUDIO_MP4;
        if (strstr(at, "audio/mpeg")) content_type = AUDIO_MPEG;

        if(content_type == MIME_UNKNOWN) {
            ESP_LOGE(TAG, "unknown content-type: %s", at);
            return -1;
        }
    }

    return 0;
}

static int on_headers_complete_cb(http_parser *parser)
{
    headers_complete = true;
    player_t *player_config = parser->data;

    player_config->media_stream->content_type = content_type;
    player_config->media_stream->eof = false;

    audio_player_start(player_config);

    return 0;
}

static int on_body_cb(http_parser* parser, const char *at, size_t length)
{
    //printf("%.*s", length, at);
    return audio_stream_consumer(at, length, parser->data);
}

static int on_message_complete_cb(http_parser *parser)
{
    player_t *player_config = parser->data;
    player_config->media_stream->eof = true;

    return 0;
}

static void http_get_task(void *pvParameters)
{
    web_radio_t *radio_conf = pvParameters;

    /* configure callbacks */
    http_parser_settings callbacks = { 0 };
    callbacks.on_body = on_body_cb;
    callbacks.on_header_field = on_header_field_cb;
    callbacks.on_header_value = on_header_value_cb;
    callbacks.on_headers_complete = on_headers_complete_cb;
    callbacks.on_message_complete = on_message_complete_cb;

    // blocks until end of stream
    playlist_entry_t *curr_track = playlist_curr_track(radio_conf->playlist);
    int result = http_client_get(curr_track->url, &callbacks,
            radio_conf->player_config);

    if (result != 0) {
        ESP_LOGE(TAG, "http_client_get error");
    } else {
        ESP_LOGI(TAG, "http_client_get completed");
    }
    // ESP_LOGI(TAG, "http_client_get stack: %d\n", uxTaskGetStackHighWaterMark(NULL));

    vTaskDelete(NULL);
}

void web_radio_start(web_radio_t *config)
{
    // start reader task
    xTaskCreatePinnedToCore(&http_get_task, "http_get_task", 2560, config, 20,
    NULL, 0);
}

void web_radio_stop(web_radio_t *config)
{
    ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());

    audio_player_stop(config->player_config);
    // reader task terminates itself
}

void web_radio_gpio_handler_task(void *pvParams)
{
    gpio_handler_param_t *params = pvParams;
    web_radio_t *config = params->user_data;
    xQueueHandle gpio_evt_queue = params->gpio_evt_queue;

    uint32_t io_num;
    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            ESP_LOGI(TAG, "GPIO[%d] intr, val: %d", io_num, gpio_get_level(io_num));

            /*
            switch (get_player_status()) {
                case RUNNING:
                    ESP_LOGI(TAG, "stopping player");
                    web_radio_stop(config);
                    break;

                case STOPPED:
                    ESP_LOGI(TAG, "starting player");
                    web_radio_start(config);
                    break;

                default:
                    ESP_LOGI(TAG, "player state: %d", get_player_status());
            }
            */
            web_radio_stop(config);
            playlist_entry_t *track = playlist_next(config->playlist);
            ESP_LOGW(TAG, "next track: %s", track->name);


            while(config->player_config->decoder_status != STOPPED) {
                vTaskDelay(20 / portTICK_PERIOD_MS);
            }

            web_radio_start(config);
        }
    }
}

void web_radio_init(web_radio_t *config)
{
    controls_init(web_radio_gpio_handler_task, 2048, config);
    audio_player_init(config->player_config);
}

void web_radio_destroy(web_radio_t *config)
{
    controls_destroy(config);
    audio_player_destroy(config->player_config);
}
