#pragma once

#include <cstdint>
#include <string_view>

#include <llvm/Support/SMLoc.h>

#include "ely/export.h"

namespace ely
{
enum struct token_type : unsigned char
{
#define X(val, x) val,
#include "token.def"
#undef X
};

ELY_EXPORT const char* token_type_to_string(ely::token_type ty);

struct token
{
private:
    token_type    type;  // 4 bytes
    std::uint32_t len;   // 8 bytes
    const char*   start; // 16 bytes

public:
    token() = default;

    constexpr token(token_type ty, const char* start, std::uint32_t len)
        : type(ty), start(start), len(len)
    {}

    constexpr token_type ty() const
    {
        return type;
    }

    constexpr std::size_t size() const
    {
        return static_cast<std::size_t>(len);
    }

    llvm::SMLoc location() const
    {
        return llvm::SMLoc::getFromPointer(start);
    }

    std::string_view content() const
    {
        return std::string_view{start, static_cast<std::size_t>(len)};
    }
};
} // namespace ely
