/*
 * mdns.c
 *
 *  Created on: 23.04.2017
 *      Author: michaelboeckling
 */


#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "mdns.h"
// #include "app_main.h"
static const int CONNECTED_BIT = 0x00000001;

#define TAG "mDNS"
#define MDNS_HOSTNAME "esp32-radio"
#define MDNS_INSTANCE "ESP32 Web Radio"

void mdns_task(EventGroupHandle_t wifi_event_group)
{
    mdns_server_t * mdns = NULL;
    while(1) {
        /* Wait for the callback to set the CONNECTED_BIT in the
           event group.
        */
        xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                            false, true, portMAX_DELAY);

        if (!mdns) {
            ESP_LOGI(TAG, "starting mDNS");
            esp_err_t err = mdns_init(TCPIP_ADAPTER_IF_STA, &mdns);
            if (err) {
                ESP_LOGE(TAG, "Failed starting MDNS: %u", err);
                continue;
            }

            ESP_ERROR_CHECK( mdns_set_hostname(mdns, MDNS_HOSTNAME) );
            ESP_ERROR_CHECK( mdns_set_instance(mdns, MDNS_INSTANCE) );

            ESP_ERROR_CHECK( mdns_service_add(mdns, "_http", "_tcp", 80) );
            ESP_ERROR_CHECK( mdns_service_instance_set(mdns, "_http", "_tcp", "ESP32 WebServer") );
        }

        // sleep 10 seconds
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
