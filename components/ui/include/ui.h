/*
 * ui.h
 *
 *  Created on: 01.04.2017
 *      Author: michaelboeckling
 */

#ifndef _INCLUDE_UI_H_
#define _INCLUDE_UI_H_

typedef enum { UI_NONE, UI_CONNECTING, UI_CONNECTED } ui_event_t;

void ui_queue_event(ui_event_t evt);
int ui_init(gpio_num_t pin);

#endif /* _INCLUDE_UI_H_ */
