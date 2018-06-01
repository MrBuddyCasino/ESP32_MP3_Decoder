/*
 * wifi.c
 *
 *  Created on: 16.05.2017
 *      Author: michaelboeckling
 */

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"

#include "playerconfig.h"
#include "wifi.h"

#define TAG "wifi"

const int CONNECTED_BIT = 0x00000001;

/* event handler for pre-defined wifi events */
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    EventGroupHandle_t wifi_event_group = ctx;
    ESP_LOGI(TAG, "received wifi event %d", event->event_id);

    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:

        switch(esp_wifi_connect()) {
            case ESP_OK:
                ESP_LOGI(TAG, "connected successfully");
                break;

            case ESP_ERR_WIFI_NOT_INIT:
                ESP_LOGE(TAG, "WiFi is not initialized by eps_wifi_init");
                break;

            case ESP_ERR_WIFI_NOT_STARTED:
                ESP_LOGE(TAG, "WiFi is not started by esp_wifi_start");
                break;

            case ESP_ERR_WIFI_CONN:
                ESP_LOGE(TAG, "WiFi internal error, station or soft-AP control block wrong");
                break;

            case ESP_ERR_WIFI_SSID:
                ESP_LOGE(TAG, "SSID of AP which station connects is invalid");
                break;

            default:
                ESP_LOGE(TAG, "Unknown return code");
                break;
        }

        break;

    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
        /* This is a workaround as ESP32 WiFi libs don't currently
           auto-reassociate. */
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
        break;

    default:
        break;
    }

    return ESP_OK;
}

void initialise_wifi(EventGroupHandle_t wifi_event_group)
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, wifi_event_group));

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_AP_NAME,
            .password = WIFI_AP_PASS,
            .bssid_set = 0
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}
