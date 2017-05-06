/*
 * web_radio.h
 *
 *  Created on: 13.03.2017
 *      Author: michaelboeckling
 */

#ifndef INCLUDE_WEB_RADIO_H_
#define INCLUDE_WEB_RADIO_H_

#include "audio_player.h"

typedef struct {

} radio_controls_t;

typedef struct {
    char *url;
    player_t *player_config;
} web_radio_t;

void web_radio_init(web_radio_t *config);
void web_radio_start(web_radio_t *config);


#endif /* INCLUDE_WEB_RADIO_H_ */