#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "ethernet-frame.h"

ethernet_frame_t *ethernet_frame_from_dynamic_buffer(dynamic_buffer_t *buffer)
{
    ethernet_header_t *header = NULL;
    ethernet_frame_t *frame = NULL;
    size_t data_len = 0;

    if (buffer == NULL)
    {
        return NULL;
    }

    if (buffer->size < sizeof(ethernet_header_t))
    {
        fprintf(stderr, "Buffer size is less than Ethernet header length.\n");
        goto cleanup;
    }

    header = (ethernet_header_t *)calloc(1, sizeof(ethernet_frame_t));

    if (header == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for Ethernet header.\n");
        goto cleanup;
    }

    memcpy(header, buffer->data, sizeof(ethernet_header_t));
    header->ethertype = ntohs(header->ethertype);

    data_len = buffer->size - sizeof(ethernet_header_t);
    frame = (ethernet_frame_t *)calloc(1, sizeof(ethernet_frame_t) + data_len);

    if (frame == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for Ethernet frame.\n");
        goto cleanup;
    }

    memcpy(frame->data, buffer->data + sizeof(ethernet_header_t), data_len);
    frame->header = header;
    frame->data_len = data_len;

    return frame;

cleanup:
    free(header);
    header = NULL;

    free(frame);
    frame = NULL;

    return NULL;
}

void ethernet_frame_free(ethernet_frame_t **frame_p)
{
    if (frame_p == NULL || *frame_p == NULL)
    {
        return;
    }

    free((*frame_p)->header);
    (*frame_p)->header = NULL;

    free(*frame_p);
    *frame_p = NULL;

    return;
}

void print_mac(mac_address_t *mac)
{
    uint32_t i = 0;

    if (mac == NULL)
    {
        printf("Invalid MAC");

        return;
    }

    for (i = 0; i < MAC_LENGTH; i++)
    {
        if (i != 0)
        {
            printf(":");
        }

        printf("%02X", mac->bytes[i]);
    }

    return;
}

void print_ethernet(ethernet_frame_t *frame, bool print_data)
{
    printf("  Ethernet Packet:\n");

    if (frame == NULL || frame->header == NULL)
    {
        printf("    NULL\n");

        return;
    }

    printf("    Destination MAC: ");
    print_mac(&frame->header->destination);
    printf("\n");
    printf("    Source MAC: ");
    print_mac(&frame->header->source);
    printf("\n");
    printf("    Ethertype: 0x%04X (%s)\n", frame->header->ethertype,
           (frame->header->ethertype == ETHERTYPE_IPV4) ? "IPv4" : "Non-IPv4");

    if (print_data)
    {
        printf("    Data:");
        print_data_f(frame->data, frame->data_len);
    }

    return;
}