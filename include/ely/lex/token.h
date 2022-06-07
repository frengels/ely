#ifndef ELY_LEX_TOKEN_H
#define ELY_LEX_TOKEN_H

#include <stdint.h>

#include "ely/location.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    ELY_TOKEN_EOF,

    ELY_TOKEN_LPAREN,
    ELY_TOKEN_RPAREN,
    ELY_TOKEN_LBRACKET,
    ELY_TOKEN_RBRACKET,
    ELY_TOKEN_LBRACE,
    ELY_TOKEN_RBRACE,

    ELY_TOKEN_IDENTIFIER,
    ELY_TOKEN_STRING,
    ELY_TOKEN_INT,
    ELY_TOKEN_DEC,
    ELY_TOKEN_CHAR,
    ELY_TOKEN_BOOL,

    ELY_TOKEN_UNKNOWN_CHAR,
    ELY_TOKEN_UNTERMINATED_STRING
} ely_token_type;

const char* ely_token_type_to_string(ely_token_type ty);

typedef struct ely_token
{
    ely_token_type type;  // 4 bytes
    uint32_t       len;   // 8 bytes
    const char*    start; // 16 bytes
    ely_position   pos;   // 28 bytes
} ely_token;

#ifdef __cplusplus
}
#endif

#endif
