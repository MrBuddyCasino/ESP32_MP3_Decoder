/*
 * common.h
 *
 *  Created on: 27.04.2017
 *      Author: michaelboeckling
 */

#ifndef _INCLUDE_COMMON_BUFFER_H_
#define _INCLUDE_COMMON_BUFFER_H_

#include <inttypes.h>
#include <stddef.h>

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

typedef struct
{
    uint8_t *base;
    uint8_t *read_pos;
    uint8_t *fill_pos;
    uint16_t len;
    uint32_t bytes_consumed;
} buffer_t;

/* create a buffer on the heap */
buffer_t *buf_create(size_t len);

/* free the backing storage, and the struct itself */
int buf_destroy(buffer_t *buf);

/**
 * Seek from the current position of the pointer.
 */
int buf_seek_rel(buffer_t *buf, uint32_t pos);

int buf_seek_abs(buffer_t *buf, uint32_t pos);

/* available unused capacity */
size_t buf_free_capacity(buffer_t *buf);

/* total amount of data in the buffer */
size_t buf_data_total(buffer_t *buf);

/* bytes left to be consumed */
size_t buf_data_unread(buffer_t *buf);

/* stale bytes that have already been consumed */
size_t buf_data_stale(buffer_t *buf);

/**
 * Reads an array of count elements, each one with a size of size bytes,
 * from the stream and stores them in the block of memory specified by ptr.
 *
 * The position indicator of the stream is advanced by the total amount of bytes read.
 *
 * The total amount of bytes read if successful is (size*count).
 *
 * @param ptr Pointer to a block of memory with a size of at least (size*count) bytes, converted to a void*.
 * @param size Size, in bytes, of each element to be read.
 * @param count Number of elements, each one with a size of size bytes.
 */
size_t buf_read( void * ptr, size_t size, size_t count, buffer_t *buf);

uint16_t fread16(buffer_t *buf, size_t position);
uint32_t fread32(buffer_t *buf, size_t position);

unsigned int REV16( unsigned int value);
unsigned int REV32( unsigned int value);

size_t fill_read_buffer(buffer_t *buf);

#endif /* _INCLUDE_COMMON_BUFFER_H_ */
