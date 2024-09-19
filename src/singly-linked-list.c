#include <stdio.h>
#include <stdlib.h>

#include "singly-linked-list.h"

/* Function to delete a node from the linked list */
void linked_list_delete_node(ListNode_t **head, ListNode_t *node, free_data_t free_data)
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

uint64_t linked_list_delete_list(ListNode_t **head, free_data_t free_data)
{
    ListNode_t *node = NULL;
    uint64_t count = 0;

    if (head == NULL || *head == NULL || free_data == NULL)
    {
        return 0;
    }

    while (*head != NULL)
    {
        node = (*head)->next;
        (*head)->next = NULL;

        free_data(*head);
        *head = node;
        count++;
    }

    return count;
}

/* Function to insert a node at the head of the list */
void linked_list_insert_at_head(ListNode_t **head, ListNode_t *new_node)
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
ListNode_t *linked_list_search(ListNode_t **head, const void *search_item, match_func_t match_func)
{
    ListNode_t *current = NULL;

    if (head == NULL || *head == NULL || search_item == NULL || match_func == NULL)
    {
        return NULL;
    }

    current = *head;

    while (current != NULL)
    {
        if (match_func(current, search_item))
        {
            return current;
        }

        current = current->next;
    }

    return NULL;
}