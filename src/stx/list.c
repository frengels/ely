#include "ely/stx/list.h"

#include <stdlib.h>
#include <string.h>

#include "ely/stx/datum.h"

ely_stx_list ely_stx_list_create(ely_stx_list_type   ty,
                                 ely_stx_datum*      data,
                                 size_t              len,
                                 const ely_position* pos)
{
    ely_stx_datum* data_dst = malloc(sizeof(ely_stx_datum) * len);
    memcpy(data_dst, data, len * sizeof(ely_stx_datum));

    ely_stx_list res;
    res.pos      = *pos;
    res.data     = data_dst;
    res.data_len = (uint32_t) len;
    res.type     = ty;

    return res;
}

void ely_stx_list_destroy(ely_stx_list* list)
{
    free(list->data);
}