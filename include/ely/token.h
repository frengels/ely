#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "ely/defines.h"
#include "ely/export.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ElyTokenKind
{
    ELY_TOKEN_WHITESPACE = 0,
    ELY_TOKEN_TAB,
    ELY_TOKEN_NEWLINE_CR,
    ELY_TOKEN_NEWLINE_LF,
    ELY_TOKEN_NEWLINE_CRLF,
    ELY_TOKEN_COMMENT,

    ELY_TOKEN_EOF,

    ELY_TOKEN_LPAREN,
    ELY_TOKEN_RPAREN,
    ELY_TOKEN_LBRACKET,
    ELY_TOKEN_RBRACKET,
    ELY_TOKEN_LBRACE,
    ELY_TOKEN_RBRACE,

    ELY_TOKEN_ID,

    ELY_TOKEN_INT_LIT,
    ELY_TOKEN_FLOAT_LIT,
    ELY_TOKEN_CHAR_LIT,
    ELY_TOKEN_STRING_LIT,
    ELY_TOKEN_KEYWORD_LIT,
    ELY_TOKEN_TRUE_LIT,
    ELY_TOKEN_FALSE_LIT,

};

typedef struct ElyToken
{
    enum ElyTokenKind kind;
    uint32_t          len;
} ElyToken;

static ELY_ALWAYS_INLINE bool ely_token_is_atmosphere(enum ElyTokenKind kind)
{
    switch (kind)
    {
    case ELY_TOKEN_WHITESPACE:
    case ELY_TOKEN_TAB:
    case ELY_TOKEN_NEWLINE_CR:
    case ELY_TOKEN_NEWLINE_LF:
    case ELY_TOKEN_NEWLINE_CRLF:
    case ELY_TOKEN_COMMENT:
        return true;
    default:
        return false;
    }
}

static ELY_ALWAYS_INLINE bool ely_token_is_eof(enum ElyTokenKind kind)
{
    return kind == ELY_TOKEN_EOF;
}

#ifdef __cplusplus
}
#endif
