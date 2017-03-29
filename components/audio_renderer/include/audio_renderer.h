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
    I2S, DAC_BUILT_IN, PDM
} output_mode_t;

typedef struct
{
    output_mode_t output_mode;
    int sample_rate;
    i2s_bits_per_sample_t bit_depth;
    i2s_port_t i2s_num;
} renderer_config_t;


/* callback for the audio decoder */
void render_sample_block(short *short_sample_buff, int no_samples);

void audio_renderer_init(renderer_config_t *config);
void audio_renderer_start(renderer_config_t *config);
void audio_renderer_stop(renderer_config_t *config);
void audio_renderer_destroy(renderer_config_t *config);


#endif /* INCLUDE_AUDIO_RENDERER_H_ */
