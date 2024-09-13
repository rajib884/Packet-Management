#ifndef __WIRESHARK_TO_BUFFER_H__
#define __WIRESHARK_TO_BUFFER_H__

#include <stdbool.h>
#include <stdint.h>

#include "dynamic-buffer.h"

#define WIRESHARK_DATA_FILE "data/udp-big.txt"

dynamic_buffer_t *get_next_packet();

#endif /* __WIRESHARK_TO_BUFFER_H__*/