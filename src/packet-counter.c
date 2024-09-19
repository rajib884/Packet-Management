#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "ipv4-packet.h"
#include "packet-counter.h"

#define KEY_LENGTH (IP_ADDRESS_LENGTH * 2)

#define FNV1A_INIT 0xcbf29ce484222325ULL
#define FNV1A_PRIME 0x100000001b3ULL

#define HASH_TABLE_INITIAL_CAPACITY 100

static uint64_t hash_table_hash_func(const void *key, uint64_t true_hash_size)
{
    uint64_t hash = 0;
    uint64_t i = 0;
    const uint8_t *str = NULL;

    hash = FNV1A_INIT;
    str = (const uint8_t *)key;

    while (i < KEY_LENGTH)
    {
        hash = (hash ^ str[i]) * FNV1A_PRIME;
        // hash = ((hash << 5) + hash) + str[i]; /* hash * 33 + c */
        // hash = str[i] + (hash << 6) + (hash << 16) - hash;
        i++;
    }

    while (hash >= ((UINT64_MAX / true_hash_size) * true_hash_size))
    {
        hash = (hash * FNV1A_PRIME) + FNV1A_INIT;
    }
    hash %= true_hash_size;

    // hash ^= (hash >> 33);
    return hash;
}

static bool hash_table_match_func(const ListNode_t *node, const void *key)
{
    if (node == NULL || key == NULL)
    {
        return false;
    }

    return memcmp(((HashNode_t *)node)->key, key, KEY_LENGTH) == 0;
}

static void hash_table_free_node(ListNode_t *node)
{
    if (node == NULL)
    {
        return;
    }

    free((void *)((HashNode_t *)node)->key);
    ((HashNode_t *)node)->key = NULL;
    ((HashNode_t *)node)->data = NULL;
    node->next = NULL;

    free(node);
    node = NULL;

    return;
}

static void *key_from_ip(ipv4_datagram_t *datagram)
{
    uint8_t *key = NULL;

    if (datagram == NULL || datagram->header == NULL)
    {
        return NULL;
    }

    key = (uint8_t *)calloc(2, sizeof(ip_addr_t));

    if (key == NULL)
    {
        return NULL;
    }

    memcpy(key, &datagram->header->source_address, sizeof(ip_addr_t));
    memcpy(key + sizeof(ip_addr_t), &datagram->header->destination_address, sizeof(ip_addr_t));

    return (void *)key;
}

void print_packet_linked_list_packets(packet_node_t *head)
{
    packet_node_t *current = NULL;
    uint64_t i = 1;

    current = head;

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

packet_counter_t *packet_counter_create()
{
    packet_counter_t *counter = NULL;

    counter = (packet_counter_t *)calloc(1, sizeof(packet_counter_t));

    if (counter == NULL)
    {
        return NULL;
    }

    counter->hash_table = hash_table_create(HASH_TABLE_INITIAL_CAPACITY, hash_table_hash_func,
                                            hash_table_match_func, hash_table_free_node);
    counter->hash_key_from_ipv4 = key_from_ip;
    counter->list_node_free = (free_data_t)free;
    return counter;
}

void packet_counter_increase(packet_counter_t *counter, ipv4_datagram_t *datagram)
{
    void *key = NULL;
    packet_node_t *result = NULL;
    packet_node_t *new_node = NULL;

    if (counter == NULL || datagram == NULL || datagram->header == NULL)
    {
        return;
    }

    key = counter->hash_key_from_ipv4(datagram);
    result = (packet_node_t *)hash_table_get_item(counter->hash_table, key);

    if (result == NULL)
    {
        new_node = (packet_node_t *)calloc(1, sizeof(packet_node_t));

        if (new_node == NULL)
        {
            return;
        }

        memcpy(&new_node->dest, &datagram->header->destination_address, sizeof(ip_addr_t));
        memcpy(&new_node->src, &datagram->header->source_address, sizeof(ip_addr_t));
        new_node->ref_counter = 1;

        linked_list_insert_at_head((ListNode_t **)&counter->linked_list, (ListNode_t *)new_node);
        hash_table_add_item(counter->hash_table, key, (void *)new_node);

        new_node = NULL;
    }
    else
    {
        result->ref_counter++;
        free(key);
        key = NULL;
    }
}

void packet_counter_free(packet_counter_t **counter_p)
{
    hash_table_free(&(*counter_p)->hash_table);
    linked_list_delete_list((ListNode_t **)&(*counter_p)->linked_list,
                            (*counter_p)->list_node_free);
}