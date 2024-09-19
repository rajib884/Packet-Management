// OK AFAIK

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic-buffer.h"

dynamic_buffer_t *dynamic_buffer_create(size_t initial_capacity)
{
    dynamic_buffer_t *buf = NULL;

    if (initial_capacity == 0)
    {
        return NULL;
    }

    buf = (dynamic_buffer_t *)calloc(1, sizeof(dynamic_buffer_t));

    if (buf == NULL)
    {
        return NULL;
    }

    buf->data = (uint8_t *)calloc(initial_capacity, sizeof(uint8_t));

    if (buf->data == NULL)
    {
        free(buf);
        buf = NULL;

        return NULL;
    }

    buf->size = 0;
    buf->capacity = initial_capacity;

    return buf;
}

bool dynamic_buffer_resize(dynamic_buffer_t *buf, size_t new_capacity)
{
    uint8_t *new_data = NULL;

    if (buf == NULL || new_capacity == 0)
    {
        return false;
    }

    new_data = (uint8_t *)realloc(buf->data, new_capacity);

    if (new_data == NULL)
    {
        return false;
    }

    buf->data = new_data;
    buf->capacity = new_capacity;

    return true;
}

bool dynamic_buffer_add_data(dynamic_buffer_t *buf, const uint8_t *data, size_t data_size)
{
    size_t needed_capacity = 0;
    size_t capacity = 0;

    if (buf == NULL || data == NULL)
    {
        return false;
    }

    /* Check available capacity */
    needed_capacity = buf->size + data_size;
    capacity = buf->capacity;

    while (capacity < needed_capacity)
    {
        capacity = capacity * 2;
    }

    if (capacity > buf->capacity && !dynamic_buffer_resize(buf, capacity))
    {
        return false;
    }

    /* Copy the data */
    memcpy(buf->data + buf->size, data, data_size);
    buf->size += data_size;

    return true;
}

void dynamic_buffer_free(dynamic_buffer_t **buf_p)
{
    if (buf_p == NULL)
    {
        return;
    }

    free((*buf_p)->data);
    (*buf_p)->data = NULL;
    (*buf_p)->size = 0;
    (*buf_p)->capacity = 0;

    free(*buf_p);
    *buf_p = NULL;

    return;
}
