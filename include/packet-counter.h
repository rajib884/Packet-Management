#ifndef __PACKET_COUNTER_H__
#define __PACKET_COUNTER_H__

#include "hash-table.h"
#include "ipv4-packet.h"
#include "singly-linked-list.h"

typedef struct packet_node
{
    ListNode_t node;      /* linked list base */
    ip_addr_t src;        /* source ip address */
    ip_addr_t dest;       /* destination ip address */
    uint64_t ref_counter; /* how many packets with this source and destination */
} packet_node_t;

typedef struct packet_counter
{
    packet_node_t *linked_list; /* head of the linked list */
    HashTable_t *hash_table;    /* hash table pointing to linked list nodes*/
    free_data_t list_node_free; /* function to free the linked list nodes */
    void *(*hash_key_from_ipv4)(ipv4_datagram_t *);
} packet_counter_t;

packet_counter_t *packet_counter_create();
void packet_counter_increase(packet_counter_t *counter, ipv4_datagram_t *datagram);
void packet_counter_free(packet_counter_t **counter_p);
void print_packet_counter_hash_table(packet_counter_t *counter);
void print_packet_counter_linked_list(packet_counter_t *counter);

#endif /* __PACKET_COUNTER_H__ */