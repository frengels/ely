#pragma once

#include <cstdint>

#include "ely/diagnostic.h"
#include "ely/export.h"
#include "ely/lex/token.hpp"

typedef struct
{
    ely_diagnostic_type type;
    const char*         msg;
} ely_lexer_diagnostic;

namespace ely
{
class ELY_EXPORT lexer
{
public:
    const char* cursor;
    uint32_t    offset;
    uint32_t    line;
    uint32_t    col;

public:
    explicit constexpr lexer(const char* src,
                             uint32_t    offset = 0,
                             uint32_t    line   = 1,
                             uint32_t    col    = 1)
        : cursor(src), offset(offset), line(line), col(col)
    {}

    constexpr ely_position position() const
    {
        return {.offset = offset, .line = line, .col = col};
    }

    constexpr bool empty() const
    {
        return *cursor == '\0';
    }

    uint32_t scan_tokens(ely::token* dst, uint32_t dst_len);
};
} // namespace ely
