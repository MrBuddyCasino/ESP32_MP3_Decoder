/*
 * common_component.h
 *
 *  Created on: 11.05.2017
 *      Author: michaelboeckling
 */

#ifndef _INCLUDE_COMMON_COMPONENT_H_
#define _INCLUDE_COMMON_COMPONENT_H_

typedef enum {
    UNINITIALIZED, INITIALIZED, RUNNING, STOPPED
} component_status_t;

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
#endif /* _INCLUDE_COMMON_COMPONENT_H_ */
