#include "ely/stx/datum.h"

#include <stdlib.h>

#include "ely/stx/identifier.h"
#include "ely/stx/list.h"
#include "ely/stx/literal.h"

ely_datum* ely_datum_create_literal(const ely_literal* lit)
{
    ely_datum* res = malloc(sizeof(ely_datum));
    res->type      = ELY_DATUM_LITERAL;
    res->data.lit  = *lit;
    return res;
}

ely_datum* ely_datum_create_identifier(const ely_identifier* ident)
{
    ely_datum* res  = malloc(sizeof(ely_datum));
    res->type       = ELY_DATUM_IDENTIFIER;
    res->data.ident = *ident;
    return res;
}

ely_datum* ely_datum_create_string_literal(const char*         str,
                                           size_t              len,
                                           const ely_position* pos)
{
    ely_literal lit = ely_literal_create_string(str, len, pos);
    return ely_datum_create_literal(&lit);
}

ely_datum* ely_datum_create_int_literal(const char*         str,
                                        size_t              len,
                                        const ely_position* pos)
{
    ely_literal lit = ely_literal_create_int(str, len, pos);
    return ely_datum_create_literal(&lit);
}

ely_datum* ely_datum_create_dec_literal(const char*         str,
                                        size_t              len,
                                        const ely_position* pos)
{
    ely_literal lit = ely_literal_create_dec(str, len, pos);
    return ely_datum_create_literal(&lit);
}

ely_datum* ely_datum_create_char_literal(const char*         str,
                                         size_t              len,
                                         const ely_position* pos)
{
    ely_literal lit = ely_literal_create_char(str, len, pos);
    return ely_datum_create_literal(&lit);
}

ely_datum* ely_datum_create_bool_literal(bool b, const ely_position* pos)
{
    ely_literal lit = ely_literal_create_bool(b, pos);
    return ely_datum_create_literal(&lit);
}

ely_datum* ely_datum_create_identifier_str(const char*         str,
                                           size_t              len,
                                           const ely_position* pos)
{
    ely_identifier ident = ely_identifier_create(str, len, pos);
    return ely_datum_create_identifier(&ident);
}

void ely_datum_destroy(ely_datum* datum)
{
    switch (datum->type)
    {
    case ELY_DATUM_LITERAL:
        ely_literal_destroy(&datum->data.lit);
        break;
    case ELY_DATUM_IDENTIFIER:
        ely_identifier_destroy(&datum->data.ident);
        break;
    case ELY_DATUM_LIST:
        ely_list_destroy(&datum->data.list);
        break;
    default:
        __builtin_unreachable();
    }
}