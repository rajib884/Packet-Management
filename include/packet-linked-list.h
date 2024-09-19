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

void packet_free_list_node(ListNode_t *list_node);
void print_packet_linked_list_packets(packet_node_t *head);
packet_node_t *packet_create_list_node(ipv4_datagram_t *datagram);

#endif /* __PACKET_LINKED_LIST_H__ */