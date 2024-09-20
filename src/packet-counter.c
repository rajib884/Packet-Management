#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "ipv4-packet.h"
#include "packet-counter.h"

#define KEY_LENGTH (IP_ADDRESS_LENGTH * 2)

#define FNV1A_INIT 0xcbf29ce484222325ULL
#define FNV1A_PRIME 0x100000001b3ULL

#define HASH_TABLE_INITIAL_CAPACITY 10
#define SPACE_FOR_IP 15
#define SPACE_FOR_COUNT 7
#define SPACE_FOR_INDEX 5

#ifdef USE_UNICODE
    #define PIPE "│"
#else
    #define PIPE "|"
#endif

#define MAX(a, b) (((a) < (b)) ? (b) : (a))

static uint64_t hash_table_hash_func(const void *key, uint64_t true_hash_size) /* OK */
{
    uint64_t i = 0;
    uint64_t hash = 0;
    const uint8_t *str = NULL;

    hash = FNV1A_INIT;
    str = (const uint8_t *)key;

    while (i < KEY_LENGTH)
    {
        hash = (hash ^ str[i++]) * FNV1A_PRIME;
    }

    while (hash >= ((UINT64_MAX / true_hash_size) * true_hash_size))
    {
        hash = (hash * FNV1A_PRIME) + FNV1A_INIT;
    }

    hash %= true_hash_size;

    return hash;
}

static bool hash_table_match_func(const ListNode_t *node, const void *key) /* OK */
{
    if (node == NULL || key == NULL)
    {
        return false;
    }

    return memcmp(((HashNode_t *)node)->key, key, KEY_LENGTH) == 0;
}

static void hash_table_free_node(ListNode_t *node) /* OK */
{
    if (node == NULL)
    {
        return;
    }

    free((void *)((HashNode_t *)node)->key); /* free key */
    ((HashNode_t *)node)->key = NULL;
    ((HashNode_t *)node)->data = NULL; /* No need to free data */
    node->next = NULL;

    free(node);
    node = NULL;

    return;
}

static void *key_from_ip(ipv4_datagram_t *datagram) /* OK */
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

void print_packet_counter_hash_table(packet_counter_t *counter) /* OK */
{
    uint64_t i = 0;
    int char_printed = 0;
    HashTable_t *hash_table = NULL;
    HashNode_t *current = NULL;

    if (counter == NULL || counter->hash_table == NULL)
    {
        printf("No data.\n");

        return;
    }

    hash_table = counter->hash_table;

#ifdef USE_UNICODE
    printf("┌─────────────────────────────────────────────────────┐\n");
    printf("│                     Hash Table                      │\n");
    printf("├───────┬───────────────────────────────────┬─────────┤\n");
    printf("│       │                Key                │         │\n");
    printf("│ Index ├─────────────────┬─────────────────┤  Count  │\n");
    printf("│       │    Source IP    │  Destination IP │         │\n");
    printf("├───────┼─────────────────┼─────────────────┼─────────┤\n");
#else
    printf("+-----------------------------------------------------+\n");
    printf("|                     Hash Table                      |\n");
    printf("+-------+-----------------------------------+---------+\n");
    printf("|       |                Key                |         +\n");
    printf("| Index +-----------------+-----------------+  Count  +\n");
    printf("|       |    Source IP    |  Destination IP |         +\n");
    printf("+-------+-----------------+-----------------+---------+\n");
#endif

    for (i = 0; i < hash_table->capacity; i++)
    {
        current = hash_table->table[i];

        if (current == NULL)
        {
            printf(PIPE " %*" PRIu64 " " PIPE " %*s " PIPE " %*s " PIPE " %*s " PIPE "\n",
                   SPACE_FOR_INDEX, i, SPACE_FOR_IP, "", SPACE_FOR_IP, "", SPACE_FOR_COUNT, "");
        }

        while (current != NULL)
        {
            if (current == hash_table->table[i])
            {
                printf(PIPE " %*" PRIu64 " " PIPE " ", SPACE_FOR_INDEX, i);
            }
            else
            {
                printf(PIPE "       " PIPE " ");
            }

            char_printed = print_ip_addr((ip_addr_t *)current->key);
            printf("%*s " PIPE " ", MAX(SPACE_FOR_IP - char_printed, 0), "");
            char_printed = print_ip_addr((ip_addr_t *)(current->key + sizeof(ip_addr_t)));
            printf("%*s " PIPE " ", MAX(SPACE_FOR_IP - char_printed, 0), "");
            printf("%*" PRIu64 " " PIPE "\n", SPACE_FOR_COUNT,
                   ((packet_node_t *)(current->data))->ref_counter);
            current = (HashNode_t *)current->node.next;
        }

#ifdef USE_UNICODE
        if (i == hash_table->capacity - 1)
        {
            printf("└───────┴─────────────────┴─────────────────┴─────────┘\n");
        }
        else
        {
            printf("├───────┼─────────────────┼─────────────────┼─────────┤\n");
        }
#else
        printf("+-------+-----------------+-----------------+---------+\n");
#endif
    }

    return;
}

