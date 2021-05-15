#pragma once

#include <stdint.h>

#include "ely/export.h"
#include "ely/token.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ElyBuffer ElyBuffer;

typedef struct ElyLexer
{
    const char* __restrict__ src;
    uint32_t len;
    uint32_t pos;
} ElyLexer;

ELY_EXPORT void
ely_lexer_create(ElyLexer* lex, const char* __restrict__ src, uint32_t len);

ELY_EXPORT uint32_t ely_lexer_lex(ElyLexer* lex,
                                  ElyToken* __restrict__ token_buf,
                                  uint32_t buf_len);

#ifdef __cplusplus
}
#endif
