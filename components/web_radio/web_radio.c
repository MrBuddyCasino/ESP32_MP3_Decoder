/*
 * web_radio.c
 *
 *  Created on: 13.03.2017
 *      Author: michaelboeckling
 */


#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "web_radio.h"
#include "http.h"
#include "url_parser.h"

#define TAG "web_radio"


static void http_get_task(void *pvParameters)
{
    web_radio_t *radio_conf = pvParameters;

    /* parse URL */
    url_t *url = url_create(radio_conf->url);

    // blocks until end of stream
    int result = http_client_get(
            url->host, url->port, url->path,
            audio_stream_consumer,
            radio_conf->player_config);

    if(result != 0) {
        ESP_LOGE(TAG, "http_client_get error");
    } else {
        ESP_LOGI(TAG, "http_client_get completed");
    }
    // ESP_LOGI(TAG, "http_client_get stack: %d\n", uxTaskGetStackHighWaterMark(NULL));

    url_free(url);

    vTaskDelete(NULL);
}


void web_radio_init(web_radio_t *config) {
    audio_player_init(config->player_config);
}

void web_radio_start(web_radio_t *config) {
    audio_player_start(config->player_config);
    // start reader task
    xTaskCreatePinnedToCore(&http_get_task, "http_get_task", 2048, config, 20, NULL, 0);
}
