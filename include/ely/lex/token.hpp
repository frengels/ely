#pragma once

#include <cstdint>
#include <string_view>

#include <fmt/format.h>
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

constexpr bool is_eof(token_type kind)
{
    return kind == token_type::eof;
}

ELY_EXPORT const char* token_type_to_string(ely::token_type ty);

template<typename V>
struct token
{
    using iterator_type = std::ranges::iterator_t<V>;

private:
    token_type    type;  // 4 bytes
    std::uint32_t len;   // 8 bytes
    iterator_type start; // 16 bytes

public:
    token() = default;

    constexpr token(token_type ty, iterator_type start, std::size_t len)
        : type(ty), len(static_cast<std::uint32_t>(len)),
          start(std::move(start))
    {}

    constexpr token_type kind() const
    {
        return type;
    }

    constexpr std::size_t size() const
    {
        return static_cast<std::size_t>(len);
    }

    constexpr bool is_eof() const
    {
        return ely::is_eof(kind());
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

namespace fmt
{
template<>
struct formatter<ely::token_type>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template<typename FmtCtx>
    constexpr auto format(ely::token_type kind, FmtCtx& ctx)
        -> decltype(ctx.out())
    {
        switch (kind)
        {
#define X(t, _)                                                                \
    case ely::token_type::t:                                                   \
        return fmt::format_to(ctx.out(), #t);
#include "token.def"
#undef X
        default:
            return fmt::format_to(ctx.out(), "<unknown>");
        }
    }
};

template<typename I>
struct formatter<ely::token<I>>
{
    template<typename ParseCtx>
    constexpr auto parse(ParseCtx& ctx) -> decltype(ctx.begin())
    {
        return ctx.begin();
    }

    template<typename FmtCtx>
    constexpr auto format(const ely::token<I>& tok, FmtCtx& ctx)
        -> decltype(ctx.out())
    {
        return fmt::format_to(ctx.out(), "({})", tok.kind());
    }
};
} // namespace fmt