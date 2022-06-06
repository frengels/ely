#ifndef ELY_STX_LITERAL_H
#define ELY_STX_LITERAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    ELY_STX_LITERAL_NUMBER,
    ELY_STX_LITERAL_STRING,
    ELY_STX_LITERAL_CHAR,
    ELY_STX_LITERAL_BOOL,
} ely_stx_literal_type;

typedef struct
{
    ely_stx_literal_type type;
    union
    {
        char* str;
        bool  b;
    } data;
} ely_stx_literal;

ely_stx_literal ely_stx_literal_create_number(const char* str, size_t len);

static inline ely_stx_literal ely_stx_literal_create_number(const char* str,
                                                            size_t      len)
{}

static inline void ely_stx_literal_destroy(ely_stx_literal* lit)
{
    switch (lit->type)
    {
    case ELY_STX_LITERAL_NUMBER:
    case ELY_STX_LITERAL_STRING:
    case ELY_STX_LITERAL_CHAR:
        free(lit->data.str);
    default:
    }
}

#ifdef __cplusplus
}
#endif

#endif