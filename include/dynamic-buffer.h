#ifndef __DYNAMIC_BUFFER_H__
#define __DYNAMIC_BUFFER_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct dynamic_buffer
{
    uint8_t *data;   /* buffer data */
    size_t size;     /* size of the data */
    size_t capacity; /* capacity of the buffer */
} dynamic_buffer_t;

dynamic_buffer_t *dynamic_buffer_create(size_t initial_capacity);
bool dynamic_buffer_resize(dynamic_buffer_t *buf, size_t new_capacity);
bool dynamic_buffer_add_data(dynamic_buffer_t *buf, const uint8_t *data, size_t data_size);
void dynamic_buffer_free(dynamic_buffer_t **buf_p);

#endif /* __DYNAMIC_BUFFER_H__ */