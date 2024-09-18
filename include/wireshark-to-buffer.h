#ifndef __WIRESHARK_TO_BUFFER_H__
#define __WIRESHARK_TO_BUFFER_H__

#include <stdbool.h>
#include <stdint.h>

#include "dynamic-buffer.h"

typedef struct wireshark_file
{
    long current_pos;
    const char *file_path;
} wireshark_file_t;

wireshark_file_t *init_wireshark_file(const char *file_path);
void free_wireshark_file(wireshark_file_t **ws_file_p);
dynamic_buffer_t *get_next_packet(wireshark_file_t *ws_file);

#endif /* __WIRESHARK_TO_BUFFER_H__*/