void print_packet_counter_linked_list(packet_counter_t *counter) /* OK */
{
    int char_printed = 0;
    uint64_t i = 0;
    uint64_t total = 0;
    packet_node_t *current = NULL;

    if (counter == NULL)
    {
        printf("No data.\n");

        return;
    }

    current = counter->linked_list;

#ifdef USE_UNICODE
    printf("┌─────────────────────────────────────────────────────┐\n");
    printf("│                     Linked List                     │\n");
    printf("├───────┬─────────────────┬─────────────────┬─────────┤\n");
    printf("│   #   │    Source IP    │  Destination IP │  Count  │\n");
    printf("├───────┼─────────────────┼─────────────────┼─────────┤\n");
#else
    printf("+-----------------------------------------------------+\n");
    printf("+                     Linked List                     +\n");
    printf("+-------+-----------------+-----------------+---------+\n");
    printf("|   #   |    Source IP    |  Destination IP |  Count  |\n");
    printf("+-------+-----------------+-----------------+---------+\n");
#endif

    while (current != NULL)
    {
        printf(PIPE " %*" PRIu64 " " PIPE " ", SPACE_FOR_INDEX, ++i);
        char_printed = print_ip_addr(&current->src);
        printf("%*s " PIPE " ", MAX(SPACE_FOR_IP - char_printed, 0), "");
        char_printed = print_ip_addr(&current->dest);
        printf("%*s " PIPE " ", MAX(SPACE_FOR_IP - char_printed, 0), "");
        printf("%*" PRIu64 " " PIPE "\n", SPACE_FOR_COUNT, current->ref_counter);

        total += current->ref_counter;

        current = (packet_node_t *)current->node.next;
    }

#ifdef USE_UNICODE
    printf("├───────┴─────────────────┴─────────────────┼─────────┤\n");
    printf("│                                     Total │ %*" PRIu64 " │\n", SPACE_FOR_COUNT,
           total);
    printf("└───────────────────────────────────────────┴─────────┘\n");
#else
    printf("+-------+-----------------+-----------------+---------+\n");
    printf("|                                     Total | %*" PRIu64 " |\n", SPACE_FOR_COUNT,
           total);
    printf("+-------------------------------------------+---------+\n");
#endif

    return;
}

packet_counter_t *packet_counter_create() /* OK */
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

void packet_counter_increase(packet_counter_t *counter, ipv4_datagram_t *datagram) /* OK */
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

    return;
}

void packet_counter_free(packet_counter_t **counter_p)
{
    if (counter_p == NULL || *counter_p == NULL)
    {
        return;
    }

    hash_table_free(&(*counter_p)->hash_table);
    linked_list_delete_list((ListNode_t **)&(*counter_p)->linked_list,
                            (*counter_p)->list_node_free);
    free(*counter_p);
    *counter_p = NULL;

    return;
}