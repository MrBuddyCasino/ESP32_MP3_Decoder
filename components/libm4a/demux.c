/*
 * ALAC (Apple Lossless Audio Codec) decoder
 * Copyright (c) 2005 David Hammerton
 * All rights reserved.
 *
 * This is the quicktime container demuxer.
 *
 * http://crazney.net/programs/itunes/alac.html
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

// #include "codeclib.h"

#include "m4a.h"

#undef DEBUGF
#if defined(DEBUG)
#define DEBUGF qtmovie->stream->ci->debugf
#else
#define DEBUGF(...)
#endif
#define DEBUGF printf
typedef struct
{
    stream_t *stream;
    demux_res_t *res;
} qtmovie_t;


/* chunk handlers */
static void read_chunk_ftyp(qtmovie_t *qtmovie, size_t chunk_len)
{
    fourcc_t type;
    size_t size_remaining = chunk_len - 8;

    type = stream_read_uint32(qtmovie->stream);
    size_remaining-=4;
    if ((type != MAKEFOURCC('M','4','A',' ')) &&
        (type != MAKEFOURCC('m','4','a',' ')) &&
        (type != MAKEFOURCC('M','4','B',' ')) &&
        (type != MAKEFOURCC('m','p','4','2')) &&
        (type != MAKEFOURCC('3','g','p','6')) &&
        (type != MAKEFOURCC('q','t',' ',' ')) &&
        (type != MAKEFOURCC('i','s','o','m')))
    {
        DEBUGF("not M4A file\n");
        return;
    }
    /* minor_ver = */ stream_read_uint32(qtmovie->stream);
    size_remaining-=4;

    /* compatible brands */
    while (size_remaining)
    {
        /* unused */
        /*fourcc_t cbrand =*/ stream_read_uint32(qtmovie->stream);
        size_remaining-=4;
    }
}

static uint32_t mp4ff_read_mp4_descr_length(stream_t* stream)
{
    uint8_t b;
    uint8_t numBytes = 0;
    uint32_t length = 0;

    do
    {
        b = stream_read_uint8(stream);
        numBytes++;
        length = (length << 7) | (b & 0x7F);
    } while ((b & 0x80) && numBytes < 4);

    return length;
}

/* The following function is based on mp4ff */
static bool read_chunk_esds(qtmovie_t *qtmovie, size_t chunk_len)
{
    uint8_t tag;
    uint32_t temp;

    (void)chunk_len;
    /* version and flags */
    temp=stream_read_uint32(qtmovie->stream);

    /* get and verify ES_DescrTag */
    tag = stream_read_uint8(qtmovie->stream);
    if (tag == 0x03)
    {
        /* read length */
        if (mp4ff_read_mp4_descr_length(qtmovie->stream) < 5 + 15)
        {
            return false;
        }
        /* skip 3 bytes */
        stream_skip(qtmovie->stream,3);
    } else {
        /* skip 2 bytes */
        stream_skip(qtmovie->stream,2);
    }

    /* get and verify DecoderConfigDescrTab */
    if (stream_read_uint8(qtmovie->stream) != 0x04)
    {
        return false;
    }

    /* read length */
    temp = mp4ff_read_mp4_descr_length(qtmovie->stream);
    if (temp < 13) return false;

    /* audioType = */  stream_read_uint8(qtmovie->stream);
    /* temp = */       stream_read_int32(qtmovie->stream);//0x15000414 ????
    /* maxBitrate = */ stream_read_int32(qtmovie->stream);
    /* avgBitrate = */ stream_read_int32(qtmovie->stream);

    /* get and verify DecSpecificInfoTag */
    if (stream_read_uint8(qtmovie->stream) != 0x05)
    {
        return false;
    }

    /* read length */
    qtmovie->res->codecdata_len = mp4ff_read_mp4_descr_length(qtmovie->stream);
    if (qtmovie->res->codecdata_len > MAX_CODECDATA_SIZE)
    {
        DEBUGF("codecdata too large (%d) in esds\n",
               (int)qtmovie->res->codecdata_len);
        return false;
    }

    stream_read(qtmovie->stream, qtmovie->res->codecdata_len, qtmovie->res->codecdata);

    /* will skip the remainder of the atom */
    return true;
}

