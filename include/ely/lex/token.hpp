#pragma once

#include <cstdint>

#include "ely/export.h"
#include "ely/position.hpp"

namespace ely
{
enum struct token_type
{
#define X(val, x) val,
#include "token.def"
#undef X
};

ELY_EXPORT const char* token_type_to_string(ely::token_type ty);

struct token
{
    token_type    type;  // 4 bytes
    uint32_t      len;   // 8 bytes
    const char*   start; // 16 bytes
    ely::position pos;   // 28 bytes
};
} // namespace ely
