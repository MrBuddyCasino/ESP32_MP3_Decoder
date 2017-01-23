// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

#include "../components/mad/mad.h"
#include "../components/mad/stream.h"
#include "../components/mad/frame.h"
#include "../components/mad/synth.h"

#include "driver/i2s.h"
#include "spiram_fifo.h"
#include "playerconfig.h"

#define WIFI_LIST_NUM   10


#define TAG "main"


//Priorities of the reader and the decoder thread. bigger number = higher prio
#define PRIO_READER configMAX_PRIORITIES -3
#define PRIO_MAD configMAX_PRIORITIES - 2
#define PRIO_MQTT configMAX_PRIORITIES - 3
#define PRIO_CONNECT configMAX_PRIORITIES -1


/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
const int CONNECTED_BIT = BIT0;



typedef enum e_player_state {
    PLAYER_STOPPED = 0,
    PLAYER_STOPPING,
    PLAYER_STARTING,
    PLAYER_STARTED
} player_state;

typedef struct _Player {
    player_state state;
    xTaskHandle readerTaskHandle;
    xTaskHandle madTaskHandle;
} Player;


Player *player;

//The mp3 read buffer size. 2106 bytes should be enough for up to 48KHz mp3s according to the sox sources. Used by libmad.
#define READBUFSZ (2106)
static char readBuf[READBUFSZ];
static long bufUnderrunCt;



/* event handler for pre-defined wifi events */
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id)
    {
    case SYSTEM_EVENT_STA_START:
        esp_wifi_connect();
        break;

    case SYSTEM_EVENT_STA_GOT_IP:
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
        //esp_blufi_send_config_state(ESP_BLUFI_CONFIG_OK);
        //esp_disable_bluetooth(); //close bluetooth function
        //esp_deinit_bluetooth();  //free bluetooth resource
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

static void initialise_wifi(void)
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_FLASH) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}




//Reformat the 16-bit mono sample to a format we can send to I2S.
static short mono_to_8bit_stereo(short s) {

    // convert 16bit to 8bit and duplicate mono sample to both channels
    char eightBit = s >> 8;
    short samp = eightBit;
    samp = (samp) & 0xff;
    samp = (samp << 8) | samp;
    return samp;
}

//Reformat the 16-bit mono sample to a format we can send to I2S.
static int mono_to_16bit_stereo(short s) {

    //We can send a 32-bit sample to the I2S subsystem and the DAC will neatly split it up in 2
    //16-bit analog values, one for left and one for right.

    //Duplicate 16-bit sample to both the L and R channel
    int samp=s;
    samp=(samp)&0xffff;
    samp=(samp<<16)|samp;
    return samp;
}



//Calculate the number of samples that we add or delete. Added samples means a slightly lower
//playback rate, deleted samples means we increase playout speed a bit. This returns an
//8.24 fixed-point number
int recalcAddDelSamp(int oldVal) {
    int ret;
    long prevUdr=0;
    static int cnt;
    int i;
    static int minFifoFill=0;

    i=spiRamFifoFill();
    if (i<minFifoFill) minFifoFill=i;

    //Do the rest of the calculations plusminus every 100mS (assuming a sample rate of 44KHz)
    cnt++;
    if (cnt<1500) return oldVal;
    cnt=0;

    if (spiRamFifoLen()<10*1024) {
        //The FIFO is very small. We can't do calculations on how much it's filled on average, so another
        //algorithm is called for.
        int tgt=1600; //we want an average of this amount of bytes as the average minimum buffer fill
        //Calculate underruns this cycle
        int udr=spiRamGetUnderrunCt()-prevUdr;
        //If we have underruns, the minimum buffer fill has been lower than 0.
        if (udr!=0) minFifoFill=-1;
        //If we're below our target decrease playback speed, and vice-versa.
        ret=oldVal+((minFifoFill-tgt)*ADD_DEL_BUFFPERSAMP_NOSPIRAM);
        prevUdr+=udr;
        minFifoFill=9999;
    } else {
        //We have a larger FIFO; we can adjust according to the FIFO fill rate.
        int tgt=spiRamFifoLen()/2;
        ret=(spiRamFifoFill()-tgt)*ADD_DEL_BUFFPERSAMP;
    }
    return ret;
}



//This routine is called by the NXP modifications of libmad. It passes us (for the mono synth)
//32 16-bit samples.
static int render_cnt = 0;
void render_sample_block(short *short_sample_buff, int no_samples) {

    render_cnt = (render_cnt+1) & 1023;
    if (render_cnt == 0) {
        printf("render_sample_block()\n");
        ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());
    }

    //Signed 16.16 fixed point number: the amount of samples we need to add or delete
    //in every 32-sample
    static int sampAddDel=0;
    //Remainder of sampAddDel cumulatives
    static int sampErr=0;

