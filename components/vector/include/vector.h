/*
 * vector.h
 *
 *  Created on: 10.06.2017
 *      Author: michaelboeckling
 */

#ifndef _INCLUDE_VECTOR_H_
#define _INCLUDE_VECTOR_H_

typedef struct vec_struct_t vec_t;

vec_t *vec_init(size_t elem_size, size_t init_capacity);
void vec_add(vec_t *v, void* elem);
void *vec_get(vec_t *v, size_t idx);
size_t vec_size(vec_t *v);

#endif /* _INCLUDE_VECTOR_H_ */
