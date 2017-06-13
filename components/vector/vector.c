/*
 * vector.c
 *
 *  Created on: 10.06.2017
 *      Author: michaelboeckling
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "vector.h"

struct vec_struct_t {
    void *data;
    size_t capacity;
    size_t elems;
    size_t elem_size;
};


vec_t *vec_init(size_t elem_size, size_t init_capacity)
{
    vec_t *v = calloc(1, sizeof(vec_t));
    if(!v) return NULL;

    v->data = malloc(init_capacity * elem_size);
    if (!v->data) {
        free(v);
        return NULL;
    }

    v->elems = 0;
    v->capacity = init_capacity;
    v->elem_size = elem_size;

    return v;
}

void vec_add(vec_t *v, void* elem)
{
    if(v->elems == v->capacity) {
        // add 50% capacity
        v->capacity = v->capacity + (v->capacity / 2);
        v->data = realloc(v->data, v->capacity * v->elem_size);
    }
    memcpy(v->data + (v->elems * v->elem_size), elem, v->elem_size);
    v->elems++;
}

void *vec_get(vec_t *v, size_t idx)
{
    if(idx >= v->elems) {
        return NULL;
    }

    return v->data + idx * v->elem_size;
}

size_t vec_size(vec_t *v)
{
    return v->elems;
}