#ifdef ADD_DEL_SAMPLES
    sampAddDel=recalcAddDelSamp(sampAddDel);
#endif


    sampErr += sampAddDel;

    int i;
    for (i=0; i < no_samples; i++) {

#if defined(USE_DAC)
        short samp = mono_to_8bit_stereo(short_sample_buff[i]);
#else
        int samp = mono_to_16bit_stereo(short_sample_buff[i]);
#endif


        //Dependent on the amount of buffer we have too much or too little, we're going to add or remove
        //samples. This basically does error diffusion on the sample added or removed.
        if (sampErr>(1<<24)) {
            sampErr-=(1<<24);
            //...and don't output an i2s sample
        } else if (sampErr<-(1<<24)) {
            sampErr+=(1<<24);
            //..and output 2 samples instead of one.
            i2s_push_sample(I2S_NUM_0,  (char *)&samp, portMAX_DELAY);
            i2s_push_sample(I2S_NUM_0,  (char *)&samp, portMAX_DELAY);
        } else {
            //Just output the sample.
            i2s_push_sample(I2S_NUM_0,  (char *)&samp, portMAX_DELAY);
        }
    }
}


#define I2S_NUM         (0)
#define SAMPLE_RATE     (44100)

static void init_i2s()
{

    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,                                  // Only TX
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = 16,                                                  // 16-bit per channel
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           // 2-channels
        .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 14,                                                    // number of buffers, 128 max.
        .dma_buf_len = 32*2,                                                    // size of each buffer
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1                                // Interrupt level 1
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = 26,
        .ws_io_num = 25,
        .data_out_num = 22,
        .data_in_num = I2S_PIN_NO_CHANGE                                          // Not used
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);
}

/*
 * Output audio without I2S codec via built-in 8-Bit DAC.
 */
static void init_i2s_dac()
{

    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN,          // Only TX
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = 8,                                                   // Only 8-bit DAC support
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,                           // 2-channels
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .dma_buf_count = 14,                                                    // number of buffers, 128 max.
        .dma_buf_len = 32*2,                                                    // size of each buffer
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1                                // Interrupt level 1
    };

    i2s_pin_config_t pin_config = {
        .bck_io_num = 26,
        .ws_io_num = 25,
        .data_out_num = 22,
        .data_in_num = I2S_PIN_NO_CHANGE                                          // Not used
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM, &pin_config);
}


//Called by the NXP modificationss of libmad. Sets the needed output sample rate.
static int oldRate=0;
void set_dac_sample_rate(int rate) {

    if (rate==oldRate) return;
    oldRate=rate;
    printf("Rate %d\n", rate);

    i2s_set_sample_rates(I2S_NUM_0, rate);
}

const uint32_t zero_sample[2] = { 0 };
static enum  mad_flow input(struct mad_stream *stream) {
    int n, i;
    int rem;
    //Shift remaining contents of buf to the front
    rem = stream->bufend - stream->next_frame;
    memmove(readBuf, stream->next_frame, rem);

    while (rem < sizeof(readBuf)) {
        n = (sizeof(readBuf) - rem);    //Calculate amount of bytes we need to fill buffer.
        i = spiRamFifoFill();
        if (i < n) n=i;                 //If the fifo can give us less, only take that amount
        if (n==0) {                 //Can't take anything?
            //Wait until there is enough data in the buffer. This only happens when the data feed
            //rate is too low, and shouldn't normally be needed!
//          printf("Buf uflow, need %d bytes.\n", sizeof(readBuf)-rem);
            bufUnderrunCt++;
            //We both silence the output as well as wait a while by pushing silent samples into the i2s system.
            //This waits for about 200mS
            // for (n=0; n<441*2; n++) i2sPushSample(0);
            for (n=0; n<441*2; n++) {
                i2s_push_sample(I2S_NUM_0, (const char *) &zero_sample, portMAX_DELAY);
            }
        } else {
            //Read some bytes from the FIFO to re-fill the buffer.
            spiRamFifoRead(&readBuf[rem], n);
            rem += n;
        }
    }

    //Okay, let MAD decode the buffer.
    mad_stream_buffer(stream, (unsigned char*) readBuf, sizeof(readBuf));
    return MAD_FLOW_CONTINUE;
}

//Routine to print out an error
static enum mad_flow error(void *data, struct mad_stream *stream, struct mad_frame *frame) {
    printf("dec err 0x%04x (%s)\n", stream->error, mad_stream_errorstr(stream));
    return MAD_FLOW_CONTINUE;
}




