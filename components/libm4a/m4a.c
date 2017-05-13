/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id$
 *
 * Copyright (C) 2005 Dave Chapman, 2011 Andree Buschmann
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/

// #include <codecs.h>
#include <stddef.h>
#include <inttypes.h>
#include "m4a.h"

/* Implementation of the stream.h functions used by libalac */

#define _Swap32(v) do { \
                   v = (((v) & 0x000000FF) << 0x18) | \
                       (((v) & 0x0000FF00) << 0x08) | \
                       (((v) & 0x00FF0000) >> 0x08) | \
                       (((v) & 0xFF000000) >> 0x18); } while(0)

#define _Swap16(v) do { \
                   v = (((v) & 0x00FF) << 0x08) | \
                       (((v) & 0xFF00) >> 0x08); } while (0)

/* A normal read without any byte-swapping */
void stream_read(stream_t *stream, size_t size, void *buf)
{
    buf_read(buf, size, 1, stream->buf);
}

int32_t stream_read_int32(stream_t *stream)
{
    int32_t v;
    stream_read(stream, 4, &v);
#ifdef ROCKBOX_LITTLE_ENDIAN
    _Swap32(v);
#endif
    return v;
}

int32_t stream_tell(stream_t *stream)
{
    return stream->buf->read_pos;
    // return stream->ci->curpos;
}

uint32_t stream_read_uint32(stream_t *stream)
{
    uint32_t v;
    stream_read(stream, 4, &v);
#ifdef ROCKBOX_LITTLE_ENDIAN
    _Swap32(v);
#endif
    return v;
}

uint16_t stream_read_uint16(stream_t *stream)
{
    uint16_t v;
    stream_read(stream, 2, &v);
#ifdef ROCKBOX_LITTLE_ENDIAN
    _Swap16(v);
#endif
    return v;
}

uint8_t stream_read_uint8(stream_t *stream)
{
    uint8_t v;
    stream_read(stream, 1, &v);
    return v;
}

void stream_skip(stream_t *stream, size_t skip)
{
    // stream->ci->advance_buffer(skip);
    buf_seek_rel(stream->buf, skip);
}

void stream_seek(stream_t *stream, size_t offset)
{
    buf_seek_abs(stream->buf, offset);
    // stream->ci->seek_buffer(offset);
}

int stream_eof(stream_t *stream)
{
    return stream->eof;
}
/*
void stream_create(stream_t *stream,struct codec_api* ci)
{
    stream->ci=ci;
    stream->eof=0;
}
*/

void stream_create(stream_t *stream,buffer_t *buf)
{
    stream->buf = buf;
    stream->eof=0;
}

/* Check if there is a dedicated byte position contained for the given frame.
 * Return this byte position in case of success or return -1. This allows to
 * skip empty samples.
 * During standard playback the search result (index i) will always increase.
 * Therefor we save this index and let the caller set this value again as start
 * index when calling m4a_check_sample_offset() for the next frame. This
 * reduces the overall loop count significantly. */
int m4a_check_sample_offset(demux_res_t *demux_res, uint32_t frame, uint32_t *start)
{
    uint32_t i = *start;
    for (i=0; i<demux_res->num_lookup_table; ++i)
    {
        if (demux_res->lookup_table[i].sample > frame ||
            demux_res->lookup_table[i].offset == 0)
            return -1;
        if (demux_res->lookup_table[i].sample == frame)
            break;
    }
    *start = i;
    return demux_res->lookup_table[i].offset;
}

/* Find the exact or preceding frame in lookup_table[]. Return both frame
 * and byte position of this match. */
static void gather_offset(demux_res_t *demux_res, uint32_t *frame, uint32_t *offset)
{
    uint32_t i = 0;
    for (i=0; i<demux_res->num_lookup_table; ++i)
    {
        if (demux_res->lookup_table[i].offset == 0)
            break;
        if (demux_res->lookup_table[i].sample > *frame)
            break;
    }
    i = (i>0) ? i-1 : 0; /* We want the last chunk _before_ *frame. */
    *frame  = demux_res->lookup_table[i].sample;
    *offset = demux_res->lookup_table[i].offset;
}

/* Seek to desired sound sample location. Return 1 on success (and modify
 * sound_samples_done and current_sample), 0 if failed.
 *
 * Find the sample (=frame) that contains the given sound sample, find a best
 * fit for this sample in the lookup_table[], seek to the byte position. */
