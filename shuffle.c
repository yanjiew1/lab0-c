#include "shuffle.h"
#include <stdlib.h>
#include "queue.h"

void q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    int size = q_size(head);
    struct list_head **entries = malloc(sizeof(struct list_head *) * size);
    if (!entries)
        return;

    /* Fill in entries array */
    struct list_head *node;
    int i = 0;
    list_for_each (node, head)
        entries[i++] = node;

    /* Fisher–Yates shuffle */
    for (i = size - 1; i > 0; i--) {
        /* The random generator can be improved to avoid bias */
        int n = rand() % i;
        /* swap i-th and n-th element */
        struct list_head *tmp = entries[i];
        entries[i] = entries[n];
        entries[n] = tmp;
    }

    /* Rebuild list */
    INIT_LIST_HEAD(head);
    for (i = 0; i < size; i++)
        list_add_tail(entries[i], head);

    free(entries);
}
