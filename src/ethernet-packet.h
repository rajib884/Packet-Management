#ifndef __ETHERNET_PACKET_H__
#define __ETHERNET_PACKET_H__

#include "dynamic-buffer.h"

#define MAC_LENGTH 6

#define ETHERNET_MINIMUM_DATA_LEN 46

#define ETHERTYPE_IPV4 0x0800
#define ETHERTYPE_IPV6 0x86DD

typedef struct mac_address
{
    uint8_t bytes[MAC_LENGTH];
} mac_address_t;

#pragma pack(push, 1)
typedef struct ethernet_header
{
    mac_address_t destination;
    mac_address_t source;
    uint16_t ethertype;
} ethernet_header_t;
#pragma pack(pop)

typedef struct ethernet_frame
{
    ethernet_header_t *header;
    size_t data_len;
    uint8_t data[];
} ethernet_frame_t;

ethernet_frame_t *buffer_to_ethernet_frame(dynamic_buffer_t *buffer);
void print_mac(mac_address_t *mac);
void free_ethernet_frame(ethernet_frame_t **frame_p);

#endif /* __ETHERNET_PACKET_H__ */