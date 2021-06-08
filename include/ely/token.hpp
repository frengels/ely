#pragma once

#include <cstdint>
#include <numeric>
#include <ranges>
#include <span>
#include <string>
#include <vector>

#include "ely/assert.h"
#include "ely/atmosphere.hpp"
#include "ely/defines.h"
#include "ely/variant.hpp"

namespace ely
{
namespace detail
{
/// a pressurized token has atmosphere around it
template<typename RawTok>
class Pressurized
{
private:
    AtmosphereList               leading_atmosphere_;
    AtmosphereList               trailing_atmosphere_;
    [[no_unique_address]] RawTok tok_;

public:
    template<typename... Args>
    constexpr Pressurized(AtmosphereList leading,
                          AtmosphereList trailing,
                          Args&&... args)
        : leading_atmosphere_(std::move(leading)),
          trailing_atmosphere_(std::move(trailing)),
          tok_(static_cast<Args&&>(args)...)
    {}

    constexpr RawTok& raw_token() & noexcept
    {
        return static_cast<RawTok&>(tok_);
    }

    constexpr const RawTok& raw_token() const& noexcept
    {
        return static_cast<const RawTok&>(tok_);
    }

    constexpr RawTok&& raw_token() && noexcept
    {
        return static_cast<RawTok&&>(tok_);
    }

    constexpr const RawTok&& raw_token() const&& noexcept
    {
        return static_cast<const RawTok&&>(tok_);
    }

    constexpr std::size_t vacuum_size() const
    {
        return tok_.size();
    }

    constexpr const AtmosphereList& leading_atmosphere() const
    {
        return leading_atmosphere_;
    }

    constexpr const AtmosphereList& trailing_atmosphere() const
    {
        return trailing_atmosphere_;
    }

    constexpr std::size_t pressurized_size() const
    {

        return leading_atmosphere_.atmosphere_size() +
               trailing_atmosphere_.atmosphere_size() + vacuum_size();
    }
};
} // namespace detail

namespace token
{
namespace raw
{
class LParen
{
public:
    LParen() = default;

