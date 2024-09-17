#ifndef __SINGLY_LINKED_LIST_H__
#define __SINGLY_LINKED_LIST_H__

#include <stdint.h>

typedef struct ListNode
{
    struct ListNode *next;
} ListNode_t;

void delete_node(ListNode_t **head, ListNode_t *node, void (*free_data)(ListNode_t *));
void insert_at_head(ListNode_t **head, ListNode_t *new_node);
ListNode_t *search(ListNode_t **head, uint32_t search_item,
                   int32_t (*match_func)(ListNode_t *, uint32_t));

#endif /* __SINGLY_LINKED_LIST_H__ */
