#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "packet-linked-list.h"

void packet_free_list_node(ListNode_t *list_node)
{
    free(list_node);
    list_node = NULL;

    return;
}

packet_node_t *packet_create_list_node(ipv4_datagram_t *datagram)
{
    packet_node_t *node = (packet_node_t *)calloc(1, sizeof(packet_node_t));
    memcpy(&node->dest, &datagram->header->destination_address, sizeof(ip_addr_t));
    memcpy(&node->src, &datagram->header->source_address, sizeof(ip_addr_t));
    node->ref_counter = 1;

    return node;
}

void print_packet_linked_list_packets(packet_node_t *head)
{
    packet_node_t *current = NULL;
    uint64_t i = 1;

    current = head;

    while (current != NULL)
    {
        printf("Packet %" PRIu64 "\nSource: ", i);
        print_ip_addr(&current->src);
        printf("\nDestination: ");
        print_ip_addr(&current->dest);
        printf("\nRef counter: %" PRIu64 "\n\n", current->ref_counter);

        current = (packet_node_t *)current->node.next;
        i++;
    }

    return;
}
