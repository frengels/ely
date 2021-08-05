#pragma once

#include <type_traits>

#include "ely/defines.h"
#include "ely/lex/span.hpp"
#include "ely/variant.hpp"

namespace ely
{
namespace lexeme
{
struct Eof
{};
struct LParen
{};
struct RParen
{};
struct LBracket
{};
struct RBracket
{};
struct LBrace
{};
struct RBrace
{};
struct Identifier
{};
struct IntLit
{};
struct FloatLit
{};
struct CharLit
{};
struct StringLit
{};
struct KeywordLit
{};
struct BoolLit
{};
struct Colon
{};
struct Quote
{};
struct SyntaxQuote
{};
struct At
{};
struct Unquote
{};
struct SyntaxUnquote
{};
struct UnquoteSplicing
{};
struct SyntaxUnquoteSplicing
{};
struct Exclamation
{};
struct Question
{};
struct Ampersand
{};
struct QuasiQuote
{};
struct QuasiSyntax
{};
struct UnterminatedStringLit
{};
struct InvalidNumberSign
{};
struct Whitespace
{};
struct Tab
{};
struct Comment
{};
struct NewlineCr
{};
struct NewlineLf
{};
struct NewlineCrlf
{};

using variant_type = ely::Variant<Eof,
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
                                  NewlineCrlf>;
} // namespace lexeme

class LexemeKind : public lexeme::variant_type
{
    using base_ = lexeme::variant_type;

public:
    using base_::base_;

    ELY_ALWAYS_INLINE constexpr bool is_trailing_atmosphere() const noexcept
    {
        return ely::visit<bool>(
            [](const auto& x) {
                using ty = ely::remove_cvref_t<decltype(x)>;

                return ely::is_same_one_of_v<ty,
                                             lexeme::Whitespace,
                                             lexeme::Tab,
                                             lexeme::Comment>;
            },
            *this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_leading_atmosphere() const noexcept
    {
        return ely::visit<bool>(
                   [](const auto& x) {
                       using ty = ely::remove_cvref_t<decltype(x)>;
                       return ely::is_same_one_of_v<ty,
                                                    lexeme::NewlineCr,
                                                    lexeme::NewlineLf,
                                                    lexeme::NewlineCrlf>;
                   },
                   *this) ||
               is_trailing_atmosphere();
    }

    ELY_ALWAYS_INLINE constexpr bool is_atmosphere() const noexcept
    {
        return is_trailing_atmosphere();
    }

    ELY_ALWAYS_INLINE constexpr bool is_literal() const noexcept
    {
        return ely::visit<bool>(
            [](const auto& x) {
                using ty = ely::remove_cvref_t<decltype(x)>;

                return ely::is_same_one_of_v<ty,
                                             lexeme::IntLit,
                                             lexeme::FloatLit,
                                             lexeme::CharLit,
                                             lexeme::StringLit,
                                             lexeme::KeywordLit,
                                             lexeme::BoolLit,
                                             lexeme::UnterminatedStringLit>;
            },
            *this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_prefix_abbrev() const noexcept
    {
        return ely::visit<bool>(
            [](const auto& x) {
                using ty = ely::remove_cvref_t<decltype(x)>;

                return ely::is_same_one_of_v<ty,
                                             lexeme::Quote,
                                             lexeme::SyntaxQuote,
                                             lexeme::At,
                                             lexeme::Unquote,
                                             lexeme::SyntaxUnquote,
                                             lexeme::UnquoteSplicing,
                                             lexeme::SyntaxUnquoteSplicing,
                                             lexeme::Exclamation,
                                             lexeme::Question,
                                             lexeme::Ampersand,
                                             lexeme::QuasiQuote,
                                             lexeme::QuasiSyntax>;
            },
            *this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_infix_abbrev() const noexcept
    {
        return ely::holds_alternative<lexeme::Colon>(*this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_abbrev() const noexcept
    {
        return is_prefix_abbrev() || is_infix_abbrev();
    }

    ELY_ALWAYS_INLINE constexpr bool is_poison() const noexcept
    {
        return ely::visit<bool>(
            [](const auto& x) {
                using ty = ely::remove_cvref_t<decltype(x)>;
                return ely::is_same_one_of_v<ty,
                                             lexeme::UnterminatedStringLit,
                                             lexeme::InvalidNumberSign>;
            },
            *this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_eof() const noexcept
    {
        return ely::holds_alternative<lexeme::Eof>(*this);
    }
};

template<typename I>
struct Lexeme
{
public:
    using iterator  = I;
    using size_type = uint32_t;

public:
    LexemeSpan<I> span;
    LexemeKind    kind{lexeme::Eof{}};

    ELY_ALWAYS_INLINE explicit constexpr operator bool() const noexcept
    {
        return !kind.is_eof();
    }
};
} // namespace ely