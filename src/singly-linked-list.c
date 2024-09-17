#include <stdio.h>
#include <stdlib.h>

#include "singly-linked-list.h"

/* Function to delete a node from the linked list */
void delete_node(ListNode_t **head, ListNode_t *node, void (*free_data)(ListNode_t *)) // OK
{
    ListNode_t *prev = NULL;

    if (node == NULL || head == NULL || *head == NULL)
    {
        return;
    }

    if (*head == node)
    {
        *head = node->next;
        node->next = NULL;
    }
    else
    {
        prev = *head;

        while (prev != NULL && prev->next != node)
        {
            prev = prev->next;
        }

        if (prev != NULL)
        {
            prev->next = node->next;
            node->next = NULL;
        }
    }

    if (free_data != NULL)
    {
        free_data(node);
    }

    return;
}

/* Function to insert a node at the head of the list */
void insert_at_head(ListNode_t **head, ListNode_t *new_node) // OK
{
    if (head == NULL || new_node == NULL)
    {
        return;
    }

    new_node->next = *head;
    *head = new_node;

    return;
}

/* Function to search for a node in the list */
ListNode_t *search(ListNode_t **head, uint32_t search_item,
                   int32_t (*match_func)(ListNode_t *, uint32_t))
{
    ListNode_t *current = *head;

    if (match_func == NULL)
    {
        return NULL;
    }

    while (current != NULL)
    {
        if (match_func(current, search_item) == 0)
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}