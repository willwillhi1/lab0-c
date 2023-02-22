#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */


/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head = malloc(sizeof(struct list_head));
    if (head == NULL)
        return NULL;
    INIT_LIST_HEAD(head);
    return head;
}

/* Free all storage used by queue */

void q_free(struct list_head *l)
{
    if (l == NULL || list_empty(l)) {
        free(l);
        return;
    }
    struct list_head *next = l->next;
    while (l != next) {
        list_del(next);
        element_t *node = list_entry(next, element_t, list);
        next = next->next;
        free(node->value);
        free(node);
    }
    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (node == NULL)
        return false;
    node->value = strdup(s);
    if (node->value == NULL) {
        free(node);
        return false;
    }
    list_add(&node->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (head == NULL)
        return false;
    element_t *node = malloc(sizeof(element_t));
    if (node == NULL)
        return false;
    node->value = strdup(s);
    if (node->value == NULL) {
        free(node);
        return false;
    }
    list_add_tail(&node->list, head);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }

    element_t *target = list_first_entry(head, element_t, list);
    list_del(&target->list);

    if (sp) {
        size_t len = strlen(target->value) + 1;
        len = (bufsize - 1) > len ? len : (bufsize - 1);
        // memcpy(sp, target->value, len);
        strncpy(sp, target->value, len);
        sp[len] = '\0';
    }
    return target;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (head == NULL || list_empty(head))
        return NULL;
    element_t *node = list_entry(head->prev, element_t, list);
    list_del(head->prev);
    if (sp != NULL) {
        size_t len = strlen(node->value) + 1;
        len = (bufsize - 1) > len ? len : (bufsize - 1);
        // memcpy(sp, target->value, len);
        strncpy(sp, node->value, len);
        sp[len] = '\0';
    }
    return node;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return 0;
    int size = 0;
    struct list_head *l;
    list_for_each (l, head)
        size++;
    return size;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (head == NULL || list_empty(head))
        return false;
    struct list_head *first = head->next;
    struct list_head *second = head->prev;
    while ((first != second) && (first->next != second)) {
        first = first->next;
        second = second->prev;
    }
    list_del(first);
    element_t *node = list_entry(first, element_t, list);
    free(node->value);
    free(node);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head)
        return false;

    element_t *first;
    element_t *second;
    bool isdup = false;
    list_for_each_entry_safe (first, second, head, list) {
        if (&second->list != head && !strcmp(first->value, second->value)) {
            list_del(&first->list);
            q_release_element(first);
            isdup = true;
        } else if (isdup) {
            list_del(&first->list);
            q_release_element(first);
            isdup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (head == NULL || list_empty(head))
        return;
    struct list_head *first = head->next;
    struct list_head *second = head->next->next;
    while (first != head && second != head) {
        first->prev->next = second;
        first->next = second->next;
        second->prev = first->prev;
        first->prev = second;
        second->next->prev = first;
        second->next = first;

        first = first->next;
        second = first->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return;
    struct list_head *first = head;
    struct list_head *second = head->next;
    do {
        first->next = first->prev;
        first->prev = second;

        first = second;
        second = first->next;
    } while (first != head);
}

void print(struct list_head *head)
{
    struct list_head *tmp = head->next;
    while (tmp != head) {
        element_t *node = list_entry(tmp, element_t, list);
        printf("%s -> ", node->value);
        tmp = tmp->next;
    }
    printf("\n");
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head)
        return;

    struct list_head *last = head->next;
    int times = q_size(head) / k;
    LIST_HEAD(tmp);
    LIST_HEAD(result);

    for (int i = 0; i < times; ++i) {
        for (int j = 0; j < k; ++j)
            last = last->next;
        list_cut_position(&tmp, head, last->prev);
        q_reverse(&tmp);
        list_splice_tail_init(&tmp, &result);
    }
    list_splice_init(&result, head);
}

struct list_head *mergelist(struct list_head *l1, struct list_head *l2)
{
    struct list_head *head = NULL;
    struct list_head **cur = &head;
    while (l1 && l2) {
        element_t *e1 = list_entry(l1, element_t, list);
        element_t *e2 = list_entry(l2, element_t, list);
        if (strcmp(e1->value, e2->value) >= 0) {
            *cur = l2;
            l2 = l2->next;
        } else {
            *cur = l1;
            l1 = l1->next;
        }
        cur = &(*cur)->next;
    }
    *cur = (struct list_head *) ((u_int64_t) l1 | (u_int64_t) l2);
    return head;
}

struct list_head *mergesort(struct list_head *l)
{
    if (!l || !l->next)
        return l;

    struct list_head *first = l;
    struct list_head *last = l->prev;

    while (first != last && first->next != last) {
        first = first->next;
        last = last->prev;
    }
    struct list_head *l1 = l;
    struct list_head *l2 = first->next;
    l2->prev = l->prev;
    l1->prev = first;
    first->next = NULL;

    return mergelist(mergesort(l1), mergesort(l2));
}

// Sort elements of queue in ascending order
void q_sort(struct list_head *head)
{
    if (head == NULL || list_empty(head))
        return;
    head->prev->next = NULL;
    head->next->prev = head->prev;
    head->next = mergesort(head->next);

    struct list_head *cur = head;
    struct list_head *next = head->next;
    while (next) {
        next->prev = cur;
        cur = next;
        next = next->next;
    }
    cur->next = head;
    head->prev = cur;
}


/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (head == NULL || list_empty(head))
        return 0;
    element_t *first = list_entry(head->prev, element_t, list);
    element_t *second = list_entry(head->prev->prev, element_t, list);
    while (&second->list != head) {
        if (strcmp(first->value, second->value) < 0) {
            second = list_entry(second->list.prev, element_t, list);
            first = list_entry(first->list.prev, element_t, list);
        } else {
            list_del(&second->list);
            q_release_element(second);
            second = list_entry(first->list.prev, element_t, list);
        }
    }
    return q_size(head);
}

