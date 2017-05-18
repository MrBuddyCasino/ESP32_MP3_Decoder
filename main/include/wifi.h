/*
 * wifi.h
 *
 *  Created on: 16.05.2017
 *      Author: michaelboeckling
 */

#ifndef _INCLUDE_WIFI_H_
#define _INCLUDE_WIFI_H_

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
extern const int CONNECTED_BIT;

void initialise_wifi(EventGroupHandle_t wifi_event_group);

#endif /* _INCLUDE_WIFI_H_ */
