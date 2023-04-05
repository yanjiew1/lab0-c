#include <string.h>
#include "list_sort.h"
#include "queue.h"

// This code is generated by GitHub copilot
static int q_cmp(void *priv,
                 const struct list_head *a,
                 const struct list_head *b)
{
    // cppcheck-suppress nullPointer
    element_t *elem_a = list_entry(a, element_t, list);
    // cppcheck-suppress nullPointer
    element_t *elem_b = list_entry(b, element_t, list);

    return strcmp(elem_a->value, elem_b->value);
}

static int q_cmp_descend(void *priv,
                         const struct list_head *a,
                         const struct list_head *b)
{
    // cppcheck-suppress nullPointer
    element_t *elem_a = list_entry(a, element_t, list);
    // cppcheck-suppress nullPointer
    element_t *elem_b = list_entry(b, element_t, list);

    return strcmp(elem_b->value, elem_a->value);
}

void q_ksort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    list_cmp_func_t cmp = descend ? q_cmp_descend : q_cmp;

    list_sort(NULL, head, cmp);
}