static bool read_chunk_stsd(qtmovie_t *qtmovie, size_t chunk_len)
{
    unsigned int i;
    int j;
    uint32_t numentries;
    size_t size_remaining = chunk_len - 8;
    bool got_codec_data = false;

    /* version */
    stream_read_uint8(qtmovie->stream);
    size_remaining -= 1;
    /* flags */
    stream_read_uint8(qtmovie->stream);
    stream_read_uint8(qtmovie->stream);
    stream_read_uint8(qtmovie->stream);
    size_remaining -= 3;

    numentries = stream_read_uint32(qtmovie->stream);
    size_remaining -= 4;

    /* if (numentries != 1)
    {
        DEBUGF("only expecting one entry in sample description atom!\n");
        return false;
    } */

    for (i = 0; i < numentries; i++)
    {
        uint32_t entry_size;

        uint32_t entry_remaining;

        entry_size = stream_read_uint32(qtmovie->stream);
        qtmovie->res->format = stream_read_uint32(qtmovie->stream);
        DEBUGF("format: %c%c%c%c\n",SPLITFOURCC(qtmovie->res->format));
        entry_remaining = entry_size;
        entry_remaining -= 8;

        /* sound info: */

        /* reserved + data reference index + sound version + reserved */
        stream_skip(qtmovie->stream, 6 + 2 + 2 + 6);
        entry_remaining -= 6 + 2 + 2 + 6;

        qtmovie->res->num_channels = stream_read_uint16(qtmovie->stream);
        qtmovie->res->sound_sample_size = stream_read_uint16(qtmovie->stream);
        entry_remaining -= 4;

        /* packet size */
        stream_skip(qtmovie->stream, 2);
        qtmovie->res->sound_sample_rate = stream_read_uint32(qtmovie->stream);
        /* reserved size */
        stream_skip(qtmovie->stream, 2);
        entry_remaining -= 8;

        /* remaining is codec data */

        if (qtmovie->res->format==MAKEFOURCC('a','l','a','c')) {
          /*
          if (qtmovie->stream->ci->id3->codectype!=AFMT_MP4_ALAC) {
               return false;
          }
          */

          /* 12 = audio format atom, 8 = padding */
          qtmovie->res->codecdata_len = entry_remaining + 12 + 8;
          if (qtmovie->res->codecdata_len > MAX_CODECDATA_SIZE)
          {
              DEBUGF("codecdata too large (%d) in stsd\n",
                      (int)qtmovie->res->codecdata_len);
              return false;
          }

          memset(qtmovie->res->codecdata, 0, qtmovie->res->codecdata_len);
          /* audio format atom */
#if 0
          /* The ALAC decoder skips these bytes, so there is no need to store them,
             and this code isn't endian/alignment safe */
          ((unsigned int*)qtmovie->res->codecdata)[0] = 0x0c000000;
          ((unsigned int*)qtmovie->res->codecdata)[1] = MAKEFOURCC('a','m','r','f');
          ((unsigned int*)qtmovie->res->codecdata)[2] = MAKEFOURCC('c','a','l','a');
#endif

          stream_read(qtmovie->stream,
                  entry_remaining,
                  ((char*)qtmovie->res->codecdata) + 12);
          entry_remaining -= entry_remaining;
          got_codec_data = true;

          if (entry_remaining)
              stream_skip(qtmovie->stream, entry_remaining);

        } else if (qtmovie->res->format==MAKEFOURCC('m','p','4','a')) {
          /*
          if (qtmovie->stream->ci->id3->codectype!=AFMT_MP4_AAC &&
              qtmovie->stream->ci->id3->codectype!=AFMT_MP4_AAC_HE) {
               return false;
          }
          */

          size_t sub_chunk_len;
          fourcc_t sub_chunk_id;

          sub_chunk_len = stream_read_uint32(qtmovie->stream);
          if (sub_chunk_len <= 1 || sub_chunk_len > entry_remaining)
          {
              DEBUGF("strange size (%lu) for chunk inside mp4a\n",
                     (unsigned long)sub_chunk_len);
              return false;
          }

          sub_chunk_id = stream_read_uint32(qtmovie->stream);

          if (sub_chunk_id != MAKEFOURCC('e','s','d','s'))
          {
              DEBUGF("Expected esds chunk inside mp4a, found %c%c%c%c\n",SPLITFOURCC(sub_chunk_id));
              return false;
          }

          j = qtmovie->stream->buf->read_pos + sub_chunk_len - 8;
          if (read_chunk_esds(qtmovie,sub_chunk_len)) {
             if (j != qtmovie->stream->buf->read_pos) {
               DEBUGF("curpos=%d, sub_chunk_len=%d, j=%d - Skipping %d bytes\n", (int)qtmovie->stream->buf->read_pos, sub_chunk_len, j, j - (int) qtmovie->stream->buf->read_pos);
               stream_skip(qtmovie->stream, j - (int) qtmovie->stream->buf->read_pos);
               // TODO hotfix
               // stream_skip(qtmovie->stream, 4);
             }
             got_codec_data = true;
             entry_remaining-=sub_chunk_len;
          } else {
              DEBUGF("Error reading esds\n");
              return false;
          }

          DEBUGF("entry_remaining=%ld\n",(long)entry_remaining);
          stream_skip(qtmovie->stream,entry_remaining);

        } else if (qtmovie->res->format==MAKEFOURCC('f','r','e','e')) {
            /* Skip "filler" atom */
            stream_skip(qtmovie->stream,entry_remaining);
        } else {
            DEBUGF("expecting 'alac', 'mp4a' or 'free' data format, got %c%c%c%c\n",
                 SPLITFOURCC(qtmovie->res->format));
            return false;
        }
    }
    return got_codec_data;
}

