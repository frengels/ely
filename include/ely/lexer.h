#pragma once

#include <stddef.h>
#include <stdint.h>

#include "ely/defines.h"
#include "ely/export.h"

#ifdef __cplusplus
#include "ely/scanner.hpp"
#include <string_view>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ElyToken ElyToken;

typedef struct ElyLexResult
{
    uint32_t bytes_processed;
    uint32_t tokens_read;
} ElyLexResult;

ELY_EXPORT ELY_NODISCARD ElyLexResult ely_lex(const char* __restrict__ src,
                                              size_t src_len,
                                              ElyToken* __restrict__ token_buf,
                                              size_t buf_len);

#ifdef __cplusplus
ELY_EXPORT ELY_NODISCARD ElyLexResult
ely_lex_scanner(const char*                              src,
                size_t                                   src_len,
                ely::Lexeme<std::string_view::iterator>* lexeme_buf,
                size_t                                   buf_len);
#endif

#ifdef __cplusplus
}
#endif
