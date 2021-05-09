#include "ely/list.h"

void ely_list_create(ElyList* list)
{
    list->next = list;
    list->prev = list;
}

void ely_list_insert(ElyList* list, ElyList* element)
{
    element->prev       = list;
    element->next       = list->next;
    list->next          = element;
    element->next->prev = element;
}

bool ely_list_empty(const ElyList* list)
{
    return list->next == list;
}

uint32_t ely_list_length(const ElyList* list)
{
    uint32_t count = 0;
    ElyList* e     = list->next;

    while (e != list)
    {
        e = e->next;
        ++count;
    }

    return count;
}

void ely_list_insert_list(ElyList* list, ElyList* other)
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