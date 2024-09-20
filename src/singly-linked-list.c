#include <stdio.h>
#include <stdlib.h>

#include "singly-linked-list.h"

/*****************************************************************************
 *
 *   Name:       linked_list_delete_node
 *
 *   Input:      head           A pointer to a pointer to the head of the linked list
 *               node           A pointer to the node to be deleted
 *               free_data      A function pointer to free the node's data, can be null if
 *                              only want to remove node from list and not delete.
 *   Return:     None
 *   Description:            Deletes the specified node from the linked list.
 ******************************************************************************/
void linked_list_delete_node(ListNode_t **head, ListNode_t *node, free_data_t free_data) /* OK */
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

/*****************************************************************************
 *
 *   Name:       linked_list_delete_list
 *
 *   Input:      head           A pointer to a pointer to the head of the linked list
 *               free_data      A function pointer to free the data of each node
 *   Return:     Count          The number of nodes deleted
 *   Description:            Deletes all nodes in the linked list, freeing their memory
 *                           using the provided free_data function.
 ******************************************************************************/
uint64_t linked_list_delete_list(ListNode_t **head, free_data_t free_data) /* OK */
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

/*****************************************************************************
 *
 *   Name:       linked_list_insert_at_head
 *
 *   Input:      head           A pointer to a pointer to the head of the linked list
 *               new_node       A pointer to the new node to be inserted
 *   Return:     None
 *   Description:            Inserts a new node at the head of the linked list.
 ******************************************************************************/
void linked_list_insert_at_head(ListNode_t **head, ListNode_t *new_node) /* OK */
{
    if (head == NULL || new_node == NULL)
    {
        return;
    }

    new_node->next = *head;
    *head = new_node;

    return;
}

/*****************************************************************************
 *
 *   Name:       linked_list_search
 *
 *   Input:      head           A pointer to a pointer to the head of the linked list
 *               search_item    A pointer to the item to search for
 *               match_func     A function pointer for matching nodes
 *   Return:     Success         A pointer to the first matching node if found
 *               Failed          NULL if the item is not found or parameters are invalid
 *   Description:            Searches the linked list for a node that matches the given
 *                           search_item using the provided match_func.
 ******************************************************************************/
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