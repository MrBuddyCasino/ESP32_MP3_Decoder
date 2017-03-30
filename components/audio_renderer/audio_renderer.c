/*
 * audio_renderer.c
 *
 *  Created on: 12.03.2017
 *      Author: michaelboeckling
 */

#include <stdbool.h>

#include "freertos/FreeRTOS.h"

#include "esp_log.h"
#include "driver/i2s.h"

#include "audio_renderer.h"
#define TAG "renderer"


/* TODO: refactor */
static renderer_config_t *curr_config;


// convert 16bit to 8bit and duplicate mono sample to both channels
static short convert_16bit_mono_to_8bit_stereo(short mono)
{
    unsigned short sample = (unsigned short) mono;
    sample = (sample << 8 & 0xff00) | (((unsigned short) mono >> 8) & 0x00ff);
    return sample;
}

// Reformat the 16-bit mono sample to a format we can send to I2S.
static int convert_16bit_mono_to_16bit_stereo(short mono)
{
    //We can send a 32-bit sample to the I2S subsystem and the DAC will neatly split it up in 2
    //16-bit analog values, one for left and one for right.

    //Duplicate 16-bit sample to both the L and R channel
    unsigned int sample = (unsigned short) mono;
    sample = (sample << 16 & 0xffff0000) | ((unsigned short) mono);
    return sample;
}

static short convert_16bit_stereo_to_8bit_stereo(short left, short right)
{
    unsigned short sample = (unsigned short) left;
    sample = (sample << 8 & 0xff00) | (((unsigned short) right >> 8) & 0x00ff);
    return sample;
}

static int convert_16bit_stereo_to_16bit_stereo(short left, short right)
{
    unsigned int sample = (unsigned short) left;
    sample = (sample << 16 & 0xffff0000) | ((unsigned short) right);
    return sample;
}

static void init_i2s(renderer_config_t *config)
{

    i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX,          // Only TX
            .sample_rate = config->sample_rate,
            .bits_per_sample = config->bit_depth,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,   // 2-channels
            .communication_format = I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB,
            .dma_buf_count = 14,                            // number of buffers, 128 max.
            .dma_buf_len = 32 * 2,                          // size of each buffer
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1        // Interrupt level 1
            };

    i2s_pin_config_t pin_config = {
            .bck_io_num = 26,
            .ws_io_num = 25,
            .data_out_num = 22,
            .data_in_num = I2S_PIN_NO_CHANGE    // Not used
            };

    i2s_driver_install(config->i2s_num, &i2s_config, 0, NULL);
    i2s_set_pin(config->i2s_num, &pin_config);
}

/*
 * Output audio without I2S codec via built-in 8-Bit DAC.
 */
static void init_i2s_dac(renderer_config_t *config)
{

    i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN, // Only TX
            .sample_rate = config->sample_rate,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_8BIT,    // Only 8-bit DAC support
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,   // 2-channels
            .communication_format = I2S_COMM_FORMAT_I2S_MSB,
            .dma_buf_count = 14,                            // number of buffers, 128 max.
            .dma_buf_len = 32 * 2,                          // size of each buffer
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1        // Interrupt level 1
            };


    i2s_driver_install(config->i2s_num, &i2s_config, 0, NULL);
    i2s_set_pin(config->i2s_num, NULL);
}

/* render callback for the libmad synth */
void render_sample_block(short *sample_buff_ch0, short *sample_buff_ch1, int num_samples, unsigned int num_channels)
{
    // if mono: just duplicate the left channel
    if(num_channels == 1) {
        sample_buff_ch1 = sample_buff_ch0;
    }

    switch(curr_config->bit_depth) {

        case I2S_BITS_PER_SAMPLE_8BIT:
            for (int i=0; i < num_samples; i++) {
                short samp8 = convert_16bit_stereo_to_8bit_stereo(sample_buff_ch0[i], sample_buff_ch1[i]);
                i2s_push_sample(curr_config->i2s_num,  (char *)&samp8, portMAX_DELAY);
            }
            break;

        case I2S_BITS_PER_SAMPLE_16BIT:
            for (int i=0; i < num_samples; i++) {
                int samp16 = convert_16bit_stereo_to_16bit_stereo(sample_buff_ch0[i], sample_buff_ch1[i]);
                i2s_push_sample(curr_config->i2s_num,  (char *)&samp16, portMAX_DELAY);
            }
            break;

        case I2S_BITS_PER_SAMPLE_24BIT:
            // TODO
            ESP_LOGE(TAG, "24 bit unsupported");
            break;

        case I2S_BITS_PER_SAMPLE_32BIT:
            // TODO
            ESP_LOGE(TAG, "32 bit unsupported");
            break;
    }
}

//This routine is called by the NXP modifications of libmad. It passes us (for the mono synth)
//32 16-bit samples.
void render_sample_block_mono(short *sample_buff, int num_samples)
{
    switch(curr_config->bit_depth) {

        case I2S_BITS_PER_SAMPLE_8BIT:
            for (int i=0; i < num_samples; i++) {
                short samp8 = convert_16bit_mono_to_8bit_stereo(sample_buff[i]);
                i2s_push_sample(curr_config->i2s_num,  (char *)&samp8, portMAX_DELAY);
            }
            break;

        case I2S_BITS_PER_SAMPLE_16BIT:
            for (int i=0; i < num_samples; i++) {
                int samp16 = convert_16bit_mono_to_16bit_stereo(sample_buff[i]);
                i2s_push_sample(curr_config->i2s_num,  (char *)&samp16, portMAX_DELAY);
            }
            break;

        case I2S_BITS_PER_SAMPLE_24BIT:
            // TODO
            ESP_LOGE(TAG, "24 bit unsupported");
            break;

        case I2S_BITS_PER_SAMPLE_32BIT:
            // TODO
            ESP_LOGE(TAG, "32 bit unsupported");
            break;
    }
}

// Called by the NXP modifications of libmad. Sets the needed output sample rate.
static int prevRate;
void set_dac_sample_rate(int rate) {
    if(rate == prevRate) return;
    prevRate = rate;

    // modifier will usually be 1.0
    rate = rate * curr_config->sample_rate_modifier;

    ESP_LOGI(TAG, "setting sample rate to %d\n", rate);
    i2s_set_sample_rates(curr_config->i2s_num, rate);
}



/* init renderer sink */
void audio_renderer_init(renderer_config_t *config)
{
    // update global
    curr_config = config;

    switch (config->output_mode) {
        case I2S:
            init_i2s(config);
            break;

        case DAC_BUILT_IN:
            init_i2s_dac(config);
            break;

        case PDM:
            // TODO
            break;
    }
}


void audio_renderer_start(renderer_config_t *config)
{
    i2s_start(config->i2s_num);
}

void audio_renderer_stop(renderer_config_t *config)
{
    i2s_stop(config->i2s_num);
}

void audio_renderer_destroy(renderer_config_t *config)
{
    i2s_driver_uninstall(config->i2s_num);
    free(config);
}


