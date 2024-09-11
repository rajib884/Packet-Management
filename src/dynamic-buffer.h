#ifndef __DYNAMIC_BUFFER_H__
#define __DYNAMIC_BUFFER_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

typedef struct dynamic_buffer
{
    uint8_t *data;   // Pointer to the buffer data
    size_t size;     // Current size of the buffer (number of bytes)
    size_t capacity; // Current capacity of the buffer (number of bytes)
} dynamic_buffer_t;

dynamic_buffer_t *create_dynamic_buffer(size_t initial_capacity);
bool resize_buffer(dynamic_buffer_t *buf, size_t new_capacity);
bool add_data(dynamic_buffer_t *buf, const uint8_t *data, size_t data_size);
void free_buffer(dynamic_buffer_t **buf_p);

#endif /* __DYNAMIC_BUFFER_H__ */