unsigned int m4a_seek(demux_res_t* demux_res, stream_t* stream,
    uint32_t sound_sample_loc, uint32_t* sound_samples_done,
    int* current_sample)
{
    uint32_t i = 0;
    uint32_t tmp_var, tmp_cnt, tmp_dur;
    uint32_t new_sample = 0;       /* Holds the amount of chunks/frames. */
    uint32_t new_sound_sample = 0; /* Sums up total amount of samples. */
    uint32_t new_pos;              /* Holds the desired chunk/frame index. */

    /* First check we have the appropriate metadata - we should always
     * have it.
     */
    if (!demux_res->num_time_to_samples || !demux_res->num_sample_byte_sizes)
    {
        return 0;
    }

    /* Find the destination block from time_to_sample array */
    time_to_sample_t *tab = demux_res->time_to_sample;
    while (i < demux_res->num_time_to_samples)
    {
        tmp_cnt = tab[i].sample_count;
        tmp_dur = tab[i].sample_duration;
        tmp_var = tmp_cnt * tmp_dur;
        if (sound_sample_loc <= new_sound_sample + tmp_var)
        {
            tmp_var = (sound_sample_loc - new_sound_sample);
            new_sample       += tmp_var / tmp_dur;
            new_sound_sample += tmp_var;
            break;
        }
        new_sample       += tmp_cnt;
        new_sound_sample += tmp_var;
        ++i;
    }

    /* We know the new sample (=frame), now calculate the file position. */
    gather_offset(demux_res, &new_sample, &new_pos);

    /* We know the new file position, so let's try to seek to it */
    // if (stream->ci->seek_buffer(new_pos))
    if (buf_seek_abs(stream->buf, new_pos))
    {
        *sound_samples_done = new_sound_sample;
        *current_sample = new_sample;
        return 1;
    }

    return 0;
}

/* Seek to the sample containing file_loc. Return 1 on success (and modify
 * sound_samples_done and current_sample), 0 if failed.
 *
 * Seeking uses the following arrays:
 *
 * 1) the lookup_table array contains the file offset for the first sample
 *    of each chunk.
 *
 * 2) the time_to_sample array contains the duration (in sound samples)
 *    of each sample of data.
 *
 * Locate the chunk containing location (using lookup_table), find the first
 * sample of that chunk (using lookup_table). Then use time_to_sample to
 * calculate the sound_samples_done value.
 */
unsigned int m4a_seek_raw(demux_res_t* demux_res, stream_t* stream,
    uint32_t file_loc, uint32_t* sound_samples_done,
    int* current_sample)
{
    uint32_t i;
    uint32_t chunk_sample     = 0;
    uint32_t total_samples    = 0;
    uint32_t new_sound_sample = 0;
    uint32_t tmp_dur;
    uint32_t tmp_cnt;
    uint32_t new_pos;

    /* We know the desired byte offset, search for the chunk right before.
     * Return the associated sample to this chunk as chunk_sample. */
    for (i=0; i < demux_res->num_lookup_table; ++i)
    {
        if (demux_res->lookup_table[i].offset > file_loc)
            break;
    }
    i = (i>0) ? i-1 : 0; /* We want the last chunk _before_ file_loc. */
    chunk_sample = demux_res->lookup_table[i].sample;
    new_pos      = demux_res->lookup_table[i].offset;

    /* Get sound sample offset. */
    i = 0;
    time_to_sample_t *tab2 = demux_res->time_to_sample;
    while (i < demux_res->num_time_to_samples)
    {
        tmp_dur = tab2[i].sample_duration;
        tmp_cnt = tab2[i].sample_count;
        total_samples    += tmp_cnt;
        new_sound_sample += tmp_cnt * tmp_dur;
        if (chunk_sample <= total_samples)
        {
            new_sound_sample += (chunk_sample - total_samples) * tmp_dur;
            break;
        }
        ++i;
    }

    /* Go to the new file position. */
    // if (stream->ci->seek_buffer(new_pos))
    if (buf_seek_abs(stream->buf, new_pos))
    {
        *sound_samples_done = new_sound_sample;
        *current_sample = chunk_sample;
        return 1;
    }

    return 0;
}
