#ifndef ELY_STX_IDENTIFIER_H
#define ELY_STX_IDENTIFIER_H

#include <stdbool.h>
#include <stddef.h>

#include "ely/export.h"
#include "ely/position.hpp"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_identifier
{
    ely::position pos;
    char*         str;
    bool          is_literal;
} ely_identifier;

ELY_EXPORT ely_identifier ely_identifier_create(const char*         str,
                                                size_t              len,
                                                const ely::position& pos);
ELY_EXPORT void           ely_identifier_destroy(ely_identifier* ident);

#ifdef __cplusplus
}
#endif

#endif