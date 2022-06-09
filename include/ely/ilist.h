#ifndef ELY_LIST_H
#define ELY_LIST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ely/export.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ELY_CONTAINER_OF(ptr, sample, member)                                  \
    (__typeof__(sample)) ((char*) (ptr) -offsetof(__typeof__(*sample), member))

typedef struct ely_ilist
{
    struct ely_ilist* prev;
    struct ely_ilist* next;
} ely_ilist;

ELY_EXPORT void     ely_ilist_init(ely_ilist* list);
ELY_EXPORT void     ely_ilist_insert(ely_ilist* list, ely_ilist* element);
ELY_EXPORT void     ely_ilist_remove(ely_ilist* element);
ELY_EXPORT uint32_t ely_ilist_length(const ely_ilist* list);
ELY_EXPORT bool     ely_ilist_empty(const ely_ilist* list);
ELY_EXPORT void     ely_ilist_insert_list(ely_ilist* list, ely_ilist* other);

#define ELY_ILIST_FOR_EACH(pos, head, member)                                  \
    for (pos = ELY_CONTAINER_OF((head)->next, pos, member);                    \
         &pos->member != (head);                                               \
         pos = ELY_CONTAINER_OF(pos->member.next, pos, member))

#define ELY_ILIST_FOR_EACH_SAFE(pos, tmp, head, member)                        \
    for (pos = ELY_CONTAINER_OF((head)->next, pos, member),                    \
        tmp  = ELY_CONTAINER_OF((pos)->member.next, tmp, member);              \
         &pos->member != (head);                                               \
         pos = tmp, tmp = ELY_CONTAINER_OF(pos->member.next, tmp, member))

#define ELY_ILIST_FOR_EACH_REVERSE(pos, head, member)                          \
    for (pos = ELY_CONTAINER_OF((head)->prev, pos, member);                    \
         &pos->member != (head);                                               \
         pos = ELY_CONTAINER_OF(pos->member.prev, pos, member))

#define ELY_ILIST_FOR_EACH_REVERSE_SAFE(pos, tmp, head, member)                \
    for (pos = ELY_CONTAINER_OF((head)->prev, pos, member),                    \
        tmp  = ELY_CONTAINER_OF((pos)->member.prev, tmp, member);              \
         &pos->member != (head);                                               \
         pos = tmp, tmp = ELY_CONTAINER_OF(pos->member.prev, tmp, member))

#ifdef __cplusplus
}
#endif

#endif