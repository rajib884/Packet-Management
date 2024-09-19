#include <stdio.h>
#include <stdlib.h>

#include "hash-table.h"

static HashNode_t *hash_table_create_node(const void *key, const void *data);
static bool hash_table_rehash(HashTable_t *hash_table, uint64_t new_capacity);

HashTable_t *hash_table_create(uint64_t capacity, hash_func_t hash_func, match_func_t match_func,
                               free_data_t free_data)
{
    HashTable_t *hash_table = NULL;
    HashNode_t **table = NULL;

    if (capacity == 0 || hash_func == NULL || match_func == NULL)
    {
        fprintf(stderr, "Invalid parameter for creating hash table\n");

        return NULL;
    }

    hash_table = (HashTable_t *)calloc(1, sizeof(HashTable_t));

    if (hash_table == NULL)
    {
        fprintf(stderr, "Unable to allocate memory for Hash Table.\n");

        return NULL;
    }

    table = (HashNode_t **)calloc(capacity, sizeof(HashNode_t *));

    if (table == NULL)
    {
        free(hash_table);
        hash_table = NULL;
        fprintf(stderr, "Unable to allocate memory for Hash Table table.\n");

        return NULL;
    }

    hash_table->capacity = capacity;
    hash_table->hash_func = hash_func;
    hash_table->match_func = match_func;
    hash_table->free_data = free_data;
    hash_table->table = table;

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

const void *hash_table_get_item(HashTable_t *hash_table, const void *key)
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

bool hash_table_add_item(HashTable_t *hash_table, const void *key, const void *data)
{
    uint64_t hash_index = 0;
    ListNode_t *result = NULL;
    HashNode_t *new_node = NULL;
    ListNode_t **head_p = NULL;

    if (hash_table == NULL || key == NULL || data == NULL)
    {
        return false;
    }

    if (hash_table->size > hash_table->capacity)
    {
        hash_table_rehash(hash_table, hash_table->capacity * 2);
    }

    hash_index = hash_table->hash_func(key, hash_table->capacity);
    head_p = (ListNode_t **)&hash_table->table[hash_index];
    result = linked_list_search(head_p, key, hash_table->match_func);

    if (result != NULL)
    {
        /* key already exists, overwrite */
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

    return true;
}

static bool hash_table_rehash(HashTable_t *hash_table, uint64_t new_capacity)
{
    uint64_t i = 0;
    uint64_t hash_index = 0;
    HashNode_t **new_table = NULL;
    ListNode_t **head_p = NULL;
    HashNode_t *node = NULL;

    if (new_capacity == 0)
    {
        return false;
    }

    new_table = (HashNode_t **)calloc(new_capacity, sizeof(HashNode_t *));

    if (new_table == NULL)
    {
        return false;
    }

    for (i = 0; i < hash_table->capacity; i++)
    {
        while (hash_table->table[i] != NULL)
        {
            node = hash_table->table[i];
            head_p = (ListNode_t **)&hash_table->table[i];
            linked_list_delete_node(head_p, *head_p, NULL);

            hash_index = hash_table->hash_func(node->key, new_capacity);
            head_p = (ListNode_t **)&new_table[hash_index];
            linked_list_insert_at_head(head_p, (ListNode_t *)node);
        }
    }

    free(hash_table->table);
    hash_table->table = new_table;
    hash_table->capacity = new_capacity;

    return true;
}

bool hash_table_remove_item(HashTable_t *hash_table, const void *key)
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
        return false;
    }

    linked_list_delete_node(head_p, node, hash_table->free_data);
    hash_table->size--;

    return true;
}

void hash_table_free(HashTable_t **hash_table_p)
{
    uint64_t i = 0;
    ListNode_t **head_p = NULL;
    HashTable_t *hash_table = NULL;

    if (hash_table_p == NULL || *hash_table_p == NULL)
    {
        return;
    }

    hash_table = *hash_table_p;

    for (i = 0; i < hash_table->capacity; i++)
    {
        head_p = (ListNode_t **)&hash_table->table[i];
        hash_table->size -= linked_list_delete_list(head_p, hash_table->free_data);
    }

    free(hash_table->table);
    hash_table->table = NULL;

    hash_table = NULL;

    free(*hash_table_p);
    *hash_table_p = NULL;

    return;
}