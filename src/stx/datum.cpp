#include "ely/stx/datum.hpp"

#include <cstdlib>
#include <string_view>

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

ely_datum* ely_datum_create_string_literal(std::string_view str)
{
    return ely_datum_create_literal(ely::make_string_literal(str));
}

ely_datum* ely_datum_create_int_literal(std::string_view str)
{
    return ely_datum_create_literal(ely::make_int_literal(str));
}

ely_datum* ely_datum_create_dec_literal(std::string_view str)
{
    return ely_datum_create_literal(ely::make_decimal_literal(str));
}

ely_datum* ely_datum_create_char_literal(std::string_view str)
{
    return ely_datum_create_literal(ely::make_char_literal(str));
}

ely_datum* ely_datum_create_identifier_str(std::string_view str)
{
    return ely_datum_create_identifier(ely::identifier(str));
}

static inline ely_datum* create_list(ely_list_type ty, llvm::SMRange range)
{
    ely_datum* res = static_cast<ely_datum*>(malloc(sizeof(ely_datum)));
    res->type      = ELY_DATUM_LIST;
    ely_list_init(&res->data.list, ty, range);
    return res;
}

ely_datum* ely_datum_create_parens_list(llvm::SMRange range)
{
    return create_list(ELY_LIST_PARENS, range);
}

ely_datum* ely_datum_create_bracket_list(llvm::SMRange range)
{
    return create_list(ELY_LIST_BRACKET, range);
}

ely_datum* ely_datum_create_brace_list(llvm::SMRange range)
{
    return create_list(ELY_LIST_BRACE, range);
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