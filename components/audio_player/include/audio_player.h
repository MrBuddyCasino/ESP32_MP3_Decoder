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
#include "audio_renderer.h"

int audio_stream_consumer(const char *recv_buf, ssize_t bytes_read, void *user_data);


typedef enum {
    CMD_NONE, CMD_START, CMD_STOP
} player_command_t;

typedef enum {
    BUF_PREF_FAST, BUF_PREF_SAFE
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

    player_command_t decoder_command;
    component_status_t decoder_status;
    buffer_pref_t buffer_pref;
    media_stream_t *media_stream;
} player_t;

component_status_t get_player_status();

void audio_player_init(player_t *player_config);
void audio_player_start();
void audio_player_stop();
void audio_player_destroy();


#endif /* INCLUDE_AUDIO_PLAYER_H_ */
