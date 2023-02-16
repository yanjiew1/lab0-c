#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Notice: sometimes, Cppcheck would find the potential NULL pointer bugs,
 * but some of them cannot occur. You can suppress them by adding the
 * following line.
 *   cppcheck-suppress nullPointer
 */

/* Create a new element with the provided string */
static inline element_t *q_new_elem(char *s)
{
    element_t *elem = malloc(sizeof(element_t));
    if (!elem)
        return NULL;

    elem->value = strdup(s);

    return elem;
}

static inline void q_copy_string(char *dest, size_t size, const char *src)
{
    dest[size - 1] = '\0';
    for (int i = 0; i < size - 1 && src[i] != '\0'; i++)
        dest[i] = src[i];
}

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));
    if (!new)
        return NULL;

    INIT_LIST_HEAD(new);

    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *l)
{
    if (!l)
        return;

    element_t *safe;
    element_t *it;

    list_for_each_entry_safe (it, safe, l, list) {
        list_del(&it->list);
        q_release_element(it);
    }

    free(l);
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *elem = q_new_elem(s);
    if (!elem)
        return false;

    list_add(&elem->list, head);

    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *elem = q_new_elem(s);
    if (!elem)
        return false;

    list_add_tail(&elem->list, head);

    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *elem = list_first_entry(head, element_t, list);
    list_del(&elem->list);

    if (sp)
        q_copy_string(sp, bufsize, elem->value);

    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *elem = list_last_entry(head, element_t, list);
    list_del(&elem->list);

    if (sp)
        q_copy_string(sp, bufsize, elem->value);

    return elem;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int count = 0;
    struct list_head *it;

    list_for_each (it, head)
        count++;

    return count;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head))
        return false;

    struct list_head *left = head->next;
    struct list_head *right = head->prev;

    while (left != right && left->next != right) {
        left = left->next;
        right = right->prev;
    }

    list_del(right);
    element_t *elem = list_entry(right, element_t, list);
    q_release_element(elem);

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    return true;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *it, *safe;
    /* Iterate the list and move each item to the head */
    list_for_each_safe (it, safe, head) {
        list_move(it, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head)
        return;
    LIST_HEAD(tmp);
    struct list_head *it, *safe, *cut;
    int cnt = k;
    cut = head;
    list_for_each_safe (it, safe, head) {
        cnt--;
        if (cnt == 0) {
            cnt = k;
            list_cut_position(&tmp, cut, it);
            q_reverse(&tmp);
            list_splice_init(&tmp, cut);
            cut = safe->prev;
        }
    }
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head) {}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending order */
int q_merge(struct list_head *head)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}
