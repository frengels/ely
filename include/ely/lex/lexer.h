#ifndef ELY_LEX_LEXER_H
#define ELY_LEX_LEXER_H

#include <stdbool.h>
#include <stdint.h>

#include "ely/diagnostic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_token ely_token;

typedef struct
{
    ely_diagnostic_type type;
    const char*         msg;
} ely_lexer_diagnostic;

typedef struct ely_lexer ely_lexer;

ely_lexer* ely_lexer_create(const char* src);
void       ely_lexer_destroy(ely_lexer* lex);

ely_position ely_lexer_position(const ely_lexer* lex);
bool         ely_lexer_empty(const ely_lexer* lex);

uint32_t
ely_lexer_scan_tokens(ely_lexer* lex, ely_token* dst, uint32_t dst_len);

#ifdef __cplusplus
}
#endif

#endif