int merge_two_list(struct list_head *first, struct list_head *second)
{
    if (!first || !second)
        return 0;
    struct list_head temp_head;
    INIT_LIST_HEAD(&temp_head);
    while (!list_empty(first) && !list_empty(second)) {
        element_t *first_front = list_first_entry(first, element_t, list);
        element_t *second_front = list_first_entry(second, element_t, list);
        char *first_str = first_front->value, *second_str = second_front->value;
        element_t *minimum =
            strcmp(first_str, second_str) < 0 ? first_front : second_front;
        list_move_tail(&minimum->list, &temp_head);
    }
    list_splice_tail_init(first, &temp_head);
    list_splice_tail(second, &temp_head);
    list_splice(&temp_head, first);
    return q_size(first);
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;
    else if (list_is_singular(head))
        return q_size(list_first_entry(head, queue_contex_t, chain)->q);
    int size = q_size(head);
    int count = (size % 2) ? size / 2 + 1 : size / 2;
    int queue_size = 0;
    for (int i = 0; i < count; ++i) {
        queue_contex_t *first = list_first_entry(head, queue_contex_t, chain);
        queue_contex_t *second =
            list_entry(first->chain.next, queue_contex_t, chain);
        while (first->q && second->q) {
            queue_size = merge_two_list(first->q, second->q);
            second->q = NULL;
            list_move_tail(&second->chain, head);
            first = list_entry(first->chain.next, queue_contex_t, chain);
            second = list_entry(first->chain.next, queue_contex_t, chain);
        }
    }
    return queue_size;
}

static inline void swap(struct list_head *node_1, struct list_head *node_2)
{
    if (node_1 == node_2)
        return;
    struct list_head *node_1_prev = node_1->prev;
    struct list_head *node_2_prev = node_2->prev;
    if (node_1->prev != node_2)
        list_move(node_2, node_1_prev);
    list_move(node_1, node_2_prev);
}

void q_shuffle(struct list_head *head)
{
    if (!head || list_is_singular(head))
        return;
    struct list_head *last = head;
    int size = q_size(head);
    while (size > 0) {
        int index = rand() % size;
        struct list_head *new = last->prev;
        struct list_head *old = new;
        while (index--)
            old = old->prev;
        swap(new, old);
        last = last->prev;
        size--;
    }
    return;
}