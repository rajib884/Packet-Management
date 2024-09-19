#include <stdio.h>
#include <stdlib.h>

#include "packet-counter.h"
#include "wireshark-to-buffer.h"

packet_counter_t *counter = NULL;

int main(void)
{
    wireshark_file_t *ws_file = NULL;
    dynamic_buffer_t *buf = NULL;
    ethernet_frame_t *frame = NULL;
    ipv4_datagram_t *datagram = NULL;

    counter = packet_counter_create();
    ws_file = wireshark_file_create("data/multi.txt");

    while (wireshark_file_readable(ws_file))
    {
        buf = wireshark_file_get_next_packet(ws_file);
        frame = ethernet_frame_from_dynamic_buffer(buf);
        dynamic_buffer_free(&buf);
        datagram = ipv4_datagram_from_ethernet_frame(frame);
        ethernet_frame_free(&frame);

        if (datagram != NULL && datagram->header->protocol == IPV4_PROTOCOL_UDP)
        {
            packet_counter_increase(counter, datagram);
        }

        ipv4_datagram_free(&datagram);
    }

    print_packet_linked_list_packets(counter->linked_list);
    printf("hash_table size %lu\n", counter->hash_table->size);

    wireshark_file_free(&ws_file);
    packet_counter_free(&counter);

    return 0;
}
