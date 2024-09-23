#include <stdio.h>
#include <stdlib.h>

#include "hash-table.h"

static HashNode_t *hash_table_create_node(const void *key, const void *data);
static bool hash_table_rehash(HashTable_t *hash_table, uint64_t new_capacity);
static bool is_prime(uint64_t n);
static uint64_t next_prime(uint64_t n);

/*****************************************************************************
 *
 *   Name:       hash_table_create
 *
 *   Input:      capacity     Number of items the hash table can hold, can be changed later
 *               hash_func    A function used for hashing keys
 *               match_func   A function used for matching keys
 *               free_node    A function used for freeing HashNode_t
 *
 *   Return:     Success      A pointer to the newly created HashTable_t
 *               Failed       NULL
 *
 *   Description:            Initializes a new hash table with the specified capacity
 *                           and function pointers. Returns NULL if any parameters
 *                           are invalid or if memory allocation fails.
 ******************************************************************************/
HashTable_t *hash_table_create(uint64_t capacity, hash_func_t hash_func, match_func_t match_func,
                               free_data_t free_node)
{
    HashTable_t *hash_table = NULL;
    HashNode_t **table = NULL;

    if (capacity == 0 || hash_func == NULL || match_func == NULL || free_node == NULL)
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
    hash_table->free_node = free_node;
    hash_table->table = table;

    return hash_table;
}

/*****************************************************************************
 *
 *   Name:       hash_table_create_node
 *
 *   Input:      key         Key associated with the new node
 *               data        Data associated with the new node
 *
 *   Return:     Success      A pointer to the newly created HashNode_t
 *               Failed       NULL
 *
 *   Description:            Creates a new hash node with the given key and data.
 ******************************************************************************/
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

/*****************************************************************************
 *
 *   Name:       hash_table_get_item
 *
 *   Input:      hash_table   Hash table from which to retrieve an item
 *               key          Key of the item to retrieve
 *
 *   Return:     Success      Data associated with the key
 *               Failed       NULL
 *
 *   Description:            Retrieves the data associated with the specified key
 *                           from the hash table. Returns NULL if the hash table
 *                           is NULL, the key is NULL, or if the key does not exist
 *                           in the table.
 ******************************************************************************/
const void *hash_table_get_item(HashTable_t *hash_table, const void *key)
{
    uint64_t hash_index = 0;
    ListNode_t *result = NULL;
    ListNode_t **head_p = NULL;

    if (hash_table == NULL || key == NULL || hash_table->table == NULL)
    {
        return NULL;
    }

    hash_index = hash_table->hash_func(key, hash_table->capacity);
    hash_index = hash_index % hash_table->capacity; /* Just to be safe */
    head_p = (ListNode_t **)&hash_table->table[hash_index];
    result = linked_list_search(head_p, key, hash_table->match_func);

    if (result != NULL)
    {
        return ((HashNode_t *)result)->data;
    }

    return NULL;
}

static bool is_prime(uint64_t n)
{
    uint64_t i = 0;

    if (n <= 1)
    {
        return false;
    }

    if (n <= 3)
    {
        return true;
    }

    if (n % 2 == 0 || n % 3 == 0)
    {
        return false;
    }

    /**
     * While finding factors of a number it is enough to iterate from 1 to sqrt(N) to find all the
     * factors of N.
     * */
    for (i = 5; i * i <= n; i += 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
        {
            return false;
        }
    }

    return true;
}

static uint64_t next_prime(uint64_t n)
{
    if (n <= 2)
    {
        return 2;
    }

    if (n % 2 == 0)
    {
        n++;
    }

    while (!is_prime(n))
    {
        n += 2;
    }

    return n;
}

/*****************************************************************************
 *
 *   Name:       hash_table_add_item
 *
 *   Input:      hash_table   Hash table where the item will be added
 *               key          Key associated with the item
 *               data         Data to be stored
 *
 *   Return:     Success      true if the item was added or updated successfully
 *               Failed       false if the operation failed
 *
 *   Description:            Adds a new item to the hash table or updates the data
 *                           if the key already exists. Rehashes the table if the
 *                           number of items exceeds the current capacity.
 ******************************************************************************/
