#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include "singly-linked-list.h"

typedef uint64_t (*hash_func_t)(const void *key, uint64_t true_hash_size);

typedef struct HashNode
{
    ListNode_t node;
    const void *key;
    const void *data;
} HashNode_t;

typedef struct HashTable
{
    uint64_t capacity;
    uint64_t size;
    hash_func_t hash_func;
    match_func_t match_func;
    free_data_t free_data;
    HashNode_t *table[0];
} HashTable_t;

HashTable_t *hash_table_create(uint64_t, hash_func_t, match_func_t, free_data_t);
const void *hash_table_get_item(HashTable_t *hash_table, const void *key);
bool hash_table_add_item(HashTable_t *hash_table, const void *key, const void *data);
bool hash_table_remove_item(HashTable_t *hash_table, const void *key);
void hash_table_free(HashTable_t **hash_table_p);

#endif /* __HASH_TABLE_H__*/