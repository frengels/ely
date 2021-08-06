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

template<typename... Ts>
class LexemeBase : public ely::Variant<Ts...>
{
private:
    using base_ = ely::Variant<Ts...>;

public:
    using base_::base_;

    ELY_ALWAYS_INLINE explicit constexpr operator bool() const noexcept
    {
        return !ely::holds_alternative<Eof>(*this);
    }
};

using All = LexemeBase<Eof,
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

class TrailingAtmosphere : public LexemeBase<Whitespace, Tab, Comment>
{
    using base_ = LexemeBase<Whitespace, Tab, Comment>;

public:
    using base_::base_;
};

class LeadingAtmosphere : public LexemeBase<Whitespace,
                                            Tab,
                                            Comment,
                                            NewlineCr,
                                            NewlineLf,
                                            NewlineCrlf>
{
    using base_ =
        LexemeBase<Whitespace, Tab, Comment, NewlineCr, NewlineLf, NewlineCrlf>;

public:
    using base_::base_;
};

using Atmosphere = LeadingAtmosphere;

class Literal : public LexemeBase<IntLit,
                                  FloatLit,
                                  CharLit,
                                  StringLit,
                                  KeywordLit,
                                  BoolLit,
                                  UnterminatedStringLit>
{
    using base_ = LexemeBase<IntLit,
                             FloatLit,
                             CharLit,
                             StringLit,
                             KeywordLit,
                             BoolLit,
                             UnterminatedStringLit>;

public:
    using base_::base_;
};

class PrefixAbbrev : public LexemeBase<Quote,
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
                                       QuasiSyntax>
{
    using base_ = LexemeBase<Quote,
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
                             QuasiSyntax>;

public:
    using base_::base_;
};

using InfixAbbrev = Colon;

class Abbrev : public LexemeBase<Quote,
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
                                 Colon>
{
    using base_ = LexemeBase<Quote,
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
                             Colon>;

public:
    using base_::base_;
};
} // namespace lexeme

namespace detail
{
template<typename Variant, typename T>
inline constexpr bool variant_contains_v =
    typename Variant::value_types::template apply_all<
        ely::is_same_mf<T>::template invoke>::value;

template<typename VRes, typename VT>
ELY_ALWAYS_INLINE constexpr VRes convert_variant_unchecked(VT&& v)
{
    ely::visit(
        [](auto&& x) -> VRes {
            // this will only work on non cvref qualified members
            using ty = ely::remove_cvref_t<decltype(x)>;
            if constexpr (variant_contains_v<VRes, ty>)
            {
                return VRes{static_cast<decltype(x)&&>(x)};
            }
            else
            {
                __builtin_unreachable();
            }
        },
        static_cast<VT&&>(v));
}

template<typename OtherV, typename ThisV>
ELY_ALWAYS_INLINE constexpr bool
holds_one_of_other_alternatives(const ThisV& v) noexcept
{
    return ely::visit<bool>(
        [](auto&& x) {
            using ty = ely::remove_cvref_t<decltype(x)>;

            return typename OtherV::value_types::template apply_all<
                ely::is_same_mf<ty>::template invoke>::value;
        },
        v);
}
} // namespace detail

class LexemeKind : public lexeme::All
{
    using base_ = lexeme::All;

public:
    using base_::base_;

    ELY_ALWAYS_INLINE constexpr bool is_trailing_atmosphere() const noexcept
    {
        return detail::holds_one_of_other_alternatives<
            lexeme::TrailingAtmosphere>(*this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_leading_atmosphere() const noexcept
    {
        return detail::holds_one_of_other_alternatives<
            lexeme::LeadingAtmosphere>(*this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_atmosphere() const noexcept
    {
        return is_leading_atmosphere();
    }

    ELY_ALWAYS_INLINE constexpr bool is_literal() const noexcept
    {
        return detail::holds_one_of_other_alternatives<lexeme::Literal>(*this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_prefix_abbrev() const noexcept
    {
        return detail::holds_one_of_other_alternatives<lexeme::PrefixAbbrev>(
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
        return detail::holds_one_of_other_alternatives<lexeme::Poison>(*this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_eof() const noexcept
    {
        return ely::holds_alternative<lexeme::Eof>(*this);
    }

    ELY_ALWAYS_INLINE constexpr lexeme::TrailingAtmosphere
    as_trailing_atmosphere_unchecked() const noexcept
    {
            ELY_ASSERT(is_trailing_atmosphere(), "expected trailing
       atmosphere"); return
       detail::convert_variant_unchecked<lexeme::TrailingAtmosphere>( *this);
    }

    ELY_ALWAYS_INLINE constexpr lexeme::LeadingAtmosphere
    as_leading_atmosphere_unchecked() const noexcept
    {
        ELY_ASSERT(is_leading_atmosphere(), "expected leading atmosphere");
        return detail::convert_variant_unchecked<lexeme::LeadingAtmosphere>(
            *this);
    }

    ELY_ALWAYS_INLINE constexpr lexeme::Atmosphere
    as_atmosphere_unchecked() const noexcept
    {
        ELY_ASSERT(is_atmosphere(), "expected atmosphere");
        return detail::convert_variant_unchecked<lexeme::Atmosphere>(*this);
    }

    ELY_ALWAYS_INLINE constexpr lexeme::Literal
    as_literal_unchecked() const noexcept
    {
        ELY_ASSERT(is_literal(), "expected literal");
        return detail::convert_variant_unchecked<lexeme::Literal>(*this);
    }

    ELY_ALWAYS_INLINE constexpr lexeme::PrefixAbbrev
    as_prefix_abbrev_unchecked() const noexcept
    {
        ELY_ASSERT(is_prefix_abbrev(), "expected prefix abbreviation");
        return detail::convert_variant_unchecked<lexeme::PrefixAbbrev>(*this);
    }

    ELY_ALWAYS_INLINE constexpr lexeme::InfixAbbrev
    as_infix_abbrev_unchecked() const noexcept
    {
        ELY_ASSERT(is_infix_abbrev(), "expected infix abbreviation");
        return {};
    }

    ELY_ALWAYS_INLINE constexpr lexeme::Abbrev
    as_abbrev_unchecked() const noexcept
    {
        ELY_ASSERT(is_abbrev(), "expected abbreviation");
        return detail::convert_variant_unchecked<lexeme::Abbrev>(*this);
    }

    ELY_ALWAYS_INLINE constexpr lexeme::Eof as_eof_unchecked() const noexcept
    {
        ELY_ASSERT(is_eof(), "expected EOF");
        return {};
    }
};

template<typename I, typename L = LexemeKind>
struct Lexeme
{
public:
    using iterator    = I;
    using size_type   = uint32_t;
    using lexeme_type = L;

public:
    LexemeSpan<I> span;
    L             kind{};

    ELY_ALWAYS_INLINE explicit constexpr operator bool() const noexcept
    {
        return kind;
    }
};
} // namespace ely