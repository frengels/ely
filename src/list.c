#include "ely/list.h"

void ely_list_init(ely_list* list)
{
    list->prev = list;
    list->next = list;
}

void ely_list_insert(ely_list* list, ely_list* element)
{
    element->prev       = list;
    element->next       = list->next;
    list->next          = element;
    element->next->prev = element;
}

void ely_list_remove(ely_list* element)
{
    element->prev->next = element->next;
    element->next->prev = element->prev;
    element->next       = NULL;
    element->prev       = NULL;
}

uint32_t ely_list_length(const ely_list* list)
{
    ely_list* e   = list->next;
    uint32_t  len = 0;

    while (e != list)
    {
        e = e->next;
        ++len;
    }

    return len;
}

bool ely_list_empty(const ely_list* list)
{
    return list->next == list;
}

void ely_list_insert_list(ely_list* list, ely_list* other)
{
    if (ely_list_empty(other))
    {
        return;
    }

    other->next->prev = list;
    other->prev->next = list->next;
    list->next->prev  = other->prev;
    list->next        = other->next;
}