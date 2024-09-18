#ifndef __PACKET_LINKED_LIST_H__
#define __PACKET_LINKED_LIST_H__

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

void packet_linked_list_add_packet(packet_node_t *packet);
void print_packet_linked_list_packets();
void packet_linked_list_delete_list();

#endif /* __PACKET_LINKED_LIST_H__ */