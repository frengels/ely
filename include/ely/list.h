#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "ely/export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ElyList ElyList;

typedef struct ElyList
{
    ElyList* next;
    ElyList* prev;
} ElyList;

/// initialize next and prev to reference list
ELY_EXPORT void ely_list_create(ElyList* list);
/// insert element after list
ELY_EXPORT void ely_list_insert(ElyList* list, ElyList* element);
/// check if the list is empty (self-referential)
ELY_EXPORT bool ely_list_empty(const ElyList* list);
/// count the elements in the list O(n)
ELY_EXPORT uint32_t ely_list_length(const ElyList* list);
/// insert elements of other after the point indicated by list
ELY_EXPORT void ely_list_insert_list(ElyList* list, ElyList* other);

#define ely_container_of(ptr, sample, member)                                  \
    (__typeof__(sample))((char*) (ptr) -offsetof(__typeof__(*sample), member))

#define ely_list_for_each(pos, head, member)                                   \
    for (pos = ely_container_of((head)->next, pos, member);                    \
         &pos->member != (head);                                               \
         pos = ely_container_of(pos->member.next, pos, member))

#define ely_list_for_each_safe(pos, tmp, head, member)                         \
    for (pos = ely_container_of((head)->next, pos, member),                    \
        tmp  = ely_container_of((pos)->member.next, tmp, member);              \
         &pos->member != (head);                                               \
         pos = tmp, tmp = ely_container_of(pos->member.next, tmp, member))

#define ely_list_for_each_reverse(pos, head, member)                           \
    for (pos = ely_container_of((head)->prev, pos, member);                    \
         &pos->member != (head);                                               \
         pos = ely_container_of(pos->member.prev, pos, member))

#define ely_list_for_each_reverse_safe(pos, tmp, head, member)                 \
    for (pos = ely_container_of((head)->prev, pos, member),                    \
        tmp  = ely_container_of((pos)->member.prev, tmp, member);              \
         &pos->member != (head);                                               \
         pos = tmp, tmp = ely_container_of(pos->member.prev, tmp, member))

#ifdef __cplusplus
}
#endif
