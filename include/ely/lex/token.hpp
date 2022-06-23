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

template<typename I>
struct token
{
private:
    token_type    type;  // 4 bytes
    std::uint32_t len;   // 8 bytes
    I             start; // 16 bytes

public:
    token() = default;

    constexpr token(token_type ty, I start, std::size_t len)
        : type(ty), len(static_cast<std::uint32_t>(len)), start(start)
    {}

    constexpr token_type kind() const
    {
        return type;
    }

    constexpr std::size_t size() const
    {
        return static_cast<std::size_t>(len);
    }

    llvm::SMLoc location() const
    {
        return llvm::SMLoc::getFromPointer(std::addressof(*start));
    }

    constexpr std::string_view content() const
    {
        return std::string_view{std::addressof(*start),
                                static_cast<std::size_t>(len)};
    }
};
} // namespace ely
