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
ELY_NODISCARD ELY_NOINLINE ElyLexResult
ely_lex_scanner(std::string_view                         src,
                ely::Lexeme<std::string_view::iterator>* lexeme_buf,
                size_t                                   buf_len)
{
    auto scanner = ely::Scanner{src.begin(), src.end()};

    auto   scanner_it = scanner.begin();
    size_t buf_i      = 0;
    for (; buf_i != buf_len; ++buf_i)
    {
        if (scanner_it == scanner.end())
        {
            break;
        }

        lexeme_buf[buf_i] = *scanner_it++;
    }

    ElyLexResult res{static_cast<uint32_t>(scanner_it.base() - src.begin()),
                     static_cast<uint32_t>(buf_i)};

    return res;
}
#endif

#ifdef __cplusplus
}
#endif
