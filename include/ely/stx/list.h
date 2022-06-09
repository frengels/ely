#ifndef ELY_STX_LIST_H
#define ELY_STX_LIST_H

#include <stddef.h>
#include <stdint.h>

#include "ely/export.h"
#include "ely/ilist.h"
#include "ely/location.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_stx_datum ely_stx_datum;

typedef enum ely_list_type
{
    ELY_LIST_PARENS,
    ELY_LIST_BRACKET,
    ELY_LIST_BRACE
} ely_list_type;

typedef struct ely_list
{
    ely_list_type type;
    ely_position  pos;
    ely_ilist     head;
} ely_list;

ELY_NO_EXPORT void
ely_list_init(ely_list* list, ely_list_type ty, const ely_position* pos);
ELY_NO_EXPORT void ely_list_destroy(ely_list* list);

#ifdef __cplusplus
}
#endif

#endif