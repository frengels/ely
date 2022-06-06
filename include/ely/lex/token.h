#ifndef ELY_LEX_TOKEN_H
#define ELY_LEX_TOKEN_H

#include <stdint.h>

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
} ely_token_type;

typedef struct
{
    uint32_t line;
    uint32_t column;
} ely_token_position;

typedef struct ely_token
{
    ely_token_type type; // 4 bytes
                         /* uint32_t           len;   // 8 bytes
                          const char*        start; // 16 bytes
                          ely_token_position pos;   // 24 bytes */
} ely_token;

#ifdef __cplusplus
}
#endif

#endif
