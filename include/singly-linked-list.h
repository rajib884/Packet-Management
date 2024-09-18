#ifndef __SINGLY_LINKED_LIST_H__
#define __SINGLY_LINKED_LIST_H__

#include <stdbool.h>
#include <stdint.h>

typedef struct ListNode
{
    struct ListNode *next;
} ListNode_t;

typedef void (*free_data_t)(ListNode_t *);
typedef bool (*match_func_t)(const ListNode_t *, const void *);

void linked_list_delete_node(ListNode_t **head, ListNode_t *node, free_data_t free_data);
bool linked_list_delete_list(ListNode_t **head, free_data_t free_data);
void linked_list_insert_at_head(ListNode_t **head, ListNode_t *new_node);
ListNode_t *linked_list_search(ListNode_t **head, const void *search_item, match_func_t match_func);

#endif /* __SINGLY_LINKED_LIST_H__ */