//This is the main mp3 decoding task. It will grab data from the input buffer FIFO in the SPI ram and
//output it to the I2S port.
void tskmad(void *pvParameters)
{
    if(player->madTaskHandle != xTaskGetCurrentTaskHandle()) {
        printf("MAD task already running!\n");
        vTaskDelete(NULL);
        return;
    }

    int r;
    struct mad_stream *stream;
    struct mad_frame *frame;
    struct mad_synth *synth;

    //Allocate structs needed for mp3 decoding
    stream = malloc(sizeof(struct mad_stream));
    frame = malloc(sizeof(struct mad_frame));
    synth = malloc(sizeof(struct mad_synth));

    if (stream==NULL) { printf("MAD: malloc(stream) failed\n"); return; }
    if (synth==NULL) { printf("MAD: malloc(synth) failed\n"); return; }
    if (frame==NULL) { printf("MAD: malloc(frame) failed\n"); return; }

    // initialize I2S
#ifdef USE_DAC
    init_i2s_dac();
#else
    init_i2s();
#endif


    bufUnderrunCt = 0;

    printf("MAD: Decoder start.\n");

    //Initialize mp3 parts
    mad_stream_init(stream);
    mad_frame_init(frame);
    mad_synth_init(synth);

    while(player->state != PLAYER_STOPPING) {
        input(stream); //calls mad_stream_buffer internally
        while(player->state != PLAYER_STOPPING) {
            r = mad_frame_decode(frame, stream);
            if (r==-1) {
                if (!MAD_RECOVERABLE(stream->error)) {
                    //We're most likely out of buffer and need to call input() again
                    break;
                }
                error(NULL, stream, frame);
                continue;
            }
            mad_synth_frame(synth, frame);
            // ESP_LOGI(TAG, "MAD decoder stack: %d\n", uxTaskGetStackHighWaterMark(NULL));
            // ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());
        }
    }


    // i2sEnd();
    i2s_stop(I2S_NUM_0);

    free(synth);
    free(frame);
    free(stream);

    // clear semaphore for reader task
    spiRamFifoReset();

    printf("MAD: Decoder stopped.\n");

    player->madTaskHandle = NULL;
    vTaskDelete(NULL);
}


static int t;
esp_err_t stream_reader(char *recv_buf, ssize_t bytes_read)
{
    if (bytes_read > 0)
    {
        spiRamFifoWrite(recv_buf, bytes_read);
    }


    if ((player->madTaskHandle == NULL) && (spiRamFifoFree() < spiRamFifoLen()/2) && player->state == PLAYER_STARTING)
    {
        //Buffer is filled. Start up the MAD task. Yes, the 2100 words of stack is a fairly large amount but MAD seems to need it.
        // if (xTaskCreate(tskmad, "tskmad", 2100, NULL, PRIO_MAD, &(player->madTaskHandle)) != pdPASS)
        if (xTaskCreatePinnedToCore(&tskmad, "tskmad", 6300, NULL, PRIO_MAD, &(player->madTaskHandle), 1) != pdPASS)
        {
            printf("ERROR creating MAD task! Out of memory?\n");
        } else {
            printf("created MAD task\n");
            // ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());
        }
    }


    t = (t+1) & 255;
    if (t == 0) {
        // printf("Buffer fill %d, DMA underrun ct %d, buff underrun ct %d\n", spiRamFifoFill(), (int)i2sGetUnderrunCnt(), (int)bufUnderrunCt);
        printf("Buffer fill %d, buff underrun ct %d\n", spiRamFifoFill(), (int)bufUnderrunCt);
    }

    return ESP_OK;
}



static void http_get_task(void *pvParameters)
{
    /* Wait for the callback to set the CONNECTED_BIT in the event group. */
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT,
                        false, true, portMAX_DELAY);

    char *host = PLAY_SERVER;
    char *port = PLAY_PORT;
    char *path = PLAY_PATH;
    esp_err_t result = http_client_get(host, port, path, stream_reader);
    if(result != ESP_OK) {
        ESP_LOGE(TAG, "http_client_get error");
    }
    ESP_LOGI(TAG, "http_client_get completed");

    // ESP_LOGI(TAG, "http_client_get stack: %d\n", uxTaskGetStackHighWaterMark(NULL));

    vTaskDelete(NULL);
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

void app_main()
{
    printf("starting app_main()\n");

    nvs_flash_init();
    initialise_wifi();

    // quick hack
    set_wifi_credentials();

    //Initialize the SPI RAM chip communications and see if it actually retains some bytes. If it
    //doesn't, warn user.
    if (!spiRamFifoInit()) {
        printf("\n\nSPI RAM chip fail!\n");
        while(1);
    }
    printf("\n\nHardware initialized. Waiting for network.\n");

    xTaskCreatePinnedToCore(&http_get_task, "httpGetTask", 2048, NULL, 20, NULL, 0);

    // init player
    player = calloc(1, sizeof(Player));
    player->state = PLAYER_STARTING;

    ESP_LOGI(TAG, "RAM left %d", esp_get_free_heap_size());

    // ESP_LOGI(TAG, "app_main stack: %d\n", uxTaskGetStackHighWaterMark(NULL));
}
