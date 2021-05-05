#pragma once

#include <stdint.h>

#include "elyc/token.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ElycLexer
{};

uint32_t elyc_lex_src(struct ElycLexer* lex,
                  const char* __restrict__ src,
                  uint32_t src_len,
                  struct ElycToken* __restrict__ token_buf,
                  uint32_t buf_len);

#ifdef __cplusplus
}
#endif