/*
 * audio_player.h
 *
 *  Created on: 12.03.2017
 *      Author: michaelboeckling
 */

#ifndef INCLUDE_AUDIO_PLAYER_H_
#define INCLUDE_AUDIO_PLAYER_H_

#include <sys/types.h>
#include "common_component.h"

int audio_stream_consumer(const char *recv_buf, ssize_t bytes_read, void *user_data);

component_status_t get_player_status();

void audio_player_init(player_t *player_config);
void audio_player_start();
void audio_player_stop();
void audio_player_destroy();


#endif /* INCLUDE_AUDIO_PLAYER_H_ */
