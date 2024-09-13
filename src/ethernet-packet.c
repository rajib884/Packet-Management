#include <arpa/inet.h> // For ntohs and ntohl
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "ethernet-packet.h"

ethernet_frame_t *buffer_to_ethernet_frame(dynamic_buffer_t *buffer)
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

void free_ethernet_frame(ethernet_frame_t **frame_p)
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
}