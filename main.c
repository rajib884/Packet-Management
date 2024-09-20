#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "debug.h"
#include "packet-counter.h"
#include "udp-packet.h"
#include "wireshark-to-buffer.h"

packet_counter_t *counter = NULL;

int main(int argc, char *argv[])
{
    const char *ws_file_path = NULL;
    wireshark_file_t *ws_file = NULL;
    dynamic_buffer_t *buf = NULL;
    ethernet_frame_t *frame = NULL;
    ipv4_datagram_t *datagram = NULL;
    udp_packet_t *packet = NULL;
    uint64_t packet_total = 0;
    uint64_t packet_valid = 0;

    /* struct timeval stop, start; */

    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <file_path>\n", argv[0]);

        return EXIT_FAILURE;
    }

    ws_file_path = argv[1];

    if (access(ws_file_path, F_OK) != 0)
    {
        perror("File does not exist\n");

        return EXIT_FAILURE;
    }

    counter = packet_counter_create(); /* uses linked list and hash table to count packets */
    ws_file = wireshark_file_create(ws_file_path); /* wireshark file object to get packets*/

    /* gettimeofday(&start, NULL); */

    while (wireshark_file_readable(ws_file) && packet_valid < 10)
    {
        packet_total++;
        printf("Packet %" PRIu64 "\n", packet_total);

        buf = wireshark_file_get_next_packet(ws_file);
        frame = ethernet_frame_from_dynamic_buffer(buf);
        debug_call(print_ethernet, frame, false);
        dynamic_buffer_free(&buf);
        datagram = ipv4_datagram_from_ethernet_frame(frame);
        debug_call(print_ipv4, datagram, false);
        ethernet_frame_free(&frame);

        if (datagram != NULL && datagram->header->protocol == IPV4_PROTOCOL_UDP)
        {
            packet_valid++;
            packet_counter_increase(counter, datagram);
#ifdef DEBUG
            packet = udp_packet_from_ipv4_datagram(datagram);
            print_udp(packet, true);
            udp_packet_free(&packet);
#endif
        }
        else
        {
            printf("  Packet ignored\n");
        }

        printf("\n");

        ipv4_datagram_free(&datagram);
    }

    /*
    gettimeofday(&stop, NULL);
    printf("took %lu us\n", (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec);
    */

    print_packet_counter_hash_table(counter);
    print_packet_counter_linked_list(counter);

    wireshark_file_free(&ws_file);
    packet_counter_free(&counter);

    printf("There was total %" PRIu64 " packets in file %s\n", packet_total, ws_file_path);
    printf("Out of which %" PRIu64 " packets were valid IPv4 UDP packet.\n", packet_valid);

    return 0;
}
