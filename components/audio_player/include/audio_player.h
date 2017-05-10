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

int audio_stream_consumer(const char *recv_buf, ssize_t bytes_read, void *user_data);


typedef enum {
    UNINITIALIZED, INITIALIZED, RUNNING, STOPPED
} player_state_t;

typedef enum {
    CMD_NONE, CMD_START, CMD_STOP
} player_command_t;

typedef enum {
    FAST, SAFE
} buffer_pref_t;

typedef enum
{
    MIME_UNKNOWN = 1, OCTET_STREAM, AUDIO_AAC, AUDIO_MP4, AUDIO_MPEG
} content_type_t;

typedef struct {
    content_type_t content_type;
    bool eof;
} media_stream_t;

typedef struct {
    player_command_t command;
    player_state_t status;

    player_command_t decoder_command;
    player_state_t decoder_status;

    renderer_config_t *renderer_config;
    buffer_pref_t buffer_pref;
    media_stream_t *media_stream;
} player_t;


void audio_player_init(player_t *player);
void audio_player_start(player_t *player);
void audio_player_stop(player_t *player);
void audio_player_destroy(player_t *player);

#endif /* INCLUDE_AUDIO_PLAYER_H_ */