    template<typename I>
    explicit constexpr LParen([[maybe_unused]] Lexeme<I> lex)
    {
        ELY_ASSERT(lex.kind == LexemeKind::LParen, "expected LParen");
    }

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RParen
{
public:
    RParen() = default;

    template<typename I>
    explicit constexpr RParen([[maybe_unused]] Lexeme<I> lex)
    {
        ELY_ASSERT(lex.kind == LexemeKind::RParen, "expected RParen");
    }

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class LBracket
{
public:
    LBracket() = default;

    template<typename I>
    explicit constexpr LBracket([[maybe_unused]] Lexeme<I> lex)
    {
        ELY_ASSERT(lex.kind == LexemeKind::LBracket, "expected LBracket");
    }

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RBracket
{
public:
    RBracket() = default;

    template<typename I>
    explicit constexpr RBracket([[maybe_unused]] Lexeme<I> lex)
    {
        ELY_ASSERT(lex.kind == LexemeKind::RBracket, "expected RBracket");
    }

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class LBrace
{
public:
    LBrace() = default;

    template<typename I>
    explicit constexpr LBrace([[maybe_unused]] Lexeme<I> lex)
    {
        ELY_ASSERT(lex.kind == LexemeKind::LBrace, "expected LBrace");
    }

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RBrace
{
public:
    RBrace() = default;

    template<typename I>
    explicit constexpr RBrace([[maybe_unused]] Lexeme<I> lex)
    {
        ELY_ASSERT(lex.kind == LexemeKind::RBrace, "expected RBrace");
    }

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class Identifier
{
private:
    std::string name_;

public:
    Identifier() = default;

    template<typename I>
    explicit constexpr Identifier(Lexeme<I> lex) : name_(lex.begin(), lex.end())
    {
        ELY_ASSERT(lex.kind == LexemeKind::Identifier, "expected Identifier");
    }

    template<typename... Args>
    constexpr Identifier(std::in_place_t, Args&&... args)
        : name_(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::string_view name() const noexcept
    {
        return static_cast<std::string_view>(name_);
    }

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return name_.size();
    }
};

class IntLit
{
private:
    std::string str_;

public:
    IntLit() = default;

    template<typename I>
    explicit constexpr IntLit(Lexeme<I> lex) : str_(lex.begin(), lex.end())
    {
        ELY_ASSERT(lex.kind == LexemeKind::IntLit, "expected IntLit");
    }

    template<typename... Args>
    constexpr IntLit(std::in_place_t, Args&&... args)
        : str_(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str_.size();
    }
};

class FloatLit
{
private:
    std::string str_;

public:
    FloatLit() = default;

    template<typename I>
    explicit constexpr FloatLit(Lexeme<I> lex) : str_(lex.begin(), lex.end())
    {
        ELY_ASSERT(lex.kind == LexemeKind::FloatLit, "expected FloatLit");
    }

    template<typename... Args>
    constexpr FloatLit(std::in_place_t, Args&&... args)
        : str_(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str_.size();
    }
};

class CharLit
{
private:
    std::string str_;

public:
    CharLit() = default;

    template<typename I>
    explicit constexpr CharLit(Lexeme<I> lex) : str_(lex.begin(), lex.end())
    {
        ELY_ASSERT(lex.kind == LexemeKind::CharLit, "expected CharLit");
    }

    template<typename... Args>
    constexpr CharLit(std::in_place_t, Args&&... args)
        : str_(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str_.size();
    }
};

class StringLit
{
private:
    std::string str_;

public:
    StringLit() = default;

    template<typename I>
    constexpr StringLit(Lexeme<I> lex) : str_(lex.begin(), lex.end())
    {
        ELY_ASSERT(lex.kind == LexemeKind::StringLit, "expected StringLit");
    }

    template<typename... Args>
    constexpr StringLit(std::in_place_t, Args&&... args)
        : str_(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str_.size();
    }
};

class KeywordLit
{
private:
    std::string str_;

public:
    KeywordLit() = default;

    template<typename I>
    explicit constexpr KeywordLit(Lexeme<I> lex) : str_(lex.begin(), lex.end())
    {
        ELY_ASSERT(lex.kind == LexemeKind::KeywordLit, "expected KeywordLit");
    }

    template<typename... Args>
    constexpr KeywordLit(std::in_place_t, Args&&... args)
        : str_(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str_.size();
    }
};

class BoolLit
{
private:
    bool b{false};

public:
    BoolLit() = default;

    // assuming the contents are either '#t' or '#f'
    template<typename I>
    explicit constexpr BoolLit(Lexeme<I> lex)
        : b(*std::next(lex.begin()) == 't')
    {
        ELY_ASSERT(lex.kind == LexemeKind::BoolLit, "expected BoolLit");
    }

    explicit constexpr BoolLit(bool b) : b(b)
    {}

    constexpr bool value() const
    {
        return b;
    }

    constexpr std::string_view str() const
    {
        return b ? std::string_view{"#t"} : std::string_view{"#f"};
    }

    static constexpr std::size_t size()
    {
        return 2;
    }
};

class UnterminatedStringLit
{
private:
    std::string str_;

public:
    template<typename I>
    explicit constexpr UnterminatedStringLit(Lexeme<I> lex)
        : str_(lex.begin(), lex.end())
    {
        ELY_ASSERT(lex.kind == LexemeKind::StringLit, "expected StringLit");
    }

    ELY_CONSTEXPR_STRING std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_CONSTEXPR_STRING std::size_t size() const noexcept
    {
        return str_.size();
    }
};

class InvalidNumberSign
{
private:
    std::string str_;

public:
    template<typename I>
    explicit constexpr InvalidNumberSign(Lexeme<I> lex)
        : str_(lex.begin(), lex.end())
    {
        ELY_ASSERT(lex.kind == LexemeKind::InvalidNumberSign,
                   "expected InvalidNumberSign");
    }

    ELY_CONSTEXPR_STRING std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_CONSTEXPR_STRING std::size_t size() const noexcept
    {
        return str_.size();
    }
};

class Eof
{
public:
    Eof() = default;

    template<typename I>
    explicit constexpr Eof([[maybe_unused]] Lexeme<I> lex)
    {
        ELY_ASSERT(lex.kind == LexemeKind::Eof, "expected Eof");
    }

    static constexpr std::size_t size()
    {
        return 0;
    }
};
} // namespace raw

using LParen = detail::Pressurized<raw::LParen>;
using RParen = detail::Pressurized<raw::RParen>;

using LBracket = detail::Pressurized<raw::LBracket>;
using RBracket = detail::Pressurized<raw::RBracket>;

using LBrace = detail::Pressurized<raw::LBrace>;
using RBrace = detail::Pressurized<raw::RBrace>;

using Identifier = detail::Pressurized<raw::Identifier>;

using IntLit     = detail::Pressurized<raw::IntLit>;
using FloatLit   = detail::Pressurized<raw::FloatLit>;
using CharLit    = detail::Pressurized<raw::CharLit>;
using StringLit  = detail::Pressurized<raw::StringLit>;
using KeywordLit = detail::Pressurized<raw::KeywordLit>;
using BoolLit    = detail::Pressurized<raw::BoolLit>;

using InvalidNumberSign     = detail::Pressurized<raw::InvalidNumberSign>;
using UnterminatedStringLit = detail::Pressurized<raw::UnterminatedStringLit>;

using Eof = detail::Pressurized<raw::Eof>;
} // namespace token

class Token
{
private:
    using VariantType = ely::Variant<token::LParen,
                                     token::RParen,
                                     token::LBracket,
                                     token::RBracket,
                                     token::LBrace,
                                     token::RBrace,
                                     token::Identifier,
                                     token::IntLit,
                                     token::FloatLit,
                                     token::CharLit,
                                     token::StringLit,
                                     token::KeywordLit,
                                     token::BoolLit,
                                     token::UnterminatedStringLit,
                                     token::InvalidNumberSign,
                                     token::Eof>;

private:
    VariantType variant_;

public:
    template<typename I>
    ELY_CONSTEXPR_VECTOR
    Token(AtmosphereList leading, AtmosphereList trailing, Lexeme<I> lexeme)
        : variant_([&]() -> VariantType {
              auto construct_token = [&](auto in_place_type) -> VariantType {
                  return VariantType(std::move(in_place_type),
                                     std::move(leading),
                                     std::move(trailing),
                                     lexeme);
              };

#define DISPATCH(tok)                                                          \
    case LexemeKind::tok:                                                      \
        return construct_token(std::in_place_type<token::tok>)

              switch (lexeme.kind)
              {
                  DISPATCH(LParen);
                  DISPATCH(RParen);
                  DISPATCH(LBracket);
                  DISPATCH(RBracket);
                  DISPATCH(LBrace);
                  DISPATCH(RBrace);
                  DISPATCH(Identifier);
                  DISPATCH(IntLit);
                  DISPATCH(FloatLit);
                  DISPATCH(CharLit);
                  DISPATCH(StringLit);
                  DISPATCH(KeywordLit);
                  DISPATCH(BoolLit);
                  DISPATCH(InvalidNumberSign);
                  DISPATCH(UnterminatedStringLit);
                  DISPATCH(Eof);
              default:
                  __builtin_unreachable();
              }

#undef DISPATCH
          }())

    {}

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return ely::visit(variant_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return ely::visit(variant_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return ely::visit(std::move(variant_), static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return ely::visit(std::move(variant_), static_cast<F&&>(fn));
    }

    constexpr bool is_eof() const noexcept
    {
        return visit([](const auto& tok) {
            const auto& raw_tok = tok.raw_token();
            using raw_ty        = std::remove_cvref_t<decltype(raw_tok)>;
            return std::is_same_v<raw_ty, ely::token::raw::Eof>;
        });
    }

    explicit constexpr operator bool() const noexcept
    {
        return !is_eof();
    }

    template<typename T>
    friend constexpr bool holds(const Token& self) noexcept
    {
        return self.visit([](const auto& x) {
            using ty = std::remove_cvref_t<decltype(x)>;
            return std::is_same_v<T, ty>;
        });
    }

    template<typename T>
    friend constexpr T& unsafe_get(Token& self) noexcept
    {
        return *get_if<T>(&self.variant_);
    }

    template<typename T>
    friend constexpr T&& unsafe_get(Token&& self) noexcept
    {
        return static_cast<T&&>(*get_if<T>(&self.variant_));
    }

    constexpr const AtmosphereList& leading_atmosphere() const
    {
        return visit([](const auto& tok) -> const AtmosphereList& {
            return tok.leading_atmosphere();
        });
    }

    constexpr const AtmosphereList& trailing_atmosphere() const
    {
        return visit([](const auto& tok) -> const AtmosphereList& {
            return tok.trailing_atmosphere();
        });
    }

    constexpr std::size_t pressurized_size() const
    {
        return visit([](const auto& tok) -> std::size_t {
            return tok.pressurized_size();
        });
    }

    constexpr std::size_t vacuum_size() const
    {
        return visit(
            [](const auto& tok) -> std::size_t { return tok.vacuum_size(); });
    }
};
} // namespace ely