#pragma once

#include <cstdint>

#include <llvm/Support/SMLoc.h>

#include "ely/export.h"
#include "ely/lex/token.hpp"
#include "ely/position.hpp"

namespace ely
{
class ELY_EXPORT lexer
{
public:
    const char* cursor;

public:
    explicit constexpr lexer(const char* src) : cursor(src)
    {}

    llvm::SMLoc location() const
    {
        return llvm::SMLoc::getFromPointer(cursor);
    }

    constexpr bool empty() const
    {
        return *cursor == '\0';
    }

    uint32_t scan_tokens(ely::token* dst, uint32_t dst_len);
};
} // namespace ely
