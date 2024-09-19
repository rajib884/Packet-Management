#ifndef __WIRESHARK_TO_BUFFER_H__
#define __WIRESHARK_TO_BUFFER_H__

#include <stdbool.h>
#include <stdint.h>

#include "dynamic-buffer.h"

typedef struct wireshark_file
{
    long current_pos;
    long file_length;
    const char *file_path;
} wireshark_file_t;

wireshark_file_t *wireshark_file_create(const char *file_path);
bool wireshark_file_readable(wireshark_file_t *ws_file);
void wireshark_file_free(wireshark_file_t **ws_file_p);
dynamic_buffer_t *wireshark_file_get_next_packet(wireshark_file_t *ws_file);

#endif /* __WIRESHARK_TO_BUFFER_H__*/