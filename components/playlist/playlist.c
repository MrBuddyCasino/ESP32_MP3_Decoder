/*
 * playlist.c
 *
 *  Created on: 10.06.2017
 *      Author: michaelboeckling
 */


#include <stddef.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <ctype.h>

#include "esp_log.h"
#include "common_utils.h"
#include "vector.h"
#include "playlist.h"
#include "ini.h"

#ifndef min
   #define min(x,y) ((x)<(y)?(x):(y))
#endif

#define TAG "playlist"


/* embedded playlist */
extern uint8_t file_start[] asm("_binary_playlist_pls_start");
extern uint8_t file_end[] asm("_binary_playlist_pls_end");


char *pls_ini_reader(char* str, int num, uint8_t **str_ptr)
{
    uint8_t *stream = *str_ptr;

    size_t remaining = file_end - stream;
    if(remaining == 0)
        return NULL;

    size_t rlen = min(num, remaining);

    uint8_t *match =  memchr(stream, '\n', rlen);
    if(match != NULL) {
        rlen = match - stream;
    }

    strncpy(str, (const char *)stream, rlen);

    if(str[rlen - 1] == '\r') {
        str[rlen - 1] = '\0';
    }
    str[rlen] = '\0';

    *str_ptr += min(rlen + 1, remaining);

    //printf("pls_ini_reader: num %d - remaining %d - rlen %d - min %d - match %p\n", num, remaining, rlen, min(num, remaining), match);
    return str;
}

int pls_ini_handler(void* user, const char* section,
   const char* name, const char* value)
{
    char name_l[16] = {0};
    for (int i = 0; i < 16 && *name; ++name, i++) {
        name_l[i] = tolower((unsigned char)*name);
    }

    long int track_num = 0;
    if(starts_with(name_l, "title")) {
        track_num = strtol(&name_l[5], NULL, 10);
        if(track_num > vec_size(user)) {
            playlist_entry_t entry = {
                    .name = strdup(value),
                    .url = NULL
            };
            vec_add(user, &entry);
        } else {
            playlist_entry_t *entry = vec_get(user, track_num - 1);
            entry->name = strdup(value);
        }
    }

    if(starts_with(name_l, "file")) {
        track_num = strtol(&name_l[4], NULL, 10);
        if(track_num > vec_size(user)) {
            playlist_entry_t entry = {
                    .name = NULL,
                    .url = strdup(value)
            };
            vec_add(user, &entry);
        } else {
            playlist_entry_t *entry = vec_get(user, track_num - 1);
            entry->url = strdup(value);
        }
    }

    // printf("%s - %s - %s\n", section, name, value);
    return 0;
}

playlist_t *playlist_create(void)
{
    playlist_t *playlist = calloc(1, sizeof(playlist_t));
    playlist->entries_vec = vec_init(sizeof(playlist_entry_t), 8);

    return playlist;
}


playlist_entry_t *playlist_next(playlist_t *p)
{
    p->curr_pos++;

    // wrap around when we have reached the end
    if(p->curr_pos == vec_size(p->entries_vec)) {
        p->curr_pos = 0;
    }

    return vec_get(p->entries_vec, p->curr_pos);
}

playlist_entry_t *playlist_prev(playlist_t *p)
{
    p->curr_pos--;

    // wrap around when we have reached the end
    if(p->curr_pos < 0) {
        p->curr_pos = vec_size(p->entries_vec) - 1;
    }

    return vec_get(p->entries_vec, p->curr_pos);
}

playlist_entry_t *playlist_curr_track(playlist_t *p)
{
    return vec_get(p->entries_vec, p->curr_pos);
}

void playlist_load_pls(playlist_t *playlist)
{
    /* arrays aren't pointers, and we need a pointer */
    uint8_t *file_start_ptr = file_start;
    int res = ini_parse_stream(pls_ini_reader, &file_start_ptr, pls_ini_handler, playlist->entries_vec);
    if(res != 0) {
        // ESP_LOGE(TAG, "failed to load playlist err=%d", res);
    }

    ESP_LOGI(TAG, "Tracks:");
    for(int i = 0; i < vec_size(playlist->entries_vec); i++) {
        playlist_entry_t *e = vec_get(playlist->entries_vec, i);
        printf("%s - %s\n", e->name, e->url);
    }
}

void playlist_destroy(playlist_t *p)
{
    free(p->entries_vec);
    free(p);
}
