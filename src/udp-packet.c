#include <arpa/inet.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "udp-packet.h"

udp_packet_t *ipv4_datagram_to_udp_packet(ipv4_datagram_t *datagram)
{
    udp_packet_t *packet = NULL;
    udp_header_t *header = NULL;
    size_t data_len = 0;
    bool more_fragments = false;
    uint16_t flags = 0;
    uint16_t fragment_offset = 0;

    if (datagram == NULL)
    {
        return NULL;
    }

    if (datagram->header->protocol != IPV4_PROTOCOL_UDP)
    {
        fprintf(stderr, "IPv4 data is not UDP type.\n");
        goto cleanup;
    }

    if (datagram->data_len < sizeof(udp_header_t))
    {
        fprintf(stderr, "IPv4 data is smaller than UDP header size.\\n");
        goto cleanup;
    }

    header = (udp_header_t *)calloc(1, sizeof(udp_header_t));

    if (header == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for UDP header.\n");
        goto cleanup;
    }

    memcpy(header, datagram->data, sizeof(udp_header_t));

    header->source_port = ntohs(header->source_port);
    header->dest_port = ntohs(header->dest_port);
    header->length = ntohs(header->length);
    header->checksum = ntohs(header->checksum);

    if (header->length != datagram->data_len)
    {
        flags = (uint16_t)datagram->header->fragment_offset_flag.fields.flags;
        fragment_offset = (uint16_t)datagram->header->fragment_offset_flag.fields.fragment_offset;
        more_fragments = flags & IPV4_MASK_FLAG_MORE_FRAGMENTS;

        if (header->length < datagram->data_len ||
            (more_fragments == false && fragment_offset == 0))
        {
            fprintf(stderr, "UDP header contains invalid header length attribute.\n");
            goto cleanup;
        }
    }

    data_len = datagram->data_len - sizeof(udp_header_t);
    packet = (udp_packet_t *)calloc(1, sizeof(udp_packet_t) + data_len);

    if (packet == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for UDP packet.\n");
        goto cleanup;
    }

    packet->header = header;
    packet->data_len = data_len;
    memcpy(packet->data, datagram->data + sizeof(udp_header_t), data_len);

    return packet;

cleanup:
    free(header);
    header = NULL;

    free(packet);
    packet = NULL;

    return NULL;
}

void free_udp_packet(udp_packet_t **packet_p)
{
    if (packet_p == NULL || *packet_p == NULL)
    {
        return;
    }

    free((*packet_p)->header);
    (*packet_p)->header = NULL;

    free((*packet_p));
    (*packet_p) = NULL;

    return;
}

void print_udp(udp_packet_t *packet, bool print_data)
{
    uint32_t i = 0;

    printf("UDP Packet:\n");

    if (packet == NULL)
    {
        printf("NULL\n");

        return;
    }

    printf("  Source port: %04x\n", packet->header->source_port);
    printf("  Destination port: %04x\n", packet->header->dest_port);
    printf("  Length: %04x\n", packet->header->length);
    printf("  Checksum: %04x\n", packet->header->checksum);

    if (print_data)
    {
        i = 0;
        printf("  UDP Data: ");
        while (i < packet->data_len)
        {
            printf("%02x ", packet->data[i++]);
        }
    }

    printf("\n");

    return;
}