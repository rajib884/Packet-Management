#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic-buffer.h"

/*****************************************************************************
 *
 *   Name:       dynamic_buffer_create
 *
 *   Input:      initial_capacity  Initial capacity of the dynamic buffer
 *   Return:     Success           A pointer to the newly created dynamic_buffer_t
 *               Failed            NULL
 *   Description:            Allocates and initializes a dynamic buffer with the
 *                           specified initial capacity.
 ******************************************************************************/
dynamic_buffer_t *dynamic_buffer_create(size_t initial_capacity) /* OK */
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

/*****************************************************************************
 *
 *   Name:       dynamic_buffer_resize
 *
 *   Input:      buf              A pointer to the dynamic buffer to resize
 *               new_capacity     The new capacity for the dynamic buffer
 *   Return:     Success          true if the buffer was resized successfully
 *               Failed           false if the operation failed
 *   Description:            Resizes the dynamic buffer to the specified new capacity.
 ******************************************************************************/
bool dynamic_buffer_resize(dynamic_buffer_t *buf, size_t new_capacity) /* OK */
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

/*****************************************************************************
 *
 *   Name:       dynamic_buffer_add_data
 *
 *   Input:      buf              A pointer to the dynamic buffer to which data will be added
 *               data             A pointer to the data to be added
 *               data_size        The size of the data to be added
 *   Return:     Success          true if the data was added successfully
 *               Failed           false if the operation failed
 *   Description:            Adds the specified data to the dynamic buffer. Automatically
 *                           resizes the buffer if the current capacity is insufficient.
 ******************************************************************************/
bool dynamic_buffer_add_data(dynamic_buffer_t *buf, const uint8_t *data, size_t data_size) /* OK */
{
    size_t needed_capacity = 0;
    size_t capacity = 0;

    if (buf == NULL || data == NULL || buf->capacity == 0)
    {
        return false;
    }

    /* Check available capacity */
    needed_capacity = buf->size + data_size;
    capacity = buf->capacity;

    while (capacity < needed_capacity)
    {
        if (capacity > SIZE_MAX / 2)
        {
            capacity = SIZE_MAX;
            break;
        }

        capacity *= 2;
    }

    if (capacity > buf->capacity && !dynamic_buffer_resize(buf, capacity))
    {
        return false;
    }

    if (buf->capacity < buf->size + data_size)
    {
        return false; /* Prevent overflow */
    }

    /* Copy the data */
    memcpy(buf->data + buf->size, data, data_size);
    buf->size += data_size;

    return true;
}

/*****************************************************************************
 *
 *   Name:       dynamic_buffer_free
 *
 *   Input:      buf_p            A pointer to a pointer to the dynamic buffer to free
 *   Return:     None
 *   Description:            Frees all memory associated with the specified dynamic buffer.
 *                           The buffer pointer is set to NULL after freeing.
 ******************************************************************************/
void dynamic_buffer_free(dynamic_buffer_t **buf_p) /* OK */
{
    if (buf_p == NULL || *buf_p == NULL)
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
