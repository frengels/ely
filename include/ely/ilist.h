#ifndef MRT_ILIST_H
#define MRT_ILIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ely/config.h"

typedef struct ely_ilist ely_ilist;

typedef struct ely_ilist
{
    ely_ilist* prev;
    ely_ilist* next;
} ely_ilist;

ELY_ALWAYS_INLINE void ely_ilist_init(ely_ilist* list)
{
    list->next = list;
    list->prev = list;
}

ELY_ALWAYS_INLINE void ely_ilist_insert(ely_ilist* list, ely_ilist* el)
{
    el->prev         = list;
    el->next         = list->next;
    list->next->prev = el;
    list->next       = el;
}

ELY_ALWAYS_INLINE void ely_ilist_append(ely_ilist* list, ely_ilist* el)
{
    el->prev         = list->prev;
    el->next         = list;
    list->prev->next = el;
    list->prev       = el;
}

ELY_ALWAYS_INLINE void ely_ilist_remove(ely_ilist* el)
{
    el->next->prev = el->prev;
    el->prev->next = el->next;
    el->next       = NULL;
    el->prev       = NULL;
}

ELY_ALWAYS_INLINE size_t ely_ilist_count_len(const ely_ilist* list)
{
    size_t count = 0;

    ely_ilist* e = list->next;
    while (e != list)
    {
        e = e->next;
        ++count;
    }

    return count;
}

ELY_ALWAYS_INLINE bool ely_ilist_empty(const ely_ilist* list)
{
    return list->next == list;
}

ELY_ALWAYS_INLINE void ely_ilist_append_ilist(ely_ilist* list, ely_ilist* other)
{
    if (ely_ilist_empty(other))
    {
        return;
    }

    other->next->prev = list;
    other->prev->next = list->next;
    list->next->prev  = other->prev;
    list->next        = other->next;
}

#define ELY_CONTAINER_OF(ptr, sample, member)                                  \
    (__typeof__(sample)) ((char*) (ptr) -offsetof(__typeof__(*sample), member))

#define ELY_ILIST_FOR_EACH(pos, head, member)                                  \
    for (pos = ELY_CONTAINER_OF((head)->next, pos, member);                    \
         &pos->member != (head);                                               \
         pos = ELY_CONTAINER_OF(pos->member.next, pos, member))

#define ELY_ILIST_FOR_EACH_SAFE(pos, tmp, head, member)                        \
    for (pos          = ELY_CONTAINER_OF((head)->next, pos, member),           \
        tmp           = ELY_CONTAINER_OF((pos)->member.next, tmp, member);     \
         &pos->member = != (head);                                             \
         pos = tmp, tmp = ELY_CONTAINER_OF(pos->member.next, tmp, member))

#define ELY_ILIST_FOR_EACH_REVERSE(pos, head, member)                          \
    for (pos = ELY_CONTAINER_OF((head)->prev, pos, member);                    \
         &pos->member != (head);                                               \
         pos = ELY_CONTAINER_OF(pos->member.prev, pos, member))

#define ELY_ILIST_FOR_EACH_REVERSE_SAFE(pos, tmp, head, member)                \
    for (pos = ELY_CONTAINER_OF((head)->prev, pos, member);                    \
         tmp = ELY_CONTAINER_OF((pos)->member.prev, tmp, member);              \
         &pos->member != (head);                                               \
         pos = tmp, tmp = ELY_CONTAINER_OF(pos->member.prev, tmp, member))

#ifdef __cplusplus
}
#endif

#endif