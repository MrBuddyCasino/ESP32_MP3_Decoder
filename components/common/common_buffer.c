/*
 * common_buffer.c
 *
 *  Created on: 28.04.2017
 *      Author: michaelboeckling
 */

#include "common_buffer.h"

#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "spiram_fifo.h"
#include "byteswap.h"

#define TAG "common"

/* creates a buffer struct and its storage on the heap */
buffer_t *buf_create(size_t len)
{
    buffer_t* buf = calloc(1, sizeof(buffer_t));

    buf->len = len;
    buf->base = calloc(len, sizeof(uint8_t));
    if(buf->base == NULL) {
        ESP_LOGE(TAG, "couldn't allocate buffer of size %d", len);
        return NULL;
    }
    buf->read_pos = buf->base;
    buf->fill_pos = buf->base;

    return buf;
}

/* free the buffer struct and its storage */
int buf_destroy(buffer_t *buf)
{
    if(buf == NULL)
        return -1;

    if(buf->base != NULL)
        free(buf->base);

    free(buf);

    return 0;
}

/* available unused capacity */
size_t buf_free_capacity(buffer_t *buf)
{
    if(buf == NULL) return -1;

    size_t unused_capacity = (buf->base + buf->len) - buf->fill_pos;
    return buf_data_consumed(buf) + unused_capacity;
}

/* amount of bytes unread */
size_t buf_data_total(buffer_t *buf)
{
    if(buf == NULL) return -1;

    return buf->fill_pos - buf->base;
}

/* amount of bytes unread */
size_t buf_data_unread(buffer_t *buf)
{
    if(buf == NULL) return -1;

    return buf->fill_pos - buf->read_pos;
}

/* amount of bytes already consumed */
size_t buf_data_consumed(buffer_t *buf)
{
    if(buf == NULL) return -1;

    return buf->read_pos - buf->base;
}

void buf_move_remaining_bytes_to_front(buffer_t *buf)
{
    size_t unread_data = buf_data_unread(buf);

    // move remaining data to front
    memmove(buf->base, buf->read_pos, unread_data);
    buf->read_pos = buf->base;
    buf->fill_pos = buf->base + unread_data;
}

size_t fill_read_buffer(buffer_t *buf)
{
    buf_move_remaining_bytes_to_front(buf);
    size_t bytes_to_read = min(buf_free_capacity(buf), spiRamFifoFill());

    if (bytes_to_read > 0) {
        spiRamFifoRead((char *) buf->fill_pos, bytes_to_read);
        buf->fill_pos += bytes_to_read;
    }

    return bytes_to_read;
}


int buf_seek_rel(buffer_t *buf, uint32_t offset)
{
    if (buf == NULL) return -1;

    // advance through buffer, loading new data as necessary
    while(1) {
        size_t data_avail = buf->fill_pos - buf->read_pos;

        // if offset exceeds buffer capacity, load more data
        if(offset > data_avail) {
            buf->read_pos += data_avail;
            offset -= data_avail;
            fill_read_buffer(buf);
        } else {
            buf->read_pos += offset;
            break;
        }
    }

    return 0;
}

int buf_seek_abs(buffer_t *buf, uint32_t pos)
{
    if (buf == NULL) return -1;

    if(pos > buf->fill_pos) {
        return -1;
    }

    buf->read_pos = pos;

    return 0;
}

size_t buf_read(void * ptr, size_t size, size_t count, buffer_t *buf)
{
    if(size == 0 || count == 0)
        return 0;

    size_t bytes_to_copy = size * count;
    if(bytes_to_copy > buf->len)
        return -1;

    size_t bytes_avail = buf->fill_pos - buf->read_pos;
    if(bytes_avail < bytes_to_copy) {
        fill_read_buffer(buf);
    }

    memcpy(ptr, buf->read_pos, bytes_to_copy);

    buf->read_pos += bytes_to_copy;

    return bytes_to_copy;
}


//read big endian 16-Bit from fileposition(position)
uint16_t fread16(buffer_t *buf, size_t position)
{
    uint16_t tmp16;
    buf_seek_rel(buf, position);
    buf_read((uint8_t *) &tmp16, sizeof(tmp16), 1, buf);
    return __bswap_16(tmp16);
}

//read big endian 32-Bit from fileposition(position)
uint32_t fread32(buffer_t *buf, size_t position)
{
    uint32_t tmp32;
    buf_seek_rel(buf, position);
    buf_read((uint8_t *) &tmp32, sizeof(tmp32), 1, buf);
    return __bswap_32(tmp32);
}
