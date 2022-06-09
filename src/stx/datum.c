#include "ely/stx/datum.h"

#include "ely/stx/identifier.h"
#include "ely/stx/list.h"
#include "ely/stx/literal.h"

ely_datum* ely_datum_create_literal(const ely_stx_literal* lit)
{
    ely_datum* res = malloc(sizeof(ely_datum));
    res->type      = ELY_DATUM_LITERAL;
    res->data.lit  = *lit;
    return res;
}

ely_datum* ely_datum_create_identifier(const ely_stx_identifier* ident)
{
    ely_datum* res  = malloc(sizeof(ely_datum));
    res->type       = ELY_DATUM_IDENTIFIER;
    res->data.ident = *ident;
    return res;
}

ely_datum* ely_datum_create_list(const ely_stx_list* list)
{
    ely_datum* res = malloc(sizeof(ely_datum));
    res->type      = ELY_DATUM_LIST;
    res->data.list = *list;
}

ely_datum* ely_datum_create_string_literal(const char*         str,
                                           size_t              len,
                                           const ely_position* pos)
{}

ely_datum* ely_datum_create_int_literal(const char*         str,
                                        size_t              len,
                                        const ely_position* pos)
{}

ely_datum* ely_datum_create_dec_literal(const char*         str,
                                        size_t              len,
                                        const ely_position* pos)
{}

ely_datum* ely_datum_create_char_literal(const char*         str,
                                         size_t              len,
                                         const ely_position* pos)
{}

ely_datum* ely_datum_create_bool_literal(bool b, const ely_position* pos)
{}

ely_datum* ely_datum_create_identifier_str(const char*         str,
                                           size_t              len,
                                           const ely_position* pos)
{}

void ely_datum_destroy(ely_datum* datum)
{}