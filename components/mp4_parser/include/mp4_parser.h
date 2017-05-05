/*
 * mp4_parser.h
 *
 *  Created on: 27.04.2017
 *      Author: michaelboeckling
 */

#ifndef _INCLUDE_MP4_PARSER_H_
#define _INCLUDE_MP4_PARSER_H_

typedef struct
{
    unsigned int position;
    unsigned int size;
} _ATOM;

typedef struct
{
    uint32_t size;
    char name[4];
} _ATOMINFO;

_ATOM findMp4Atom(buffer_t *buf, const char *atom, const uint32_t posi,
        const bool loop);

#endif /* _INCLUDE_MP4_PARSER_H_ */
