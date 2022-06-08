#ifndef ELY_STX_LITERAL_H
#define ELY_STX_LITERAL_H

#include <stdbool.h>
#include <stddef.h>

#include "ely/export.h"
#include "ely/location.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    ELY_STX_LITERAL_INT,
    ELY_STX_LITERAL_DEC,
    ELY_STX_LITERAL_STRING,
    ELY_STX_LITERAL_CHAR,
    ELY_STX_LITERAL_BOOL,
} ely_stx_literal_type;

typedef struct
{
    ely_position         pos;
    ely_stx_literal_type type;
    union
    {
        char* str;
        bool  b;
    } data;
} ely_stx_literal;

ELY_EXPORT ely_stx_literal ely_stx_literal_create_int(const char*         str,
                                                      size_t              len,
                                                      const ely_position* pos);
ELY_EXPORT ely_stx_literal ely_stx_literal_create_dec(const char*         str,
                                                      size_t              len,
                                                      const ely_position* pos);
ELY_EXPORT ely_stx_literal
                           ely_stx_literal_create_string(const char*         str,
                                                         size_t              len,
                                                         const ely_position* pos);
ELY_EXPORT ely_stx_literal ely_stx_literal_create_char(const char*         str,
                                                       size_t              len,
                                                       const ely_position* pos);
ELY_EXPORT ely_stx_literal ely_stx_literal_create_bool(bool                b,
                                                       const ely_position* pos);

ELY_EXPORT void ely_stx_literal_destroy(ely_stx_literal* lit);

#ifdef __cplusplus
}
#endif

#endif