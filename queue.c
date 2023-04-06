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
    char *tmp = strdup(s);

    if (!elem || !tmp) {
        free(elem);
        free(tmp);
        return NULL;
    }

    elem->value = tmp;
    return elem;
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

    element_t *safe, *it;
    list_for_each_entry_safe (it, safe, l, list)
        q_release_element(it);

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

    return q_insert_head(head->prev, s);
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *elem = list_first_entry(head, element_t, list);
    list_del(&elem->list);

    if (!sp || !bufsize)
        return elem;

    strncpy(sp, elem->value, bufsize);
    sp[bufsize - 1] = '\0';

    return elem;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    return q_remove_head(head->prev->prev, sp, bufsize);
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
    if (!head)
        return false;

    LIST_HEAD(pending);
    element_t *it, *safe;
    struct list_head *cut = head;

    list_for_each_entry_safe (it, safe, head, list) {
        if (&safe->list != head && !strcmp(safe->value, it->value))
            continue;
        /* Detect duplicated elements */
        if (it->list.prev != cut) {
            LIST_HEAD(tmp);
            list_cut_position(&tmp, cut, &it->list);
            list_splice(&tmp, &pending);
        }
        cut = safe->list.prev;
    }

    /* Process pending list */
    list_for_each_entry_safe (it, safe, &pending, list)
        q_release_element(it);

    return true;
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head)
        return;

    struct list_head *it, *safe;
    /* Iterate the list and move each item to the head */
    list_for_each_safe (it, safe, head)
        list_move(it, head);
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head))
        return;
    struct list_head *it, *safe, *cut;
    int cnt = k;
    cut = head;
    list_for_each_safe (it, safe, head) {
        if (--cnt)
            continue;
        LIST_HEAD(tmp);
        cnt = k;
        list_cut_position(&tmp, cut, it);
        q_reverse(&tmp);
        list_splice(&tmp, cut);
        cut = safe->prev;
    }
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    q_reverseK(head, 2);
}

/* Merge two sorted list into `first` list */
/* Thanks `alanjian85` for inspiration */
static int q_merge_two(struct list_head *first,
                       struct list_head *second,
                       bool descend)
{
    if (!first || !second)
        return 0;

    int count = 0;
    LIST_HEAD(tmp);
    while (!list_empty(first) && !list_empty(second)) {
        element_t *f = list_first_entry(first, element_t, list);
        element_t *s = list_first_entry(second, element_t, list);
        int cmp = strcmp(f->value, s->value);
        if (descend)
            cmp = -cmp;
        if (cmp <= 0)
            list_move_tail(&f->list, &tmp);
        else
            list_move_tail(&s->list, &tmp);
        count++;
    }
    count += q_size(first) + q_size(second);
    list_splice(&tmp, first);
    list_splice_tail_init(second, first);

    return count;
}

/* Sort elements of queue in ascending order */
void q_sort(struct list_head *head, bool descend)
{
    /* Try to use merge sort*/
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    /* Find middle point */
    struct list_head *mid, *left, *right;
    left = right = head;
    do {
        left = left->next;
        right = right->prev;
    } while (left != right && left->next != right);
    mid = left;

    /* Divide into two part */
    LIST_HEAD(second);
    list_cut_position(&second, mid, head->prev);

    /* Conquer */
    q_sort(head, descend);
    q_sort(&second, descend);

    /* Merge */
    q_merge_two(head, &second, descend);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    /**
     * Traverse from the last entry and remove the element that is
     * smaller or equal to its right. Also count the number of elements.
     */
    int cnt = 1;
    element_t *cur = list_last_entry(head, element_t, list);
    while (cur->list.prev != head) {
        element_t *prev = list_last_entry(&cur->list, element_t, list);
        if (strcmp(prev->value, cur->value) > 0) {
            list_del(&prev->list);
            q_release_element(prev);
        } else {
            cnt++;
            cur = prev;
        }
    }

    return cnt;
}


/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    if (!head || list_empty(head))
        return 0;

    /**
     * Traverse from the last entry and remove the element that is
     * smaller or equal to its right. Also count the number of elements.
     */
    int cnt = 1;
    element_t *cur = list_last_entry(head, element_t, list);
    while (cur->list.prev != head) {
        element_t *prev = list_last_entry(&cur->list, element_t, list);
        if (strcmp(prev->value, cur->value) < 0) {
            list_del(&prev->list);
            q_release_element(prev);
        } else {
            cnt++;
            cur = prev;
        }
    }

    return cnt;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    if (!head || list_empty(head))
        return 0;

    if (list_is_singular(head))
        return list_first_entry(head, queue_contex_t, chain)->size;

    /* Use 2-merge algorithm in https://arxiv.org/pdf/1801.04641.pdf */
    LIST_HEAD(pending);
    LIST_HEAD(empty);
    int n = 0;
    while (!list_empty(head)) {
        list_move(head->next, &pending);
        n++;

        while (n > 3) {
            queue_contex_t *x, *y, *z;
            x = list_first_entry(&pending, queue_contex_t, chain);
            y = list_first_entry(&x->chain, queue_contex_t, chain);
            z = list_first_entry(&x->chain, queue_contex_t, chain);

            if (y->size >= z->size << 1)
                break;

            if (x->size < z->size) {
                y->size = q_merge_two(y->q, x->q, descend);
                list_move(&x->chain, &empty);
                n--;
            } else {
                z->size = q_merge_two(z->q, y->q, descend);
                list_move(&y->chain, &empty);
                n--;
            }
        }
    }

    /* Merge remaining list */
    while (n > 1) {
        queue_contex_t *x, *y;
        x = list_first_entry(&pending, queue_contex_t, chain);
        y = list_first_entry(&x->chain, queue_contex_t, chain);
        y->size = q_merge_two(y->q, x->q, descend);
        list_move(&x->chain, &empty);
        n--;
    }

    /* Move the last queue and empty queue to head */
    list_splice(&empty, head);
    list_splice(&pending, head);

    return list_first_entry(head, queue_contex_t, chain)->size;
}
