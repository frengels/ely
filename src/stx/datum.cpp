#include "ely/stx/datum.hpp"

#include <cstdlib>

#include "ely/stx/identifier.hpp"
#include "ely/stx/list.hpp"
#include "ely/stx/literal.hpp"

ely_datum* ely_datum_create_literal(const ely_literal* lit)
{
    ely_datum* res = static_cast<ely_datum*>(malloc(sizeof(ely_datum)));
    res->type      = ELY_DATUM_LITERAL;
    res->data.lit  = *lit;
    return res;
}

ely_datum* ely_datum_create_identifier(const ely_identifier* ident)
{
    ely_datum* res  = static_cast<ely_datum*>(malloc(sizeof(ely_datum)));
    res->type       = ELY_DATUM_IDENTIFIER;
    res->data.ident = *ident;
    return res;
}

ely_datum* ely_datum_create_string_literal(const char*          str,
                                           size_t               len,
                                           const ely::position& pos)
{
    ely_literal lit = ely_literal_create_string(str, len, pos);
    return ely_datum_create_literal(&lit);
}

ely_datum* ely_datum_create_int_literal(const char*          str,
                                        size_t               len,
                                        const ely::position& pos)
{
    ely_literal lit = ely_literal_create_int(str, len, pos);
    return ely_datum_create_literal(&lit);
}

ely_datum* ely_datum_create_dec_literal(const char*          str,
                                        size_t               len,
                                        const ely::position& pos)
{
    ely_literal lit = ely_literal_create_dec(str, len, pos);
    return ely_datum_create_literal(&lit);
}

ely_datum* ely_datum_create_char_literal(const char*          str,
                                         size_t               len,
                                         const ely::position& pos)
{
    ely_literal lit = ely_literal_create_char(str, len, pos);
    return ely_datum_create_literal(&lit);
}

ely_datum* ely_datum_create_bool_literal(bool b, const ely::position& pos)
{
    ely_literal lit = ely_literal_create_bool(b, pos);
    return ely_datum_create_literal(&lit);
}

ely_datum* ely_datum_create_identifier_str(const char*          str,
                                           size_t               len,
                                           const ely::position& pos)
{
    ely_identifier ident = ely_identifier_create(str, len, pos);
    return ely_datum_create_identifier(&ident);
}

static inline ely_datum* create_list(ely_list_type ty, const ely::position& pos)
{
    ely_datum* res      = static_cast<ely_datum*>(malloc(sizeof(ely_datum)));
    res->type           = ELY_DATUM_LIST;
    res->data.list.type = ty;
    res->data.list.pos  = pos;
    ely_ilist_init(&res->data.list.head);
    return res;
}

ely_datum* ely_datum_create_parens_list(const ely::position& pos)
{
    return create_list(ELY_LIST_PARENS, pos);
}

ely_datum* ely_datum_create_bracket_list(const ely::position& pos)
{
    return create_list(ELY_LIST_BRACKET, pos);
}

ely_datum* ely_datum_create_brace_list(const ely::position& pos)
{
    return create_list(ELY_LIST_BRACE, pos);
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