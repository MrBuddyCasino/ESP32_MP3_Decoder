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


/* render callback for libmad */
void render_sample_block(short *sample_buff_ch0, short *sample_buff_ch1, int num_samples, unsigned int num_channels);

void audio_renderer_init(renderer_config_t *config);
void audio_renderer_start(renderer_config_t *config);
void audio_renderer_stop(renderer_config_t *config);
void audio_renderer_destroy(renderer_config_t *config);


#endif /* INCLUDE_AUDIO_RENDERER_H_ */
