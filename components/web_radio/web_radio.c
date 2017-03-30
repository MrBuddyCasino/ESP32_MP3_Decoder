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


static xQueueHandle gpio_evt_queue = NULL;
static TaskHandle_t *gpio_task;
#define ESP_INTR_FLAG_DEFAULT 0

/* gpio event handler */
static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

void gpio_handler_task(void *pvParams)
{
    web_radio_t *config = (web_radio_t *) pvParams;
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%d] intr, val: %d\n", io_num, gpio_get_level(io_num));

            switch(config->player_config->state) {
                case PLAYING:
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

static void web_radio_controls_init(web_radio_t *config)
{
    gpio_config_t io_conf;

    //interrupt of rising edge
    io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
    //bit mask of the pins, use GPIO0 here ("Boot" button)
    io_conf.pin_bit_mask = (1 << GPIO_NUM_0);
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //disable pull-down mode
    io_conf.pull_down_en = 0;
    //enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    //start gpio task

    xTaskCreatePinnedToCore(gpio_handler_task, "gpio_handler_task", 2048, config, 10, gpio_task, 0);

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);

    // remove existing handler that may be present
    gpio_isr_handler_remove(GPIO_NUM_0);

    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(GPIO_NUM_0, gpio_isr_handler, (void*) GPIO_NUM_0);
}

static void web_radio_controls_destroy(web_radio_t *config)
{
    gpio_isr_handler_remove(GPIO_NUM_0);
    vTaskDelete(gpio_task);
    vQueueDelete(gpio_evt_queue);
}

void web_radio_init(web_radio_t *config)
{
    web_radio_controls_init(config);
    audio_player_init(config->player_config);
}

void web_radio_destroy(web_radio_t *config)
{
    web_radio_controls_destroy(config);
    audio_player_destroy(config->player_config);
}


