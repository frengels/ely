#pragma once

#include <string_view>

// all of the location, position, range follow the names used by the LSP spec

namespace ely
{
namespace stx
{
struct Position
{
    uint32_t line{};
    uint32_t character{};

    constexpr Position& next_line() noexcept
    {
        ++line;
        character = 0;

        return *this;
    }

    constexpr Position& next() noexcept
    {
        ++character;

        return *this;
    }
};

struct Range
{
    Position start;
    Position end;
};

struct Location
{
    std::string_view uri;
    Range            range;
};
} // namespace stx
} // namespace ely