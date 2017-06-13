/*
 * playlist.h
 *
 *  Created on: 10.06.2017
 *      Author: michaelboeckling
 */

#ifndef _INCLUDE_PLAYLIST_H_
#define _INCLUDE_PLAYLIST_H_

typedef struct {
    int16_t curr_pos;
    vec_t *entries_vec;
} playlist_t;

typedef struct {
    const char *name;
    const char *url;
} playlist_entry_t;


playlist_t *playlist_create(void);

void playlist_destroy(playlist_t *p);

void playlist_load_pls(playlist_t *playlist);

playlist_entry_t *playlist_next(playlist_t *p);
playlist_entry_t *playlist_prev(playlist_t *p);
playlist_entry_t *playlist_curr_track(playlist_t *p);

#endif /* _INCLUDE_PLAYLIST_H_ */
