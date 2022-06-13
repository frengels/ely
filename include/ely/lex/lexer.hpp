#ifndef ELY_LEX_LEXER_H
#define ELY_LEX_LEXER_H

#include <stdbool.h>
#include <stdint.h>

#include "ely/diagnostic.h"
#include "ely/export.h"
#include "ely/lex/token.hpp"

typedef struct
{
    ely_diagnostic_type type;
    const char*         msg;
} ely_lexer_diagnostic;

typedef struct ely_lexer ely_lexer;

ELY_EXPORT ely_lexer* ely_lexer_create(const char* src);
ELY_EXPORT void       ely_lexer_destroy(ely_lexer* lex);

ELY_EXPORT ely_position ely_lexer_position(const ely_lexer* lex);
ELY_EXPORT bool         ely_lexer_empty(const ely_lexer* lex);
ELY_EXPORT const char*  ely_lexer_cursor(const ely_lexer* lex);

ELY_EXPORT uint32_t ely_lexer_scan_tokens(ely_lexer*  lex,
                                          ely::token* dst,
                                          uint32_t    dst_len);

#endif