static bool read_chunk_stts(qtmovie_t *qtmovie, size_t chunk_len)
{
    unsigned int i;
    uint32_t numentries;
    size_t size_remaining = chunk_len - 8;

    /* version */
    stream_read_uint8(qtmovie->stream);
    size_remaining -= 1;
    /* flags */
    stream_read_uint8(qtmovie->stream);
    stream_read_uint8(qtmovie->stream);
    stream_read_uint8(qtmovie->stream);
    size_remaining -= 3;

    numentries = stream_read_uint32(qtmovie->stream);
    size_remaining -= 4;

    qtmovie->res->num_time_to_samples = numentries;
    qtmovie->res->time_to_sample = malloc(numentries * sizeof(*qtmovie->res->time_to_sample));

    if (!qtmovie->res->time_to_sample)
    {
        DEBUGF("stts too large\n");
        return false;
    }

    for (i = 0; i < numentries; i++)
    {
        qtmovie->res->time_to_sample[i].sample_count = stream_read_uint32(qtmovie->stream);
        qtmovie->res->time_to_sample[i].sample_duration = stream_read_uint32(qtmovie->stream);
        size_remaining -= 8;
    }

    if (size_remaining)
    {
        DEBUGF("ehm, size remianing?\n");
        stream_skip(qtmovie->stream, size_remaining);
    }

    return true;
}

static bool read_chunk_stsz(qtmovie_t *qtmovie, size_t chunk_len)
{
    size_t size_remaining = chunk_len - 8;

    /* version */
    stream_read_uint8(qtmovie->stream);
    size_remaining -= 1;
    /* flags */
    stream_read_uint8(qtmovie->stream);
    stream_read_uint8(qtmovie->stream);
    stream_read_uint8(qtmovie->stream);
    size_remaining -= 3;

    /* default sample size */
    if (stream_read_uint32(qtmovie->stream) != 0)
    {
        DEBUGF("i was expecting variable samples sizes\n");
        stream_read_uint32(qtmovie->stream);
        size_remaining -= 4;
        return true;
    }
    size_remaining -= 4;

    qtmovie->res->num_sample_byte_sizes = stream_read_uint32(qtmovie->stream);
    size_remaining -= 4;

    if (size_remaining)
    {
        stream_skip(qtmovie->stream, size_remaining);
    }

    return true;
}

