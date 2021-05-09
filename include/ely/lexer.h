#pragma once

#include <stdint.h>

#include "ely/export.h"
#include "ely/token.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ElyLexer
{
    const char* __restrict__ src;
    uint32_t len;
    uint32_t pos;
} ElyLexer;

ELY_EXPORT void
ely_lex_create(ElyLexer* lex, const char* __restrict__ src, uint32_t len);

ELY_EXPORT uint32_t ely_lex_src(ElyLexer* lex,
                                ElyToken* __restrict__ token_buf,
                                uint32_t buf_len);

#ifdef __cplusplus
}
#endif
