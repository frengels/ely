#include "ely/stx/datum.h"

#include "ely/stx/identifier.h"
#include "ely/stx/list.h"
#include "ely/stx/literal.h"

ely_stx_datum ely_stx_datum_create_literal(ely_stx_literal lit)
{
    ely_stx_datum res;
    res.type     = ELY_STX_DATUM_LITERAL;
    res.data.lit = lit;
    return res;
}

ely_stx_datum ely_stx_datum_create_identifier(ely_stx_identifier ident)
{
    ely_stx_datum res;
    res.type       = ELY_STX_DATUM_IDENTIFIER;
    res.data.ident = ident;
    return res;
}

ely_stx_datum ely_stx_datum_create_list(ely_stx_list list)
{
    ely_stx_datum res;
    res.type      = ELY_STX_DATUM_LIST;
    res.data.list = list;
    return res;
}

void ely_stx_datum_destroy(ely_stx_datum* datum)
{
    switch (datum->type)
    {
    case ELY_STX_DATUM_LITERAL:
        ely_stx_literal_destroy(datum->data.lit);
    case ELY_STX_DATUM_LIST:
        ely_stx_list_destroy(datum->data.list);
    case ELY_STX_DATUM_IDENTIFIER:
        ely_stx_identifier_destroy(datum->data.ident);
    default:
        __builtin_unreachable();
    }
}