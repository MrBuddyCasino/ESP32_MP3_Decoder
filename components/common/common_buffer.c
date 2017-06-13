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


static void buf_move_remaining_bytes_to_front(buffer_t *buf)
{
    size_t unread_data = buf_data_unread(buf);

    // move remaining data to front
    memmove(buf->base, buf->read_pos, unread_data);
    buf->read_pos = buf->base;
    buf->write_pos = buf->base + unread_data;
}


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
    buf->write_pos = buf->base;
    buf->bytes_consumed = 0;

    return buf;
}

/* wraps an existing buffer */
buffer_t *buf_wrap(void *existing, size_t len)
{
    buffer_t* buf = calloc(1, sizeof(buffer_t));

    buf->len = len;
    buf->base = existing;
    buf->read_pos = buf->base;
    buf->write_pos = buf->base;
    buf->bytes_consumed = 0;

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

/* TODO */
int buf_resize(buffer_t *buf, size_t new_size)
{
    if(buf == NULL)
        return -1;

    if(buf->len > new_size) {
        ESP_LOGE(TAG, "shrinking unsupported");
        return -1;
    }

    size_t stale_bytes = buf_data_stale(buf);
    size_t total_bytes = buf_data_total(buf);

    void *new_buf = realloc(buf->base, new_size);
    if(new_buf == NULL) {
        ESP_LOGE(TAG, "buf_resize(%d) failed", new_size);
        return -1;
    }

    buf->len = new_size;
    buf->base = new_buf;
    buf->read_pos = buf->base + stale_bytes;
    buf->write_pos = buf->base + total_bytes;

    return 0;
}

size_t buf_write(buffer_t *buf, const void* from, size_t len)
{
    size_t bytes_to_write = min(buf_free_capacity_after_purge(buf), len);

    if (bytes_to_write > 0) {
        memcpy(buf->write_pos, from, bytes_to_write);
        buf->write_pos += bytes_to_write;
    }

    return bytes_to_write;
}

/* available unused capacity */
size_t buf_free_capacity_after_purge(buffer_t *buf)
{
    if(buf == NULL) return -1;

    size_t unused_capacity = (buf->base + buf->len) - buf->write_pos;
    return buf_data_stale(buf) + unused_capacity;
}

/* amount of bytes unread */
size_t buf_data_total(buffer_t *buf)
{
    if(buf == NULL) return -1;

    return buf->write_pos - buf->base;
}

/* amount of bytes unread */
size_t buf_data_unread(buffer_t *buf)
{
    if(buf == NULL) return -1;

    return buf->write_pos - buf->read_pos;
}

/* amount of bytes already consumed */
size_t buf_data_stale(buffer_t *buf)
{
    if(buf == NULL) return -1;

    return buf->read_pos - buf->base;
}



size_t fill_read_buffer(buffer_t *buf)
{
    buf_move_remaining_bytes_to_front(buf);
    size_t bytes_to_read = min(buf_free_capacity_after_purge(buf), spiRamFifoFill());

    if (bytes_to_read > 0) {
        spiRamFifoRead((char *) buf->write_pos, bytes_to_read);
        buf->write_pos += bytes_to_read;
    }

    return bytes_to_read;
}


int buf_seek_rel(buffer_t *buf, uint32_t offset)
{
    if (buf == NULL) return -1;

    // advance through buffer, loading new data as necessary
    while(1) {
        size_t data_avail = buf_data_unread(buf);

        // if offset exceeds buffer capacity, load more data
        if(offset > data_avail) {
            buf->read_pos += data_avail;
            offset -= data_avail;
            buf->bytes_consumed += data_avail;
            fill_read_buffer(buf);
        } else {
            buf->read_pos += offset;
            buf->bytes_consumed += offset;
            break;
        }
    }

    return 0;
}

int buf_seek_abs(buffer_t *buf, uint32_t pos)
{
    if (buf == NULL) return -1;

    if(pos > buf->write_pos) {
        ESP_LOGE(TAG, "buf_seek_abs failed, pos = %u larger than fill_pos %u", pos, (uint32_t) buf->write_pos);
        return -1;
    }

    size_t delta = pos - (uint32_t) buf->read_pos;
    buf->bytes_consumed += delta;
    buf->read_pos = pos;

    return 0;
}

size_t buf_read(void * ptr, size_t size, size_t count, buffer_t *buf)
{
    if(size == 0 || count == 0)
        return 0;

    size_t bytes_to_copy = size * count;
    if(bytes_to_copy > buf->len) {
        ESP_LOGE(TAG, "buf_read failed, bytes_to_copy = %d larger than buffer size %d", bytes_to_copy, buf->len);
        return -1;
    }

    uint16_t delay = 0;
    while(bytes_to_copy > buf_data_unread(buf) && delay < 5000) {
        fill_read_buffer(buf);
        vTaskDelay(50 / portTICK_PERIOD_MS);
        delay += 50;
    }

    if(bytes_to_copy > buf_data_unread(buf)) {
        ESP_LOGE(TAG, "buf_read failed bytes_to_copy %d, buf_data_unread %d", bytes_to_copy, buf_data_unread(buf));
        return -1;
    }

    memcpy(ptr, buf->read_pos, bytes_to_copy);

    buf->read_pos += bytes_to_copy;
    buf->bytes_consumed += bytes_to_copy;

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
