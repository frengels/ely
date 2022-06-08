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

typedef struct ely_list
{
    struct ely_list* prev;
    struct ely_list* next;
} ely_list;

ELY_EXPORT void     ely_list_init(ely_list* list);
ELY_EXPORT void     ely_list_insert(ely_list* list, ely_list* element);
ELY_EXPORT void     ely_list_remove(ely_list* element);
ELY_EXPORT uint32_t ely_list_length(const ely_list* list);
ELY_EXPORT bool     ely_list_empty(const ely_list* list);
ELY_EXPORT void     ely_list_insert_list(ely_list* list, ely_list* other);

#define ELY_LIST_FOR_EACH(pos, head, member)                                   \
    for (pos = ELY_CONTAINER_OF((head)->next, pos, member);                    \
         &pos->member != (head);                                               \
         pos = ELY_CONTAINER_OF(pos->member.next, pos, member))

#define ELY_LIST_FOR_EACH_SAFE(pos, tmp, head, member)                         \
    for (pos = ELY_CONTAINER_OF((head)->next, pos, member),                    \
        tmp  = ELY_CONTAINER_OF((pos)->member.next, tmp, member);              \
         &pos->member != (head);                                               \
         pos = tmp, tmp = ELY_CONTAINER_OF(pos->member.next, tmp, member))

#define ELY_LIST_FOR_EACH_REVERSE(pos, head, member)                           \
    for (pos = ELY_CONTAINER_OF((head)->prev, pos, member);                    \
         &pos->member != (head);                                               \
         pos = ELY_CONTAINER_OF(pos->member.prev, pos, member))

#define ELY_LIST_FOR_EACH_REVERSE_SAFE(pos, tmp, head, member)                 \
    for (pos = ELY_CONTAINER_OF((head)->prev, pos, member),                    \
        tmp  = ELY_CONTAINER_OF((pos)->member.prev, tmp, member);              \
         &pos->member != (head);                                               \
         pos = tmp, tmp = ELY_CONTAINER_OF(pos->member.prev, tmp, member))

#ifdef __cplusplus
}
#endif

#endif