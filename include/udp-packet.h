#ifndef __UDP_PACKET_H__
#define __UDP_PACKET_H__

#include <stdint.h>

#include "ipv4-packet.h"

#pragma pack(push, 1)
typedef struct udp_header
{
    uint16_t source_port;
    uint16_t dest_port;
    uint16_t length;
    uint16_t checksum;
} udp_header_t;
#pragma pack(pop)

typedef struct udp_packet
{
    udp_header_t *header;
    size_t data_len;
    uint8_t data[];
} udp_packet_t;

udp_packet_t *ipv4_datagram_to_udp_packet(ipv4_datagram_t *datagram);
void free_udp_packet(udp_packet_t **packet_p);

#endif /* __UDP_PACKET_H__ */