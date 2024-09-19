#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "packet-counter.h"
#include "wireshark-to-buffer.h"

packet_counter_t *counter = NULL;

int main(int argc, char *argv[])
{
    wireshark_file_t *ws_file = NULL;
    dynamic_buffer_t *buf = NULL;
    ethernet_frame_t *frame = NULL;
    ipv4_datagram_t *datagram = NULL;
    const char *ws_file_path = NULL;

    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    ws_file_path = argv[1];

    if (access(ws_file_path, F_OK) != 0)
    {
        perror("File does not exist");
        return EXIT_FAILURE;
    }

    counter = packet_counter_create();
    ws_file = wireshark_file_create(ws_file_path);

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

    print_packet_counter_hash_table(counter);
    print_packet_counter_linked_list(counter);

    wireshark_file_free(&ws_file);
    packet_counter_free(&counter);

    return 0;
}
