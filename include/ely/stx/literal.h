#ifndef ELY_STX_LITERAL_H
#define ELY_STX_LITERAL_H

#include <stdbool.h>
#include <stddef.h>

#include "ely/export.h"
#include "ely/location.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ely_literal_type
{
    ELY_LITERAL_INT,
    ELY_LITERAL_DEC,
    ELY_LITERAL_STRING,
    ELY_LITERAL_CHAR,
    ELY_LITERAL_BOOL,
} ely_literal_type;

typedef struct ely_literal
{
    ely_position     pos;
    ely_literal_type type;
    union
    {
        char* str;
        bool  b;
    } data;
} ely_literal;

ELY_EXPORT ely_literal ely_literal_create_int(const char*         str,
                                              size_t              len,
                                              const ely_position* pos);
ELY_EXPORT ely_literal ely_literal_create_dec(const char*         str,
                                              size_t              len,
                                              const ely_position* pos);
ELY_EXPORT ely_literal ely_literal_create_string(const char*         str,
                                                 size_t              len,
                                                 const ely_position* pos);
ELY_EXPORT ely_literal ely_literal_create_char(const char*         str,
                                               size_t              len,
                                               const ely_position* pos);
ELY_EXPORT ely_literal ely_literal_create_bool(bool b, const ely_position* pos);

ELY_EXPORT void ely_literal_destroy(ely_literal* lit);

#ifdef __cplusplus
}
#endif

#endif