/*
 * ui.c
 *
 *  Created on: 01.04.2017
 *      Author: michaelboeckling
 */



#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <soc/rmt_struct.h>
#include <esp_system.h>
#include <esp_log.h>
#include <nvs_flash.h>
#include <driver/gpio.h>
#include <stdio.h>
#include "ws2812.h"
#include "ui.h"

#define TAG "ui"

#define delay_ms(ms) ((ms) / portTICK_RATE_MS)


typedef struct {
    int pin;
    int queue_size;
    QueueHandle_t ui_queue;
} ui_obj_t;

/** UI object instance */
static ui_obj_t *p_ui_obj = NULL;

static rgbVal black;
static rgbVal white;
static rgbVal red;
static rgbVal green;
static rgbVal blue;


void task_ui(void *pvParameters)
{
    ui_event_t curr_anim = UI_NONE;
    rgbVal *next_color = &black;
    // be lazy and use queue for blink speed
    TickType_t delay = portMAX_DELAY;

    while(1) {
        xQueueReceive((p_ui_obj->ui_queue), &curr_anim, delay);

        switch(curr_anim) {
            case UI_NONE:
                delay = portMAX_DELAY;
                ws2812_setColors(1, &black);
            break;

            case UI_CONNECTING:
                delay = delay_ms(250);
                next_color = (next_color == &white) ? &blue : &white;
                ws2812_setColors(1, next_color);
            break;

            case UI_CONNECTED:
                delay = portMAX_DELAY;
                ws2812_setColors(1, &white);
            break;
        }
        // ESP_LOGI(TAG, "task_ui stack: %d\n", uxTaskGetStackHighWaterMark(NULL));
    }

    vTaskDelete(NULL);
}


void ui_queue_event(ui_event_t evt)
{
    if(p_ui_obj != NULL)
        xQueueSend(p_ui_obj->ui_queue, &evt, 0);
}

int ui_init(gpio_num_t pin)
{
    if(p_ui_obj == NULL) {
        p_ui_obj = malloc(sizeof(ui_obj_t));
    } else {
        ESP_LOGE(TAG, "UI already initialized");
        return -1;
    }

    p_ui_obj->queue_size = 1;
    p_ui_obj->ui_queue = xQueueCreate(1, sizeof(ui_event_t));

    black    = makeRGBVal(0, 0, 0);
    white    = makeRGBVal(32, 32, 32);
    red      = makeRGBVal(32, 0, 0);
    green    = makeRGBVal(0, 32, 0);
    blue    = makeRGBVal(0, 0, 32);

    ws2812_init(pin);
    ws2812_setColors(1, &black);

    xTaskCreatePinnedToCore(&task_ui, "task_ui", 2048, NULL, 20, NULL, 0);

    return 0;
}
