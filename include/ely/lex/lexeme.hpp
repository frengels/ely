#pragma once

#include <type_traits>

#include "ely/lex/lexemes.hpp"
#include "ely/lex/tokens.hpp"

namespace ely
{
class LexemeKind : public lexeme::variant_type
{
    using base_ = lexeme::variant_type;

public:
    using base_::base_;

    constexpr bool is_eof() const noexcept
    {
        return !ely::holds_alternative<std::in_place_type_t<token2::Eof>>(
            *this);
    }
};

static_assert(std::is_trivially_destructible_v<LexemeKind>);
static_assert(std::is_trivially_copy_constructible_v<LexemeKind>);
static_assert(
    std::is_default_constructible_v<LexemeKind>); // not trivial since index
                                                  // needs to be initialized too
static_assert(sizeof(LexemeKind) == 1);

template<typename I>
struct Lexeme
{
public:
    using iterator  = I;
    using size_type = uint32_t;

public:
    LexemeSpan<I> span;
    LexemeKind    kind{std::in_place_type<token2::Eof>};

    explicit constexpr operator bool() const noexcept
    {
        return !kind.is_eof();
    }
};
} // namespace ely