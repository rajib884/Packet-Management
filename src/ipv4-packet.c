// OK AFAIK

#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ipv4-packet.h"

ipv4_datagram_t *ipv4_datagram_from_ethernet_frame(ethernet_frame_t *frame)
{
    ipv4_header_t *header = NULL;
    uint8_t *options = NULL;
    ipv4_datagram_t *datagram = NULL;
    size_t header_len = 0;
    size_t data_len = 0;
    size_t options_len = 0;

    if (frame == NULL)
    {
        return NULL;
    }

    if (frame->header->ethertype != ETHERTYPE_IPV4)
    {
        // fprintf(stderr, "Ethernet data is not IPv4 type.\n");
        goto cleanup;
    }

    if (frame->data_len < sizeof(ipv4_header_t))
    {
        fprintf(stderr, "Ethernet data is smaller than IPv4 header size.\n");
        goto cleanup;
    }

    header = (ipv4_header_t *)calloc(1, sizeof(ipv4_header_t));

    if (header == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for IPv4 header.\n");
        goto cleanup;
    }

    memcpy(header, frame->data, sizeof(ipv4_header_t));

    header->total_length = ntohs(header->total_length);
    header->identification = ntohs(header->identification);
    header->fragment_offset_flag.raw_data = ntohs(header->fragment_offset_flag.raw_data);
    header->checksum = ntohs(header->checksum);

    header_len = header->header_length * sizeof(uint32_t); /* length in 32bit words */

    if (header_len < sizeof(ipv4_header_t) || header_len > frame->data_len)
    {
        fprintf(stderr, "IPv4 header contains invalid header length attribute.\n");
        goto cleanup;
    }

    options_len = header_len - sizeof(ipv4_header_t);

    if (options_len != 0)
    {
        options = (uint8_t *)calloc(options_len, sizeof(uint8_t));

        if (options == NULL)
        {
            fprintf(stderr, "Unable to allocate memory for IPv4 options.\n");
            goto cleanup;
        }

        memcpy(options, frame->data + sizeof(ipv4_header_t), options_len * sizeof(uint8_t));
    }

    data_len = header->total_length - header_len;

    if (header->total_length != frame->data_len)
    {
        if (frame->data_len != ETHERNET_MINIMUM_DATA_LEN ||
            header->total_length > ETHERNET_MINIMUM_DATA_LEN)
        {
            fprintf(stderr, "IPv4 header contains invalid total length attribute.\n");
            goto cleanup;
        }
    }

    datagram = (ipv4_datagram_t *)calloc(1, sizeof(ipv4_datagram_t) + data_len);

    if (datagram == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for IPv4 datagram.\n");
        goto cleanup;
    }

    memcpy(datagram->data, frame->data + header_len, data_len);
    datagram->data_len = data_len;
    datagram->options = options;
    datagram->option_len = options_len;
    datagram->header = header;

    return datagram;

cleanup:
    free(header);
    header = NULL;

    free(options);
    options = NULL;

    free(datagram);
    datagram = NULL;

    return NULL;
}

void ipv4_datagram_free(ipv4_datagram_t **datagram_p)
{
    if (datagram_p == NULL || *datagram_p == NULL)
    {
        return;
    }

    free((*datagram_p)->header);
    (*datagram_p)->header = NULL;

    free((*datagram_p)->options);
    (*datagram_p)->options = NULL;

    free((*datagram_p));
    (*datagram_p) = NULL;

    return;
}

void print_ip_addr(ip_addr_t *ip)
{
    uint32_t i = 0;

    if (ip == NULL)
    {
        printf("No value.");

        return;
    }

    for (i = 0; i < IP_ADDRESS_LENGTH; i++)
    {
        if (i != 0)
        {
            printf(".");
        }

        printf("%d", ip->byte[i]);
    }
}

void print_ipv4(ipv4_datagram_t *datagram, bool print_data)
{
    uint32_t i = 0;

    printf("IPv4 Datagram:\n");

    if (datagram == NULL)
    {
        printf("  NULL\n");

        return;
    }

    printf("  version: %02x\n", datagram->header->version);
    printf("  IHL: %02x\n", datagram->header->header_length);
    printf("  type_of_service: %02x\n", datagram->header->type_of_service);
    printf("  total_length: %04x\n", datagram->header->total_length);
    printf("  identification: %04x\n", datagram->header->identification);
    printf("  flags: %03b\n", datagram->header->fragment_offset_flag.fields.flags);
    printf("  fragment_offset: %013b\n",
           datagram->header->fragment_offset_flag.fields.fragment_offset);
    printf("  time_to_live: %02x\n", datagram->header->time_to_live);
    printf("  protocol: %02x\n", datagram->header->protocol);
    printf("  checksum: %04x\n", datagram->header->checksum);
    printf("  Source IP: ");
    print_ip_addr(&datagram->header->source_address);
    printf("\n");
    printf("  Destination IP: ");
    print_ip_addr(&datagram->header->destination_address);
    printf("\n");

    if (print_data)
    {
        i = 0;
        printf(" IPv4 Data: ");

        while (i < datagram->data_len)
        {
            printf("%02x ", datagram->data[i++]);
        }
    }

    printf("\n");

    return;
}