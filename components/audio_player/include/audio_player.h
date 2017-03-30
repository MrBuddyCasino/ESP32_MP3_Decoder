/*
 * audio_player.h
 *
 *  Created on: 12.03.2017
 *      Author: michaelboeckling
 */

#ifndef INCLUDE_AUDIO_PLAYER_H_
#define INCLUDE_AUDIO_PLAYER_H_

#include <sys/types.h>
#include "audio_renderer.h"

int audio_stream_consumer(char *recv_buf, ssize_t bytes_read, void *user_data);


typedef enum {
    IDLE, PLAYING, BUFFER_UNDERRUN, FINISHED, STOPPED
} player_state_t;


typedef struct {
    volatile player_state_t state;
    renderer_config_t *renderer_config;
} player_t;


void audio_player_init(player_t *player);
void audio_player_start(player_t *player);
void audio_player_stop(player_t *player);
void audio_player_destroy(player_t *player);

#endif /* INCLUDE_AUDIO_PLAYER_H_ */
