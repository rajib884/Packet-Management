#ifndef __IPV4_PACKET_H__
#define __IPV4_PACKET_H__

#include "ethernet-frame.h"

#define IP_ADDRESS_LENGTH 4

#define IPV4_PROTOCOL_UDP 0x11
#define IPV4_PROTOCOL_TCP 0x06

#define IPV4_MASK_FLAG_MORE_FRAGMENTS 0x01
#define IPV4_MASK_FLAG_DONT_FRAGMENT 0x02

typedef struct ip_addr
{
    uint8_t byte[IP_ADDRESS_LENGTH];
} ip_addr_t;

typedef union {
    struct
    {
#if BYTE_ORDER == LITTLE_ENDIAN
        uint16_t fragment_offset : 13;
        uint16_t flags : 3;
#elif BYTE_ORDER == BIG_ENDIAN
        uint16_t flags : 3;
        uint16_t fragment_offset : 13;
#else
    #error "Unknown Byte Order!"
#endif
    } fields;
    uint16_t raw_data;
} ipv4_fragment_offset_flags_t;

#pragma pack(push, 1)
typedef struct ipv4_header
{
#if BYTE_ORDER == LITTLE_ENDIAN
    uint8_t header_length : 4;
    uint8_t version : 4;
#elif BYTE_ORDER == BIG_ENDIAN
    uint8_t version : 4;
    uint8_t header_length : 4;
#else
    #error "Unknown Byte Order!"
#endif
    uint8_t type_of_service;
    uint16_t total_length;
    uint16_t identification;
    ipv4_fragment_offset_flags_t fragment_offset_flag;
    uint8_t time_to_live;
    uint8_t protocol;
    uint16_t checksum;
    ip_addr_t source_address;
    ip_addr_t destination_address;
} ipv4_header_t;
#pragma pack(pop)

typedef struct ipv4_datagram
{
    ipv4_header_t *header;
    size_t option_len;
    uint8_t *options;
    size_t data_len;
    uint8_t data[];
} ipv4_datagram_t;

ipv4_datagram_t *ipv4_datagram_from_ethernet_frame(ethernet_frame_t *frame);
void ipv4_datagram_free(ipv4_datagram_t **datagram_p);
int print_ip_addr(ip_addr_t *ip);
void print_ipv4(ipv4_datagram_t *datagram, bool print_data);

#endif /* __IPV4_PACKET_H__ */
