#pragma once

#include <cstddef>

#include "ely/export.h"
#include "ely/ilist.h"
#include "ely/position.hpp"
#include "ely/stx/identifier.hpp"
#include "ely/stx/list.hpp"
#include "ely/stx/literal.hpp"

extern "C" {
typedef enum ely_datum_type
{
    ELY_DATUM_LITERAL,
    ELY_DATUM_LIST,
    ELY_DATUM_IDENTIFIER
} ely_datum_type;

typedef struct ely_datum
{
    ely_ilist      link;
    ely_datum_type type;
    union
    {
        ely::literal    lit;
        ely_list        list;
        ely::identifier ident;
    } data;
} ely_datum;

ELY_EXPORT ely_datum* ely_datum_create_literal(ely::literal lit);
ELY_EXPORT ely_datum* ely_datum_create_identifier(ely::identifier ident);

ELY_EXPORT ely_datum* ely_datum_create_string_literal(std::string          str,
                                                      const ely::position& pos);
ELY_EXPORT ely_datum* ely_datum_create_int_literal(std::string          str,
                                                   const ely::position& pos);
ELY_EXPORT ely_datum* ely_datum_create_dec_literal(std::string          str,
                                                   const ely::position& pos);
ELY_EXPORT ely_datum* ely_datum_create_char_literal(std::string          str,
                                                    const ely::position& pos);

ELY_EXPORT ely_datum* ely_datum_create_identifier_str(std::string          str,
                                                      const ely::position& pos);

ELY_EXPORT ely_datum* ely_datum_create_parens_list(const ely::position& pos);
ELY_EXPORT ely_datum* ely_datum_create_bracket_list(const ely::position& pos);
ELY_EXPORT ely_datum* ely_datum_create_brace_list(const ely::position& pos);

ELY_EXPORT void ely_datum_destroy(ely_datum* datum);
}
