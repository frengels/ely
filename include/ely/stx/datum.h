#ifndef ELY_STX_DATUM_H
#define ELY_STX_DATUM_H

#include <stddef.h>

#include "ely/export.h"
#include "ely/ilist.h"
#include "ely/location.h"
#include "ely/stx/identifier.h"
#include "ely/stx/list.h"
#include "ely/stx/literal.h"

#ifdef __cplusplus
extern "C" {
#endif

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
        ely_literal    lit;
        ely_list       list;
        ely_identifier ident;
    } data;
} ely_datum;

ELY_EXPORT ely_datum* ely_datum_create_literal(const ely_literal* lit);
ELY_EXPORT ely_datum* ely_datum_create_identifier(const ely_identifier* ident);

ELY_EXPORT ely_datum* ely_datum_create_string_literal(const char*         str,
                                                      size_t              len,
                                                      const ely_position* pos);
ELY_EXPORT ely_datum* ely_datum_create_int_literal(const char*         str,
                                                   size_t              len,
                                                   const ely_position* pos);
ELY_EXPORT ely_datum* ely_datum_create_dec_literal(const char*         str,
                                                   size_t              len,
                                                   const ely_position* pos);
ELY_EXPORT ely_datum* ely_datum_create_char_literal(const char*         str,
                                                    size_t              len,
                                                    const ely_position* pos);
ELY_EXPORT ely_datum* ely_datum_create_bool_literal(bool                b,
                                                    const ely_position* pos);

ELY_EXPORT ely_datum* ely_datum_create_identifier_str(const char*         str,
                                                      size_t              len,
                                                      const ely_position* pos);

ELY_EXPORT ely_datum* ely_datum_create_parens_list(const ely_position* pos);
ELY_EXPORT ely_datum* ely_datum_create_bracket_list(const ely_position* pos);
ELY_EXPORT ely_datum* ely_datum_create_brace_list(const ely_position* pos);

ELY_EXPORT void ely_datum_destroy(ely_datum* datum);

#ifdef __cplusplus
}
#endif

#endif