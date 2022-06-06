#ifndef ELY_STX_DATUM_H
#define ELY_STX_DATUM_H

#include "ely/stx/identifier.h"
#include "ely/stx/list.h"
#include "ely/stx/literal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    ELY_STX_DATUM_LITERAL,
    ELY_STX_DATUM_LIST,
    ELY_STX_DATUM_IDENTIFIER
} ely_stx_datum_type;

typedef struct
{
    ely_stx_datum_type type;
    union
    {
        ely_stx_literal    lit;
        ely_stx_list       list;
        ely_stx_identifier ident;
    } data;
} ely_stx_datum;

static inline ely_stx_datum ely_stx_datum_create_literal(ely_stx_literal lit)
{
    ely_stx_datum res;
    res.type     = ELY_STX_DATUM_LITERAL;
    res.data.lit = lit;
    return res;
}

static inline ely_stx_datum
ely_stx_datum_create_identifier(ely_stx_identifier ident)
{
    ely_stx_datum res;
    res.type       = ELY_STX_DATUM_IDENTIFIER;
    res.data.ident = ident;
    return res;
}

static inline ely_stx_datum ely_stx_datum_create_list(ely_stx_list list)
{
    ely_stx_datum res;
    res.type      = ELY_STX_DATUM_LIST;
    res.data.list = list;
    return res;
}

static inline void ely_stx_datum_destroy(ely_stx_datum* datum)
{
    switch (datum->type)
    {
    case ELY_STX_DATUM_LITERAL:
        return ely_stx_literal_destroy(&datum->data.lit);
    case ELY_STX_DATUM_LIST:
        return ely_stx_list_destroy(&datum->data.list);
    case ELY_STX_DATUM_IDENTIFIER:
        return ely_stx_identifier_destroy(&datum->data.ident);
    default:
        __builtin_unreachable();
    }
}

#ifdef __cplusplus
}
#endif

#endif