bool hash_table_add_item(HashTable_t *hash_table, const void *key, const void *data)
{
    uint64_t hash_index = 0;
    uint64_t new_capacity = 0;
    ListNode_t *result = NULL;
    HashNode_t *new_node = NULL;
    ListNode_t **head_p = NULL;

    if (hash_table == NULL || key == NULL || data == NULL || hash_table->table == NULL)
    {
        return false;
    }

    if (hash_table->size > hash_table->capacity / 2)
    {
        new_capacity = next_prime(hash_table->capacity * 2);

        if (hash_table->capacity > new_capacity)
        {
            /* Overflowed? */
            new_capacity = UINT64_MAX;
        }

        hash_table_rehash(hash_table, new_capacity);
    }

    hash_index = hash_table->hash_func(key, hash_table->capacity);
    hash_index = hash_index % hash_table->capacity; /* Just to be safe */
    head_p = (ListNode_t **)&hash_table->table[hash_index];
    result = linked_list_search(head_p, key, hash_table->match_func);

    if (result != NULL)
    {
        ((HashNode_t *)result)->data = data; /* key already exists, overwrite */

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

    if (hash_table == NULL || new_capacity == 0 || hash_table->table == NULL)
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
            node = hash_table->table[i];                    /* Save head of old list to node */
            head_p = (ListNode_t **)&hash_table->table[i];  /* Address of head of old list */
            linked_list_delete_node(head_p, *head_p, NULL); /* Move head to next node */

            hash_index = hash_table->hash_func(node->key, new_capacity); /* Get new index */
            hash_index = hash_index % new_capacity;                      /* Just to be safe */
            head_p = (ListNode_t **)&new_table[hash_index];              /* Address of new list */
            linked_list_insert_at_head(head_p, (ListNode_t *)node);      /* Add node to new list */
        }
    }

    free(hash_table->table);
    hash_table->table = NULL;
    hash_table->table = new_table;
    hash_table->capacity = new_capacity;

    return true;
}

/*****************************************************************************
 *
 *   Name:       hash_table_remove_item
 *
 *   Input:      hash_table   Hash table from which to remove an item
 *               key          Key of the item to be removed
 *
 *   Return:     Success      true if the item was removed successfully
 *               Failed       false if the operation failed
 *
 *   Description:            Removes the item associated with the specified key
 *                           from the hash table.
 ******************************************************************************/
bool hash_table_remove_item(HashTable_t *hash_table, const void *key)
{
    uint64_t hash_index = 0;
    ListNode_t *node = NULL;
    ListNode_t **head_p = NULL;

    if (hash_table == NULL || key == NULL || hash_table->table == NULL)
    {
        return false;
    }

    hash_index = hash_table->hash_func(key, hash_table->capacity);
    hash_index = hash_index % hash_table->capacity; /* Just to be safe */
    head_p = (ListNode_t **)&hash_table->table[hash_index];
    node = linked_list_search(head_p, key, hash_table->match_func);

    if (node == NULL)
    {
        return false; /* item was not found */
    }

    linked_list_delete_node(head_p, node, hash_table->free_node);
    hash_table->size--;

    return true;
}

/*****************************************************************************
 *
 *   Name:       hash_table_free
 *
 *   Input:      hash_table_p  A pointer to a pointer to the hash table to be freed
 *
 *   Return:     None
 *
 *   Description:            Frees all memory associated with the specified hash table,
 *                           including its nodes and the table itself. After freeing,
 *                           the hash table pointer is set to NULL.
 ******************************************************************************/
void hash_table_free(HashTable_t **hash_table_p)
{
    uint64_t i = 0;
    ListNode_t **head_p = NULL;
    HashTable_t *hash_table = NULL;

    if (hash_table_p == NULL || *hash_table_p == NULL || (*hash_table_p)->table == NULL)
    {
        return;
    }

    hash_table = *hash_table_p;

    for (i = 0; i < hash_table->capacity; i++)
    {
        head_p = (ListNode_t **)&hash_table->table[i];
        hash_table->size -= linked_list_delete_list(head_p, hash_table->free_node);
    }

    free(hash_table->table);
    hash_table->table = NULL;

    hash_table = NULL;

    free(*hash_table_p);
    *hash_table_p = NULL;

    return;
}