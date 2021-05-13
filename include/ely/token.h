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
#define X(item) item,
#include "ely/tokens.def"
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

static ELY_ALWAYS_INLINE bool ely_token_is_literal(enum ElyTokenKind kind)
{
    switch (kind)
    {
    case ELY_TOKEN_INT_LIT:
    case ELY_TOKEN_FLOAT_LIT:
    case ELY_TOKEN_CHAR_LIT:
    case ELY_TOKEN_STRING_LIT:
    case ELY_TOKEN_KEYWORD_LIT:
    case ELY_TOKEN_TRUE_LIT:
    case ELY_TOKEN_FALSE_LIT:
        return true;
    default:
        return false;
    }
}

static ELY_ALWAYS_INLINE bool ely_token_is_identifier(enum ElyTokenKind kind)
{
    return kind == ELY_TOKEN_ID;
}

static ELY_ALWAYS_INLINE bool ely_token_is_eof(enum ElyTokenKind kind)
{
    return kind == ELY_TOKEN_EOF;
}

ELY_EXPORT const char* ely_token_as_string(enum ElyTokenKind kind);

#ifdef __cplusplus
}
#endif