static bool read_chunk_stsc(qtmovie_t *qtmovie, size_t chunk_len)
{
    unsigned int i;
    uint32_t numentries;
    size_t size_remaining = chunk_len - 8;

    /* version + flags */
    stream_read_uint32(qtmovie->stream);
    size_remaining -= 4;

    numentries = stream_read_uint32(qtmovie->stream);
    size_remaining -= 4;

    qtmovie->res->num_sample_to_chunks = numentries;
    qtmovie->res->sample_to_chunk = malloc(numentries * sizeof(sample_to_chunk_t));

    if (!qtmovie->res->sample_to_chunk)
    {
        DEBUGF("stsc too large\n");
        return false;
    }

    for (i = 0; i < numentries; i++)
    {
        qtmovie->res->sample_to_chunk[i].first_chunk =
            stream_read_uint32(qtmovie->stream);
        qtmovie->res->sample_to_chunk[i].num_samples =
            stream_read_uint32(qtmovie->stream);
        stream_read_uint32(qtmovie->stream);
        size_remaining -= 12;
    }

    if (size_remaining)
    {
        DEBUGF("ehm, size remianing?\n");
        stream_skip(qtmovie->stream, size_remaining);
    }

    return true;
}

static bool read_chunk_stco(qtmovie_t *qtmovie, size_t chunk_len)
{
    uint32_t i, k, old_i;
    uint32_t numentries;
    uint32_t idx = 0;
    uint32_t frame;
    uint32_t offset;
    uint32_t old_first;
    uint32_t new_first;
    uint32_t old_frame;
    size_t size_remaining = chunk_len - 8;

    /* version + flags */
    stream_read_uint32(qtmovie->stream);
    size_remaining -= 4;

    numentries = stream_read_uint32(qtmovie->stream);
    size_remaining -= 4;

    qtmovie->res->num_lookup_table = numentries;
    qtmovie->res->lookup_table = malloc(numentries * sizeof(*qtmovie->res->lookup_table));

    if (!qtmovie->res->lookup_table)
    {
        DEBUGF("stco too large to allocate lookup_table[]\n");
        return false;
    }

    /* read first offset */
    offset = stream_read_uint32(qtmovie->stream);
    size_remaining -= 4;

    /* Build up lookup table. The lookup table contains the sample index and
     * byte position in the file for each chunk. This table is used to seek
     * and resume (see m4a_seek() and m4a_seek_raw() in libm4a/m4a.c) and
     * to skip empty chunks (see m4a_check_sample_offset() in codecs/aac.c and
     * libm4a/m4a.c).
     * The seek/resume precision is lower than using sample_byte_size[] and
     * depends on numentries. Typically the resolution is ~1/10 of all frames
     * which equals about 1/4-1/2 seconds. The loss of seek precision is
     * accepted to be able to avoid allocation of the large sample_byte_size[]
     * table. This reduces the memory consumption by a factor of 2 or even
     * more. */
    i = 1;
    old_i = 1;
    frame = 0;
    old_first = qtmovie->res->sample_to_chunk[0].first_chunk;
    old_frame = qtmovie->res->sample_to_chunk[0].num_samples;
    new_first = qtmovie->res->sample_to_chunk[1].first_chunk;
    for (k = 1; k < numentries; ++k)
    {
        for (; i < qtmovie->res->num_sample_to_chunks; ++i)
        {
            if (i > old_i)
            {
                /* Only access sample_to_chunk[] if new data is required. */
                old_first = qtmovie->res->sample_to_chunk[i-1].first_chunk;
                old_frame = qtmovie->res->sample_to_chunk[i-1].num_samples;
                new_first = qtmovie->res->sample_to_chunk[i  ].first_chunk;
                old_i = i;
            }

            if (new_first > k)
                break;

            frame += (new_first - old_first) * old_frame;
        }
        frame += (k - old_first) * old_frame;

        qtmovie->res->lookup_table[idx].sample = frame;
        qtmovie->res->lookup_table[idx].offset = offset;
        idx++;

        frame -= (k - old_first) * old_frame;

        offset = stream_read_uint32(qtmovie->stream);
        size_remaining -= 4;
    }
    /* zero-terminate the lookup table */
    qtmovie->res->lookup_table[idx].sample = 0;
    qtmovie->res->lookup_table[idx].offset = 0;

    if (size_remaining)
    {
        DEBUGF("ehm, size remianing?\n");
        stream_skip(qtmovie->stream, size_remaining);
    }

    return true;
}

