#ifndef ELY_STX_LIST_H
#define ELY_STX_LIST_H

#include <stdint.h>
#include <stddef.h>

#include "ely/location.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_stx_datum ely_stx_datum;

static inline void ely_stx_datum_destroy(ely_stx_datum*);

typedef enum
{
    ELY_STX_LIST_PARENS,
    ELY_STX_LIST_BRACKET,
    ELY_STX_LIST_BRACE
} ely_stx_list_type;

typedef struct
{
    ely_position      pos;
    ely_stx_datum*    data;
    uint32_t          data_len;
    ely_stx_list_type type;
} ely_stx_list;

ely_stx_list ely_stx_list_create(ely_stx_list_type   ty,
                                 ely_stx_datum*      data,
                                 size_t              len,
                                 const ely_position* pos);
void         ely_stx_list_destroy(ely_stx_list* list);

#ifdef __cplusplus
}
#endif

#endif