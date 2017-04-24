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
#include "esp_system.h"
#include "driver/gpio.h"

#include "web_radio.h"
#include "http.h"
#include "url_parser.h"
#include "controls.h"

#define TAG "web_radio"

static TaskHandle_t *reader_task;

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


void web_radio_start(web_radio_t *config)
{
    audio_player_start(config->player_config);
    // start reader task
    xTaskCreatePinnedToCore(&http_get_task, "http_get_task", 2048, config, 20, reader_task, 0);
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
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));

            switch(config->player_config->state) {
                case STARTED:
                    printf("stopping player\n");
                    web_radio_stop(config);
                    break;

                case STOPPED:
                    printf("starting player\n");
                    web_radio_start(config);
                    break;

                default:
                    printf("player state: %d\n", config->player_config->state);
            }
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


