#pragma once

#include <cstddef>

#include "ely/export.h"
#include "ely/position.hpp"

extern "C" {
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
    ely::position    pos;
    ely_literal_type type;
    union
    {
        char* str;
        bool  b;
    } data;
} ely_literal;

ELY_EXPORT ely_literal ely_literal_create_int(const char*          str,
                                              size_t               len,
                                              const ely::position& pos);
ELY_EXPORT ely_literal ely_literal_create_dec(const char*          str,
                                              size_t               len,
                                              const ely::position& pos);
ELY_EXPORT ely_literal ely_literal_create_string(const char*          str,
                                                 size_t               len,
                                                 const ely::position& pos);
ELY_EXPORT ely_literal ely_literal_create_char(const char*          str,
                                               size_t               len,
                                               const ely::position& pos);
ELY_EXPORT ely_literal ely_literal_create_bool(bool                 b,
                                               const ely::position& pos);

ELY_EXPORT void ely_literal_destroy(ely_literal* lit);
}