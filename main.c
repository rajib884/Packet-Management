#include <stdio.h>
#include <stdlib.h>

#include "packet-hash-table.h"
#include "packet-linked-list.h"
#include "wireshark-to-buffer.h"

HashTable_t *packet_hash_table = NULL;
packet_node_t *packet_linked_list = NULL; // head

int main(void)
{
    wireshark_file_t *ws_file = NULL;
    dynamic_buffer_t *buf = NULL;
    ethernet_frame_t *frame = NULL;
    ipv4_datagram_t *datagram = NULL;
    packet_node_t *node = NULL;
    void *key = NULL;

    packet_hash_table = packet_hash_table_create(100);
    ws_file = wireshark_file_create("data/multi.txt");

    while (wireshark_file_readable(ws_file))
    {
        buf = wireshark_file_get_next_packet(ws_file);
        frame = ethernet_frame_from_dynamic_buffer(buf);
        dynamic_buffer_free(&buf);
        datagram = ipv4_datagram_from_ethernet_frame(frame);
        ethernet_frame_free(&frame);

        if (datagram == NULL || datagram->header->protocol != IPV4_PROTOCOL_UDP)
        {
            // printf("Invalid packet.\n");
            ipv4_datagram_free(&datagram);
            continue;
        }

        key = key_from_ip(datagram);
        node = (packet_node_t *)hash_table_get_item(packet_hash_table, key);

        if (node == NULL)
        {
            node = packet_create_list_node(datagram);
            linked_list_insert_at_head((ListNode_t **)&packet_linked_list, (ListNode_t *)node);
            hash_table_add_item(packet_hash_table, key, (void *)node);

            node = NULL;
        }
        else
        {
            node->ref_counter++;
            free(key);
            key = NULL;
        }

        ipv4_datagram_free(&datagram);
    }

    printf("hash_table size %lu\n", packet_hash_table->size);

    wireshark_file_free(&ws_file);
    hash_table_free(&packet_hash_table);
    linked_list_delete_list((ListNode_t **)&packet_linked_list, packet_free_list_node);

    return 0;
}
