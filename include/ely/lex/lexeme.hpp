#pragma once

#include <functional>
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

enum struct PrefixAbbrev : unsigned char
{
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
    QuasiSyntax
};

enum struct InfixAbbrev : unsigned char
{
    Colon
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

template<typename E>
ELY_ALWAYS_INLINE constexpr bool is_trailing_atmosphere(E kind) noexcept
{
    switch (kind)
    {
    case E::Whitespace:
    case E::Tab:
    case E::Comment:
        return true;
    default:
        return false;
    }
}

template<typename E>
ELY_ALWAYS_INLINE constexpr bool is_leading_atmosphere(E kind) noexcept
{
    switch (kind)
    {
    case E::NewlineCr:
    case E::NewlineLf:
    case E::NewlineCrlf:
        return true;
    default:
        return is_trailing_atmosphere(kind);
    }
}

template<typename E>
ELY_ALWAYS_INLINE constexpr bool is_atmosphere(E kind) noexcept
{
    return is_leading_atmosphere(kind);
}

template<typename E>
ELY_ALWAYS_INLINE constexpr bool is_literal(E kind) noexcept
{
    switch (kind)
    {
    case E::IntLit:
    case E::FloatLit:
    case E::CharLit:
    case E::StringLit:
    case E::KeywordLit:
    case E::BoolLit:
    case E::UnterminatedStringLit:
        return true;
    default:
        return false;
    }
}

template<typename E>
ELY_ALWAYS_INLINE constexpr bool is_prefix_abbrev(E kind) noexcept
{
    switch (kind)
    {
    case E::Quote:
    case E::SyntaxQuote:
    case E::At:
    case E::Unquote:
    case E::SyntaxUnquote:
    case E::UnquoteSplicing:
    case E::SyntaxUnquoteSplicing:
    case E::Exclamation:
    case E::Question:
    case E::Ampersand:
    case E::QuasiQuote:
    case E::QuasiSyntax:
        return true;
    default:
        return false;
    }
}

template<typename E>
ELY_ALWAYS_INLINE constexpr bool is_infix_abbrev(E lex) noexcept
{
    switch (lex)
    {
    case E::Colon:
        return true;
    default:
        return false;
    }
}

ELY_ALWAYS_INLINE constexpr bool is_abbrev(LexemeKind kind) noexcept
{
    return is_prefix_abbrev(kind) || is_infix_abbrev(kind);
}

template<typename To = TrailingAtmosphere, typename From>
ELY_ALWAYS_INLINE constexpr To as_trailing_atmosphere(From lex,
                                                      ely::UncheckedT) noexcept
{
    ELY_ASSERT(is_trailing_atmosphere(lex), "expected trailing atmosphere");

    switch (lex)
    {
    case From::Whitespace:
        return To::Whitespace;
    case From::Tab:
        return To::Tab;
    case From::Comment:
        return To::Comment;
    default:
        __builtin_unreachable();
    }
}

template<typename To = TrailingAtmosphere, typename From>
ELY_ALWAYS_INLINE constexpr std::optional<To>
as_trailing_atmosphere(From lex) noexcept
{
    if (is_trailing_atmosphere(lex))
    {
        return as_trailing_atmosphere(lex, ely::Unchecked);
    }
    else
    {
        return std::nullopt;
    }
}

template<typename To = LeadingAtmosphere, typename From>
ELY_ALWAYS_INLINE constexpr To as_leading_atmosphere(From lex,
                                                     ely::UncheckedT) noexcept
{
    ELY_ASSERT(is_leading_atmosphere(lex), "expected leading atmosphere");

    switch (lex)
    {
    case From::Whitespace:
        return To::Whitespace;
    case From::Tab:
        return To::Tab;
    case From::Comment:
        return To::Comment;
    case From::NewlineCr:
        return To::NewlineCr;
    case From::NewlineLf:
        return To::NewlineLf;
    case From::NewlineCrlf:
        return To::NewlineCrlf;
    default:
        __builtin_unreachable();
    }
}

template<typename To = LeadingAtmosphere, typename From>
ELY_ALWAYS_INLINE constexpr std::optional<To>
as_leading_atmosphere(From lex) noexcept
{
    if (is_leading_atmosphere(lex))
    {
        return as_leading_atmosphere<To>(lex, ely::Unchecked);
    }
    else
    {
        return std::nullopt;
    }
}

template<typename To = Atmosphere, typename From>
ELY_ALWAYS_INLINE constexpr Atmosphere as_atmosphere(From lex,
                                                     ely::UncheckedT) noexcept
{
    return as_leading_atmosphere<To>(lex, ely::Unchecked);
}

template<typename To = Atmosphere, typename From>
ELY_ALWAYS_INLINE constexpr std::optional<To> as_atmosphere(From lex) noexcept
{
    return as_leading_atmosphere<To>(lex);
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

    template<typename F>
    constexpr Lexeme<I, std::invoke_result_t<F, L>>
    transform(F&& fn) const noexcept
    {
        return {span, std::invoke(static_cast<F&&>(fn), span)};
    }
};
} // namespace ely