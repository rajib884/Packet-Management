#ifndef __LINKED_LIST_PACKET_H__
#define __LINKED_LIST_PACKET_H__

#include <stdint.h>

#include "ipv4-packet.h"
#include "singly-linked-list.h"

typedef struct packet_node
{
    ListNode_t node;
    ip_addr_t src;
    ip_addr_t dest;
    uint64_t ref_counter;
} packet_node_t;

void add_packet(packet_node_t *packet);
void print_packets();

#endif /* __LINKED_LIST_PACKET_H__ */