static bool read_chunk_stbl(qtmovie_t *qtmovie, size_t chunk_len)
{
    size_t size_remaining = chunk_len - 8;

    while (size_remaining)
    {
        size_t sub_chunk_len;
        fourcc_t sub_chunk_id;

        sub_chunk_len = stream_read_uint32(qtmovie->stream);
        if (sub_chunk_len <= 1 || sub_chunk_len > size_remaining)
        {
            DEBUGF("strange size (%lu) for chunk inside stbl\n",
                   (unsigned long)sub_chunk_len);
            return false;
        }

        sub_chunk_id = stream_read_uint32(qtmovie->stream);
        DEBUGF("Found a sub_chunk %c%c%c%c, length=%d\n",SPLITFOURCC(sub_chunk_id), sub_chunk_len);

        switch (sub_chunk_id)
        {
        case MAKEFOURCC('s','t','s','d'):
            if (!read_chunk_stsd(qtmovie, sub_chunk_len)) {
               return false;
            }
            break;
        case MAKEFOURCC('s','t','t','s'):
            if (!read_chunk_stts(qtmovie, sub_chunk_len))
            {
               return false;
            }
            break;
        case MAKEFOURCC('s','t','s','z'):
            if (!read_chunk_stsz(qtmovie, sub_chunk_len))
            {
               return false;
            }
            break;
        case MAKEFOURCC('s','t','s','c'):
            if (!read_chunk_stsc(qtmovie, sub_chunk_len))
            {
               return false;
            }
            break;
        case MAKEFOURCC('s','t','c','o'):
            if (!read_chunk_stco(qtmovie, sub_chunk_len))
            {
               DEBUGF("failed to read chunk stco\n");
               return false;
            }
            break;
        default:
            //DEBUGF("(stbl) unknown chunk id: %c%c%c%c\n",
            //       SPLITFOURCC(sub_chunk_id));
            stream_skip(qtmovie->stream, sub_chunk_len - 8);
        }

        size_remaining -= sub_chunk_len;
    }
    return true;
}

