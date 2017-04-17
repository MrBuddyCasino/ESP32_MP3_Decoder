
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "http.h"
#include "driver/i2s.h"

#include "ui.h"
#include "spiram_fifo.h"
#include "audio_renderer.h"
#include "web_radio.h"
#include "playerconfig.h"
#include "app_main.h"


#define WIFI_LIST_NUM   10


#define TAG "main"


//Priorities of the reader and the decoder thread. bigger number = higher prio
#define PRIO_READER configMAX_PRIORITIES -3
#define PRIO_MQTT configMAX_PRIORITIES - 3
#define PRIO_CONNECT configMAX_PRIORITIES -1



/* event handler for pre-defined wifi events */
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    EventGroupHandle_t wifi_event_group = ctx;

    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
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

static void initialise_wifi(EventGroupHandle_t wifi_event_group)
{
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, wifi_event_group) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}


static void set_wifi_credentials()
{
    wifi_config_t current_config;
    esp_wifi_get_config(WIFI_IF_STA, &current_config);

    // no changes? return and save a bit of startup time
    if(strcmp( (const char *) current_config.sta.ssid, WIFI_AP_NAME) == 0 &&
       strcmp( (const char *) current_config.sta.password, WIFI_AP_PASS) == 0)
    {
        ESP_LOGI(TAG, "keeping wifi config: %s", WIFI_AP_NAME);
        return;
    }

    // wifi config has changed, update
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_AP_NAME,
            .password = WIFI_AP_PASS,
            .bssid_set = 0,
        },
    };

    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    esp_wifi_disconnect();
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    ESP_LOGI(TAG, "connecting\n");
    esp_wifi_connect();
}

static void start_web_radio()
{
    // init web radio
    web_radio_t *radio_config = calloc(1, sizeof(web_radio_t));
    radio_config->url = PLAY_URL;

    // init player config
    radio_config->player_config = calloc(1, sizeof(player_t));
    radio_config->player_config->state = STOPPED;
    radio_config->player_config->buffer_pref = SAFE;

    // init renderer
    radio_config->player_config->renderer_config = calloc(1, sizeof(renderer_config_t));
    renderer_config_t *renderer_config = radio_config->player_config->renderer_config;
    renderer_config->bit_depth = I2S_BITS_PER_SAMPLE_16BIT;
    renderer_config->i2s_num = I2S_NUM_0;
    renderer_config->sample_rate = 44100;
    renderer_config->sample_rate_modifier = 1.0;
    renderer_config->output_mode = AUDIO_OUTPUT_MODE;

    if(renderer_config->output_mode == I2S_MERUS) {
        renderer_config->bit_depth = I2S_BITS_PER_SAMPLE_32BIT;
    }

    if(renderer_config->output_mode == DAC_BUILT_IN) {
        renderer_config->bit_depth = I2S_BITS_PER_SAMPLE_8BIT;
#ifdef CONFIG_DAC_BUG_WORKAROUND
        // DAC is consuming samples too fast by default
        renderer_config->sample_rate_modifier = 0.0625;
#endif
    }

    // start radio
    web_radio_init(radio_config);
    web_radio_start(radio_config);
}

/**
 * entry point
 */
void app_main()
{
    printf("starting app_main()\n");

    /* FreeRTOS event group to signal when we are connected & ready to make a request */
    EventGroupHandle_t wifi_event_group = xEventGroupCreate();

    nvs_flash_init();

    // init UI
    ui_init(GPIO_NUM_32);
    ui_queue_event(UI_CONNECTING);

    initialise_wifi(wifi_event_group);

    // quick hack
    set_wifi_credentials();

    //Initialize the SPI RAM chip communications and see if it actually retains some bytes. If it
    //doesn't, warn user.
    if (!spiRamFifoInit()) {
        printf("\n\nSPI RAM chip fail!\n");
        while(1);
    }
    printf("\n\nHardware initialized. Waiting for network.\n");

    /* Wait for the callback to set the CONNECTED_BIT in the event group. */
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);

    ui_queue_event(UI_CONNECTED);

    start_web_radio();

    ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());

    // ESP_LOGI(TAG, "app_main stack: %d\n", uxTaskGetStackHighWaterMark(NULL));
}
