#include <inttypes.h>
#include <stdlib.h>

#include "packet-linked-list.h"

static packet_node_t *packet_node_head = NULL;

static void free_packet_node(ListNode_t *list_node)
{
    free(list_node);
    list_node = NULL;

    return;
}

void packet_linked_list_add_packet(packet_node_t *packet_node)
{
    if (packet_node == NULL)
    {
        return;
    }

    linked_list_insert_at_head((ListNode_t **)&packet_node_head, (ListNode_t *)packet_node);

    return;
}

void packet_linked_list_delete_list()
{
    linked_list_delete_list((ListNode_t **)&packet_node_head, free_packet_node);
}

void print_packet_linked_list_packets()
{
    packet_node_t *current = NULL;
    uint64_t i = 1;

    current = packet_node_head;

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
