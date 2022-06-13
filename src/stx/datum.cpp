#include "ely/stx/datum.hpp"

#include <cstdlib>

#include "ely/stx/identifier.hpp"
#include "ely/stx/list.hpp"
#include "ely/stx/literal.hpp"

ely_datum* ely_datum_create_literal(ely::literal lit)
{
    ely_datum* res = static_cast<ely_datum*>(malloc(sizeof(ely_datum)));
    res->type      = ELY_DATUM_LITERAL;
    new (&res->data.lit) ely::literal(std::move(lit));
    return res;
}

ely_datum* ely_datum_create_identifier(ely::identifier ident)
{
    ely_datum* res = static_cast<ely_datum*>(malloc(sizeof(ely_datum)));
    res->type      = ELY_DATUM_IDENTIFIER;

    new (&res->data.ident) ely::identifier(std::move(ident));
    return res;
}

ely_datum* ely_datum_create_string_literal(std::string          str,
                                           const ely::position& pos)
{
    return ely_datum_create_literal(
        ely::make_string_literal(std::move(str), pos));
}

ely_datum* ely_datum_create_int_literal(std::string          str,
                                        const ely::position& pos)
{
    return ely_datum_create_literal(ely::make_int_literal(std::move(str), pos));
}

ely_datum* ely_datum_create_dec_literal(std::string          str,
                                        const ely::position& pos)
{
    return ely_datum_create_literal(
        ely::make_decimal_literal(std::move(str), pos));
}

ely_datum* ely_datum_create_char_literal(std::string          str,
                                         const ely::position& pos)
{
    return ely_datum_create_literal(
        ely::make_char_literal(std::move(str), pos));
}

ely_datum* ely_datum_create_identifier_str(std::string          str,
                                           const ely::position& pos)
{
    return ely_datum_create_identifier(ely::identifier(std::move(str), pos));
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
        datum->data.lit.~literal();
        break;
    case ELY_DATUM_IDENTIFIER:
        datum->data.ident.~identifier();
        break;
    case ELY_DATUM_LIST:
        ely_list_destroy(&datum->data.list);
        break;
    default:
        __builtin_unreachable();
    }
}