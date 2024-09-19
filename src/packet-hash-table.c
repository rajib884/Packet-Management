#include <stdlib.h>
#include <string.h>

#include "ipv4-packet.h"
#include "packet-hash-table.h"

#define KEY_LENGTH (IP_ADDRESS_LENGTH * 2)

#define FNV1A_INIT 0xcbf29ce484222325ULL
#define FNV1A_PRIME 0x100000001b3ULL

static uint64_t hash_func(const void *key, uint64_t true_hash_size)
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

static bool match_func(const ListNode_t *node, const void *key)
{
    if (node == NULL || key == NULL)
    {
        return false;
    }

    return memcmp(((HashNode_t *)node)->key, key, KEY_LENGTH) == 0;
}

static void free_hash_node(ListNode_t *node)
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

HashTable_t *packet_hash_table_create(uint64_t capacity)
{
    return hash_table_create(capacity, hash_func, match_func, free_hash_node);
}

void *key_from_ip(ipv4_datagram_t *datagram)
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