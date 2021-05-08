#pragma once

#include <stdint.h>

#include "ely/token.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ElyLexer
{
    const char* __restrict__ src;
    uint32_t len;
    uint32_t pos;
};

uint32_t ely_lex_src(struct ElyLexer* lex,
                      struct ElyToken* __restrict__ token_buf,
                      uint32_t buf_len);

#ifdef __cplusplus
}
#endif