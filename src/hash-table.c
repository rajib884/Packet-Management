#include <stdio.h>
#include <stdlib.h>

#include "hash-table.h"

static HashNode_t *hash_table_create_node(const void *key, const void *data);

HashTable_t *hash_table_create(uint64_t capacity, hash_func_t hash_func, match_func_t match_func,
                               free_data_t free_data)
{
    HashTable_t *hash_table = NULL;

    if (capacity == 0 || hash_func == NULL || match_func == NULL)
    {
        return NULL;
    }

    hash_table = (HashTable_t *)calloc(1, sizeof(HashTable_t) + capacity * (sizeof(HashNode_t *)));

    if (hash_table == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for Hash Table.\n");
        return NULL;
    }

    hash_table->capacity = capacity;
    hash_table->hash_func = hash_func;
    hash_table->match_func = match_func;
    hash_table->free_data = free_data;

    return hash_table;
}

static HashNode_t *hash_table_create_node(const void *key, const void *data)
{
    HashNode_t *node = NULL;

    if (key == NULL || data == NULL)
    {
        return NULL;
    }

    node = (HashNode_t *)calloc(1, sizeof(HashNode_t));

    if (node == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for Hash Node.\n");
        return NULL;
    }

    node->key = key;
    node->data = data;

    return node;
}

const void *hash_table_search(HashTable_t *hash_table, const void *key)
{
    uint64_t hash_index = 0;
    ListNode_t *result = NULL;
    ListNode_t **head_p = NULL;

    if (hash_table == NULL || key == NULL)
    {
        return NULL;
    }

    hash_index = hash_table->hash_func(key, hash_table->capacity);
    head_p = (ListNode_t **)&hash_table->table[hash_index];
    result = linked_list_search(head_p, key, hash_table->match_func);

    if (result != NULL)
    {
        return ((HashNode_t *)result)->data;
    }

    return NULL;
}

bool hash_table_insert_node(HashTable_t *hash_table, const void *key, const void *data)
{
    uint64_t hash_index = 0;
    ListNode_t *result = NULL;
    HashNode_t *new_node = NULL;
    ListNode_t **head_p = NULL;

    if (hash_table == NULL || key == NULL || data == NULL)
    {
        return false;
    }

    hash_index = hash_table->hash_func(key, hash_table->capacity);
    head_p = (ListNode_t **)&hash_table->table[hash_index];
    result = linked_list_search(head_p, key, hash_table->match_func);

    if (result != NULL)
    {
        /* key already exists */
        ((HashNode_t *)result)->data = data;
        return true;
    }

    new_node = hash_table_create_node(key, data);
    if (new_node == NULL)
    {
        return false;
    }

    linked_list_insert_at_head(head_p, (ListNode_t *)new_node);
    hash_table->size++;

    // rehash??

    return true;
}

bool hash_table_delete_node(HashTable_t *hash_table, const void *key)
{
    uint64_t hash_index = 0;
    ListNode_t *node = NULL;
    ListNode_t **head_p = NULL;

    if (hash_table == NULL || key == NULL)
    {
        return false;
    }

    hash_index = hash_table->hash_func(key, hash_table->capacity);
    head_p = (ListNode_t **)&hash_table->table[hash_index];
    node = linked_list_search(head_p, key, hash_table->match_func);

    if (node == NULL)
    {
        return false; // Node not found
    }

    linked_list_delete_node(head_p, node, hash_table->free_data);
    hash_table->size--;

    return true;
}

void hash_table_delete_table(HashTable_t **hash_table_p)
{
    uint64_t i = 0;
    HashTable_t *hash_table = NULL;
    HashNode_t *hash_node = NULL;

    if (hash_table_p == NULL || *hash_table_p == NULL)
    {
        return;
    }

    hash_table = *hash_table_p;

    for (i = 0; i < hash_table->capacity; i++)
    {
        hash_node = hash_table->table[i];
        while (hash_node != NULL)
        {
            linked_list_delete_node((ListNode_t **)&hash_node, (ListNode_t *)hash_node,
                                    hash_table->free_data);
            hash_table->size--;
        }
    }

    free(*hash_table_p);
    *hash_table_p = NULL;
    hash_table = NULL;

    return;
}