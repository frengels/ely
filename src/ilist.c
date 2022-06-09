#include "ely/ilist.h"

void ely_ilist_init(ely_ilist* list)
{
    list->prev = list;
    list->next = list;
}

void ely_ilist_insert(ely_ilist* list, ely_ilist* element)
{
    element->prev       = list;
    element->next       = list->next;
    list->next          = element;
    element->next->prev = element;
}

void ely_ilist_remove(ely_ilist* element)
{
    element->prev->next = element->next;
    element->next->prev = element->prev;
    element->next       = NULL;
    element->prev       = NULL;
}

uint32_t ely_ilist_length(const ely_ilist* list)
{
    ely_ilist* e   = list->next;
    uint32_t   len = 0;

    while (e != list)
    {
        e = e->next;
        ++len;
    }

    return len;
}

bool ely_ilist_empty(const ely_ilist* list)
{
    return list->next == list;
}

void ely_ilist_insert_list(ely_ilist* list, ely_ilist* other)
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