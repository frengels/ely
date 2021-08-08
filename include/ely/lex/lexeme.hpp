#pragma once

#include <type_traits>

#include "ely/defines.h"
#include "ely/lex/span.hpp"
#include "ely/unchecked.hpp"
#include "ely/variant.hpp"

namespace ely
{
namespace lexeme
{
enum struct TrailingAtmosphere : unsigned char
{
    Whitespace,
    Tab,
    Comment,
};

enum struct LeadingAtmosphere : unsigned char
{
    Whitespace,
    Tab,
    Comment,
    NewlineCr,
    NewlineLf,
    NewlineCrlf,
};

using Atmosphere = LeadingAtmosphere;

ELY_ALWAYS_INLINE constexpr Atmosphere
as_atmosphere(TrailingAtmosphere tatmo) noexcept
{
    switch (tatmo)
    {
    case TrailingAtmosphere::Whitespace:
        return Atmosphere::Whitespace;
    case TrailingAtmosphere::Tab:
        return Atmosphere::Tab;
    case TrailingAtmosphere::Comment:
        return Atmosphere::Comment;
    default:
        __builtin_unreachable();
    }
}

enum struct Literal : unsigned char
{
    IntLit,
    FloatLit,
    CharLit,
    StringLit,
    KeywordLit,
    BoolLit,
    UnterminatedStringLit,
};

enum struct LexemeKind : unsigned char
{
    Eof,
    LParen,
    RParen,
    LBracket,
    RBracket,
    LBrace,
    RBrace,
    Identifier,
    IntLit,
    FloatLit,
    CharLit,
    StringLit,
    KeywordLit,
    BoolLit,
    Colon,
    Quote,
    SyntaxQuote,
    At,
    Unquote,
    SyntaxUnquote,
    UnquoteSplicing,
    SyntaxUnquoteSplicing,
    Exclamation,
    Question,
    Ampersand,
    QuasiQuote,
    QuasiSyntax,
    UnterminatedStringLit,
    InvalidNumberSign,
    Whitespace,
    Tab,
    Comment,
    NewlineCr,
    NewlineLf,
    NewlineCrlf,
};

ELY_ALWAYS_INLINE constexpr bool
is_trailing_atmosphere(LexemeKind kind) noexcept
{
    switch (kind)
    {
    case LexemeKind::Whitespace:
    case LexemeKind::Tab:
    case LexemeKind::Comment:
        return true;
    default:
        return false;
    }
}

ELY_ALWAYS_INLINE constexpr bool is_leading_atmosphere(LexemeKind kind) noexcept
{
    switch (kind)
    {
    case LexemeKind::NewlineCr:
    case LexemeKind::NewlineLf:
    case LexemeKind::NewlineCrlf:
        return true;
    default:
        return is_trailing_atmosphere(kind);
    }
}

ELY_ALWAYS_INLINE constexpr bool is_atmosphere(LexemeKind kind) noexcept
{
    return is_leading_atmosphere(kind);
}

ELY_ALWAYS_INLINE constexpr bool is_literal(LexemeKind kind) noexcept
{
    switch (kind)
    {
    case LexemeKind::IntLit:
    case LexemeKind::FloatLit:
    case LexemeKind::CharLit:
    case LexemeKind::StringLit:
    case LexemeKind::KeywordLit:
    case LexemeKind::BoolLit:
    case LexemeKind::UnterminatedStringLit:
        return true;
    default:
        return false;
    }
}

ELY_ALWAYS_INLINE constexpr bool is_prefix_abbrev(LexemeKind kind) noexcept
{
    switch (kind)
    {
    case LexemeKind::Quote:
    case LexemeKind::SyntaxQuote:
    case LexemeKind::At:
    case LexemeKind::Unquote:
    case LexemeKind::SyntaxUnquote:
    case LexemeKind::UnquoteSplicing:
    case LexemeKind::SyntaxUnquoteSplicing:
    case LexemeKind::Exclamation:
    case LexemeKind::Question:
    case LexemeKind::Ampersand:
    case LexemeKind::QuasiQuote:
    case LexemeKind::QuasiSyntax:
        return true;
    default:
        return false;
    }
}

ELY_ALWAYS_INLINE constexpr bool is_infix_abbrev(LexemeKind kind) noexcept
{
    switch (kind)
    {
    case LexemeKind::Colon:
        return true;
    default:
        return false;
    }
}

ELY_ALWAYS_INLINE constexpr bool is_abbrev(LexemeKind kind) noexcept
{
    return is_prefix_abbrev(kind) || is_infix_abbrev(kind);
}

ELY_ALWAYS_INLINE constexpr TrailingAtmosphere
as_trailing_atmosphere(LexemeKind kind, ely::UncheckedT) noexcept
{
    ELY_ASSERT(is_trailing_atmosphere(kind), "expected trailing atmosphere");
    switch (kind)
    {
    case LexemeKind::Whitespace:
        return TrailingAtmosphere::Whitespace;
    case LexemeKind::Tab:
        return TrailingAtmosphere::Tab;
    case LexemeKind::Comment:
        return TrailingAtmosphere::Comment;
    default:
        __builtin_unreachable();
    }
}

ELY_ALWAYS_INLINE constexpr std::optional<TrailingAtmosphere>
as_trailing_atmosphere(LexemeKind kind) noexcept
{
    if (is_trailing_atmosphere(kind))
    {
        return as_trailing_atmosphere(kind, ely::Unchecked);
    }
    else
    {
        return std::nullopt;
    }
}

ELY_ALWAYS_INLINE constexpr LeadingAtmosphere
as_leading_atmosphere(LexemeKind kind, ely::UncheckedT) noexcept
{
    ELY_ASSERT(is_leading_atmosphere(kind), "expected leading atmosphere");

    switch (kind)
    {
    case LexemeKind::Whitespace:
        return LeadingAtmosphere::Whitespace;
    case LexemeKind::Tab:
        return LeadingAtmosphere::Tab;
    case LexemeKind::Comment:
        return LeadingAtmosphere::Comment;
    case LexemeKind::NewlineCr:
        return LeadingAtmosphere::NewlineCr;
    case LexemeKind::NewlineLf:
        return LeadingAtmosphere::NewlineLf;
    case LexemeKind::NewlineCrlf:
        return LeadingAtmosphere::NewlineCrlf;
    default:
        __builtin_unreachable();
    }
}

ELY_ALWAYS_INLINE constexpr std::optional<LeadingAtmosphere>
as_leading_atmosphere(LexemeKind kind) noexcept
{
    if (is_leading_atmosphere(kind))
    {
        return as_leading_atmosphere(kind, ely::Unchecked);
    }
    else
    {
        return std::nullopt;
    }
}

ELY_ALWAYS_INLINE constexpr Atmosphere as_atmosphere(LexemeKind kind,
                                                     ely::UncheckedT) noexcept
{
    return as_leading_atmosphere(kind, ely::Unchecked);
}

ELY_ALWAYS_INLINE constexpr std::optional<Atmosphere>
as_atmosphere(LexemeKind kind) noexcept
{
    return as_leading_atmosphere(kind);
}
} // namespace lexeme

template<typename I, typename L = lexeme::LexemeKind>
struct Lexeme
{
public:
    using iterator    = I;
    using size_type   = uint32_t;
    using lexeme_type = L;

public:
    LexemeSpan<I> span;
    L             kind{};
};
} // namespace ely