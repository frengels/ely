#include "ely/stx/list.hpp"

#include <stdlib.h>
#include <string.h>

#include "ely/stx/datum.hpp"

void ely_list_init(ely_list* list, ely_list_type ty, llvm::SMRange range)
{
    list->type  = ty;
    list->range = range;
    ely_ilist_init(&list->head);
}

void ely_list_destroy(ely_list* list)
{
    ely_ilist* it = list->head.next;
    while (it != &list->head)
    {
        ely_datum* datum = ELY_CONTAINER_OF(it, datum, link);
        it               = it->next;
        ely_datum_destroy(datum);
    }
}

ELY_EXPORT uint32_t ely_list_length(const ely_list* list)
{
    return ely_ilist_length(&list->head);
}

ELY_EXPORT bool ely_list_empty(const ely_list* list)
{
    return ely_ilist_empty(&list->head);
}

ELY_EXPORT void ely_list_insert(ely_list* list, ely_datum* datum)
{
    return ely_ilist_insert(&list->head, &datum->link);
}

ELY_EXPORT void ely_list_insert_list(ely_list* list, ely_list* other)
{
    return ely_ilist_insert_list(&list->head, &other->head);
}

ELY_EXPORT ely_datum* ely_list_head(ely_list* list)
{
    ely_ilist* head_link = list->head.next;
    ely_datum* datum     = ELY_CONTAINER_OF(head_link, datum, link);
    return datum;
}