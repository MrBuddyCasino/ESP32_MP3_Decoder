/*
 * audio_renderer.h
 *
 *  Created on: 12.03.2017
 *      Author: michaelboeckling
 */

#ifndef INCLUDE_AUDIO_RENDERER_H_
#define INCLUDE_AUDIO_RENDERER_H_

#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"
#include "common_component.h"

typedef enum {
    I2S, I2S_MERUS, DAC_BUILT_IN, PDM
} output_mode_t;


typedef struct
{
    output_mode_t output_mode;
    int sample_rate;
    float sample_rate_modifier;
    i2s_bits_per_sample_t bit_depth;
    i2s_port_t i2s_num;
} renderer_config_t;

typedef enum
{
    PCM_INTERLEAVED, PCM_LEFT_RIGHT
} pcm_buffer_layout_t;

typedef struct
{
    uint32_t sample_rate;
    i2s_bits_per_sample_t bit_depth;
    uint8_t num_channels;
    pcm_buffer_layout_t buffer_format;
} pcm_format_t;


/* generic renderer interface */
void render_samples(char *buf, uint32_t len, pcm_format_t *format);

/* render callback for libmad */
void render_sample_block(short *sample_buff_ch0, short *sample_buff_ch1, int num_samples, unsigned int num_channels);

void renderer_init(renderer_config_t *config);
void renderer_start();
void renderer_stop();
void renderer_destroy();

void renderer_zero_dma_buffer();
renderer_config_t *renderer_get();


#endif /* INCLUDE_AUDIO_RENDERER_H_ */