static bool read_chunk_minf(qtmovie_t *qtmovie, size_t chunk_len)
{
    size_t size_remaining = chunk_len - 8;
    uint32_t i;

    /* Check for smhd, only kind of minf we care about */

    if ((i = stream_read_uint32(qtmovie->stream)) != 16)
    {
        DEBUGF("unexpected size in media info: %ld\n", (long)i);
        stream_skip(qtmovie->stream, size_remaining-4);
        return true;
    }

    if (stream_read_uint32(qtmovie->stream) != MAKEFOURCC('s','m','h','d'))
    {
        DEBUGF("not a sound header! can't handle this.\n");
        return false;
    }

    /* now skip the rest of the atom */
    stream_skip(qtmovie->stream, 16 - 8);
    size_remaining -= 16;

    while (size_remaining)
    {
        size_t sub_chunk_len;
        fourcc_t sub_chunk_id;

        sub_chunk_len = stream_read_uint32(qtmovie->stream);

        if (sub_chunk_len <= 1 || sub_chunk_len > size_remaining)
        {
            DEBUGF("strange size (%lu) for chunk inside minf\n",
                   (unsigned long)sub_chunk_len);
            return false;
        }

        sub_chunk_id = stream_read_uint32(qtmovie->stream);

        printf("Found a sub_chunk %c%c%c%c, length=%d\n",SPLITFOURCC(sub_chunk_id), sub_chunk_len);

        switch (sub_chunk_id)
        {
        case MAKEFOURCC('s','t','b','l'):
            if (!read_chunk_stbl(qtmovie, sub_chunk_len)) {
                DEBUGF("read_chunk_stbl failed");
                return false;
            }
            break;
        default:
            //DEBUGF("(minf) unknown chunk id: %c%c%c%c\n",
            //       SPLITFOURCC(sub_chunk_id));
            stream_skip(qtmovie->stream, sub_chunk_len - 8);
            break;
        }

        size_remaining -= sub_chunk_len;
    }
    return true;
}

static bool read_chunk_mdia(qtmovie_t *qtmovie, size_t chunk_len)
{
    size_t size_remaining = chunk_len - 8;

    while (size_remaining)
    {
        size_t sub_chunk_len;
        fourcc_t sub_chunk_id;

        sub_chunk_len = stream_read_uint32(qtmovie->stream);
        if (sub_chunk_len <= 1 || sub_chunk_len > size_remaining)
        {
            DEBUGF("strange size (%lu) for chunk inside mdia\n",
                   (unsigned long)sub_chunk_len);
            return false;
        }

        sub_chunk_id = stream_read_uint32(qtmovie->stream);
        printf("Found a sub_chunk %c%c%c%c, length=%d\n",SPLITFOURCC(sub_chunk_id), sub_chunk_len);

        switch (sub_chunk_id)
        {
        case MAKEFOURCC('m','i','n','f'):
            if (!read_chunk_minf(qtmovie, sub_chunk_len)) {
               DEBUGF("read_chunk_minf failed");
               return false;
            }
            break;
        default:
            //DEBUGF("(mdia) unknown chunk id: %c%c%c%c\n",
            //       SPLITFOURCC(sub_chunk_id));
            stream_skip(qtmovie->stream, sub_chunk_len - 8);
            break;
        }

        size_remaining -= sub_chunk_len;
    }
    return true;
}

/* 'trak' - a movie track - contains other atoms */
static bool read_chunk_trak(qtmovie_t *qtmovie, size_t chunk_len)
{
    size_t size_remaining = chunk_len - 8;

    while (size_remaining)
    {
        size_t sub_chunk_len;
        fourcc_t sub_chunk_id;

        sub_chunk_len = stream_read_uint32(qtmovie->stream);
        if (sub_chunk_len <= 1 || sub_chunk_len > size_remaining)
        {
            DEBUGF("strange size (%lu) for chunk inside trak\n",
                   (unsigned long)sub_chunk_len);
            return false;
        }

        sub_chunk_id = stream_read_uint32(qtmovie->stream);
        printf("Found a sub_chunk %c%c%c%c, length=%d\n",SPLITFOURCC(sub_chunk_id), sub_chunk_len);

        switch (sub_chunk_id)
        {
        case MAKEFOURCC('m','d','i','a'):
            if (!read_chunk_mdia(qtmovie, sub_chunk_len)) {
               DEBUGF("read_chunk_mdia failed");
               return false;
            }
            break;
        default:
            //DEBUGF("(trak) unknown chunk id: %c%c%c%c\n",
            //        SPLITFOURCC(sub_chunk_id));
            stream_skip(qtmovie->stream, sub_chunk_len - 8);
            break;
        }

        size_remaining -= sub_chunk_len;
    }
    return true;
}

