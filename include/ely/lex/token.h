#ifndef ELY_LEX_TOKEN_H
#define ELY_LEX_TOKEN_H

#include <stdint.h>
#include <stdio.h>

#include "ely/location.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ely_token_type
{
#define X(val, x) val,
#include "token.def"
#undef X
} ely_token_type;

const char* ely_token_type_to_string(ely_token_type ty);

typedef struct ely_token
{
    ely_token_type type;  // 4 bytes
    uint32_t       len;   // 8 bytes
    const char*    start; // 16 bytes
    ely_position   pos;   // 28 bytes
} ely_token;

int ely_token_print(FILE* f, const ely_token* t);

#ifdef __cplusplus
}
#endif

#endif
