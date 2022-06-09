#include "ely/stx/list.h"

#include <stdlib.h>
#include <string.h>

#include "ely/stx/datum.h"

void ely_list_init(ely_list* list, ely_list_type ty, const ely_position* pos)
{
    list->type = ty;
    list->pos  = *pos;
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