/* 'moov' movie atom - contains other atoms */
static bool read_chunk_moov(qtmovie_t *qtmovie, size_t chunk_len)
{
    size_t size_remaining = chunk_len - 8;

    while (size_remaining)
    {
        size_t sub_chunk_len;
        fourcc_t sub_chunk_id;

        sub_chunk_len = stream_read_uint32(qtmovie->stream);
        if (sub_chunk_len <= 1 || sub_chunk_len > size_remaining)
        {
            DEBUGF("strange size (%lu) for chunk inside moov\n",
                   (unsigned long)sub_chunk_len);
            return false;
        }

        sub_chunk_id = stream_read_uint32(qtmovie->stream);
        printf("Found moov->%c%c%c%c, length=%d\n",SPLITFOURCC(sub_chunk_id), sub_chunk_len);

        switch (sub_chunk_id)
        {
        case MAKEFOURCC('t','r','a','k'):
            if (!read_chunk_trak(qtmovie, sub_chunk_len)) {
               DEBUGF("read_chunk_trak failed");
               return false;
            }
            break;

        case MAKEFOURCC('u','d','t','a'):
            stream_skip(qtmovie->stream, sub_chunk_len - 8);
            break;

        default:
            //DEBUGF("(moov) unknown chunk id: %c%c%c%c\n",
            //        SPLITFOURCC(sub_chunk_id));
            stream_skip(qtmovie->stream, sub_chunk_len - 8);
            break;
        }

        size_remaining -= sub_chunk_len;
    }
    return true;
}

static void read_chunk_mdat(qtmovie_t *qtmovie, size_t chunk_len)
{
    size_t size_remaining = chunk_len - 8;

    qtmovie->res->mdat_len = size_remaining;
}

int qtmovie_read(stream_t *file, demux_res_t *demux_res)
{
    qtmovie_t qtmovie;

    /* construct the stream */
    qtmovie.stream = file;
    qtmovie.res = demux_res;

    /* read the chunks */
    while (1)
    {
        size_t chunk_len;
        fourcc_t chunk_id;

        chunk_len = stream_read_uint32(qtmovie.stream);
        if (stream_eof(qtmovie.stream))
        {
            DEBUGF("stream_eof reached\n");
            if(qtmovie.res->mdat_offset == 0 || qtmovie.res->format == 0)
                return 0;
            stream_seek(qtmovie.stream, qtmovie.res->mdat_offset);
            return 1;
        }

        if (chunk_len == 1)
        {
            //DEBUGF("need 64bit support\n");
            return 0;
        }
        chunk_id = stream_read_uint32(qtmovie.stream);

        DEBUGF("Found a chunk %c%c%c%c, length=%d\n",SPLITFOURCC(chunk_id),chunk_len);
        switch (chunk_id)
        {
        case MAKEFOURCC('f','t','y','p'):
            read_chunk_ftyp(&qtmovie, chunk_len);
            break;
        case MAKEFOURCC('m','o','o','v'):
            if (!read_chunk_moov(&qtmovie, chunk_len)) {
               DEBUGF("read_chunk_moov failed");
               return 0;
            }
            break;
        case MAKEFOURCC('m','d','a','t'):
            /* There can be empty mdats before the real one. If so, skip them */
            if (chunk_len == 8)
                break;
            read_chunk_mdat(&qtmovie, chunk_len);
            qtmovie.res->mdat_offset=stream_tell(qtmovie.stream);
            /* If we've already seen the format, assume there's nothing
               interesting after the mdat chunk (the file is "streamable").
               This avoids having to seek, which might cause rebuffering. */
            if(qtmovie.res->format > 0)
                return 1;
            stream_skip(qtmovie.stream, chunk_len - 8);
            break;

            /*  these following atoms can be skipped !!!! */
        case MAKEFOURCC('f','r','e','e'):
            stream_skip(qtmovie.stream, chunk_len - 8);
            break;

        default:
            DEBUGF("(top) unknown chunk id: %c%c%c%c\n",SPLITFOURCC(chunk_id));
            return 0;
        }

    }
    return 0;
}


