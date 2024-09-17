#include <inttypes.h>

#include "linked-list-packet.h"

packet_node_t *head = NULL;

void add_packet(packet_node_t *packet)
{
    if (packet == NULL)
    {
        return;
    }

    insert_at_head((ListNode_t **)&head, (ListNode_t *)packet);

    return;
}

void print_packets()
{
    packet_node_t *current = NULL;
    uint64_t i = 1;

    current = head;

    while (current != NULL)
    {
        printf("Packet %" PRIu64 "\nSource Address: ", i);
        print_ip_addr(&current->src);
        printf("\nDestination Address: ");
        print_ip_addr(&current->dest);
        printf("\nRef counter: %" PRIu64 "\n\n", current->ref_counter);

        current = (packet_node_t *)current->node.next;
        i++;
    }

    return;
}
