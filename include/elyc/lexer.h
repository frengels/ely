#pragma once

#include <stdint.h>

#include "elyc/token.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ElycLexer
{
    const char* __restrict__ src;
    uint32_t len;
    uint32_t pos;
};

uint32_t elyc_lex_src(struct ElycLexer* lex,
                      struct ElycToken* __restrict__ token_buf,
                      uint32_t buf_len);

#ifdef __cplusplus
}
#endif