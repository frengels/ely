#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum ElycTokenKind
{
    ELY_TOKEN_EOF = 0,

    ELY_TOKEN_WHITESPACE,
    ELY_TOKEN_TAB,
    ELY_TOKEN_NEWLINE_CR,
    ELY_TOKEN_NEWLINE_LF,
    ELY_TOKEN_NEWLINE_CRLF,
    ELY_TOKEN_COMMENT,

    ELY_TOKEN_LPAREN,
    ELY_TOKEN_RPAREN,
    ELY_TOKEN_LBRACKET,
    ELY_TOKEN_RBRACKET,
    ELY_TOKEN_LBRACE,
    ELY_TOKEN_RBRACE,

    ELY_TOKEN_ID,

    ELY_TOKEN_INT_LIT,
    ELY_TOKEN_FLOAT_LIT,
    ELY_TOKEN_STRING_LIT,
    ELY_TOKEN_KEYWORD_LIT,
    ELY_TOKEN_TRUE_LIT,
    ELY_TOKEN_FALSE_LIT,

};

struct ElycToken
{
    enum ElycTokenKind kind;
    uint32_t           len;
};

#ifdef __cplusplus
}
#endif