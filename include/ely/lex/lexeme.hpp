#pragma once

#include <type_traits>

#include "ely/defines.h"
#include "ely/lex/span.hpp"

namespace ely
{
enum class LexemeKind : unsigned char
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
lexeme_kind_is_trailing_atmosphere(LexemeKind kind) noexcept
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

ELY_ALWAYS_INLINE constexpr bool
lexeme_kind_is_leading_atmosphere(LexemeKind kind) noexcept
{
    switch (kind)
    {
    case LexemeKind::NewlineCr:
    case LexemeKind::NewlineLf:
    case LexemeKind::NewlineCrlf:
        return true;
    default:
        ELY_MUSTTAIL return lexeme_kind_is_trailing_atmosphere(kind);
    }
}

ELY_ALWAYS_INLINE constexpr bool
lexeme_kind_is_atmosphere(LexemeKind kind) noexcept
{
    ELY_MUSTTAIL return lexeme_kind_is_trailing_atmosphere(kind);
}

ELY_ALWAYS_INLINE constexpr bool
lexeme_kind_is_literal(LexemeKind kind) noexcept
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

ELY_ALWAYS_INLINE constexpr bool
lexeme_kind_is_prefix_abbrev(LexemeKind kind) noexcept
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

ELY_ALWAYS_INLINE constexpr bool
lexeme_kind_is_infix_abbrev(LexemeKind kind) noexcept
{
    switch (kind)
    {
    case LexemeKind::Colon:
        return true;
    default:
        return false;
    }
}

ELY_ALWAYS_INLINE constexpr bool lexeme_kind_is_abbrev(LexemeKind kind) noexcept
{
    return lexeme_kind_is_prefix_abbrev(kind) ||
           lexeme_kind_is_infix_abbrev(kind);
}

ELY_ALWAYS_INLINE constexpr bool lexeme_kind_is_poison(LexemeKind kind) noexcept
{
    switch (kind)
    {
    case LexemeKind::UnterminatedStringLit:
    case LexemeKind::InvalidNumberSign:
        return true;
    default:
        return false;
    }
}

template<typename I>
struct Lexeme
{
public:
    using iterator  = I;
    using size_type = uint32_t;

public:
    LexemeSpan<I> span;
    LexemeKind    kind{LexemeKind::Eof};

    ELY_ALWAYS_INLINE explicit constexpr operator bool() const noexcept
    {
        return kind != LexemeKind::Eof;
    }
};
} // namespace ely