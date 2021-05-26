#pragma once

#include <cstdint>
#include <numeric>
#include <span>
#include <string>
#include <vector>

#include "ely/atmosphere.hpp"
#include "ely/defines.h"
#include "ely/variant.hpp"

namespace ely
{
namespace token
{
class LParen
{
public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RParen
{
public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class LBracket
{
public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RBracket
{
public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class LBrace
{
public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RBrace
{
public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class Identifier
{
private:
    std::string name;

public:
    Identifier() = default;

    template<typename I>
    constexpr Identifier(I first, std::size_t len)
        : name(first, std::next(first, len))
    {}

    template<typename... Args>
    constexpr Identifier(std::in_place_t, Args&&... args)
        : name(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return name.size();
    }
};

class IntLit
{
private:
    std::string str;

public:
    IntLit() = default;

    template<typename I>
    constexpr IntLit(I first, std::size_t len)
        : str(first, std::next(first, len))
    {}

    template<typename... Args>
    constexpr IntLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str.size();
    }
};

class FloatLit
{
private:
    std::string str;

public:
    FloatLit() = default;

    template<typename I>
    constexpr FloatLit(I first, std::size_t len)
        : str(first, std::next(first, len))
    {}

    template<typename... Args>
    constexpr FloatLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str.size();
    }
};

class CharLit
{
private:
    std::string str;

public:
    CharLit() = default;

    /// automatically cuts off the first 2 values from the iterator
    template<typename I>
    constexpr CharLit(I first, std::size_t len)
        : str(std::next(first, 2), std::next(first, len))
    {}

    template<typename... Args>
    constexpr CharLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return 2 + str.size();
    }
};

class StringLit
{
private:
    std::string str;

public:
    StringLit() = default;

    template<typename I>
    constexpr StringLit(I first, std::size_t len)
        : str(std::next(first), std::next(first, len - 1))
    {}

    template<typename... Args>
    constexpr StringLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return 2 + str.size();
    }
};

class KeywordLit
{
private:
    std::string str;

public:
    KeywordLit() = default;

    template<typename I>
    constexpr KeywordLit(I first, std::size_t len)
        : str(std::next(first, 2), std::next(first, len))
    {}

    template<typename... Args>
    constexpr KeywordLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return 2 + str.size();
    }
};

class BoolLit
{
private:
    bool b;

public:
    BoolLit() = default;

    template<typename I>
    constexpr BoolLit(I first, [[maybe_unused]] std::size_t len)
        : b(*std::next(first) == 't')
    {}

    constexpr BoolLit(bool b) : b(b)
    {}

    constexpr bool value() const
    {
        return b;
    }

    static constexpr std::size_t size()
    {
        return 2;
    }
};

class Poison
{
private:
    std::string str;

public:
    Poison() = default;

    template<typename I>
    constexpr Poison(I first, std::size_t len)
        : str(first, std::next(first, len))
    {}

    template<typename... Args>
    explicit constexpr Poison(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str.size();
    }
};

class Eof
{
public:
    Eof() = default;

    template<typename I>
    constexpr Eof(I, std::size_t) : Eof()
    {}

    static constexpr std::size_t size()
    {
        return 0;
    }
};
} // namespace token

/// unlike a Lexeme, a RawToken owns the data it holds
class RawToken
{
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
                                     token::Poison,
                                     token::Eof>;

private:
    VariantType variant_;

public:
    template<typename T, typename... Args>
    explicit constexpr RawToken(std::in_place_type_t<T>, Args&&... args)
        : variant_(std::in_place_type<T>, static_cast<Args&&>(args)...)
    {}

    template<typename I>
    constexpr RawToken(Lexeme<I> lexeme)
        : variant_([&]() -> VariantType {
              ELY_ASSERT(!ely::lexeme_is_atmosphere(lexeme.kind),
                         "RawToken cannot be made from atmosphere");

              switch (lexeme.kind)
              {
              case LexemeKind::LParen:
                  return VariantType(std::in_place_type<token::LParen>);
              case LexemeKind::RParen:
                  return VariantType(std::in_place_type<token::RParen>);
              case LexemeKind::LBracket:
                  return VariantType(std::in_place_type<token::LBracket>);
              case LexemeKind::RBracket:
                  return VariantType(std::in_place_type<token::RBracket>);
              case LexemeKind::LBrace:
                  return VariantType(std::in_place_type<token::LBrace>);
              case LexemeKind::RBrace:
                  return VariantType(std::in_place_type<token::RBrace>);

              case LexemeKind::Identifier:
                  return VariantType(std::in_place_type<token::Identifier>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::IntLit:
                  return VariantType(std::in_place_type<token::IntLit>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::FloatLit:
                  return VariantType(std::in_place_type<token::FloatLit>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::CharLit:
                  return VariantType(std::in_place_type<token::CharLit>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::StringLit:
                  return VariantType(std::in_place_type<token::StringLit>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::KeywordLit:
                  return VariantType(std::in_place_type<token::KeywordLit>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::BoolLit:
                  return VariantType(std::in_place_type<token::BoolLit>,
                                     lexeme.start,
                                     lexeme.size());

              case LexemeKind::Poison:
                  return VariantType(std::in_place_type<token::Poison>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::Eof:
                  return VariantType(std::in_place_type<token::Eof>,
                                     lexeme.start,
                                     lexeme.size());

              default:
                  __builtin_unreachable();
              }
          }())
    {}

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return ely::visit(variant_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
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
        return visit([](const auto& x) {
            using ty = std::remove_cvref_t<decltype(x)>;
            return std::is_same_v<ty, token::Eof>;
        });
    }

    explicit constexpr operator bool() const noexcept
    {
        return !is_eof();
    }

    constexpr std::size_t size() const
    {
        return visit([](const auto& x) -> std::size_t { return x.size(); });
    }
};

class Token
{
    std::vector<Atmosphere> leading_atmo;
    std::vector<Atmosphere> trailing_atmo;
    RawToken                raw;

public:
    ELY_CONSTEXPR_VECTOR Token(std::vector<Atmosphere> leading_atmosphere,
                               std::vector<Atmosphere> trailing_atmosphere,
                               RawToken                tok)
        : leading_atmo(std::move(leading_atmosphere)),
          trailing_atmo(std::move(trailing_atmosphere)), raw(std::move(tok))
    {}

    constexpr RawToken& raw_token() &
    {
        return raw;
    }

    constexpr const RawToken& raw_token() const&
    {
        return raw;
    }

    constexpr RawToken&& raw_token() &&
    {
        return std::move(raw);
    }

    constexpr const RawToken&& raw_token() const&&
    {
        return std::move(raw);
    }

    ELY_CONSTEXPR_VECTOR std::span<const Atmosphere> leading_atmosphere() const&
    {
        return std::span<const Atmosphere>{leading_atmo.begin(),
                                           leading_atmo.end()};
    }

    ELY_CONSTEXPR_VECTOR std::span<const Atmosphere>
                         trailing_atmosphere() const&
    {
        return std::span<const Atmosphere>{trailing_atmo.begin(),
                                           trailing_atmo.end()};
    }

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return static_cast<RawToken&>(raw).visit(static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return static_cast<const RawToken&>(raw).visit(static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return static_cast<RawToken&&>(raw).visit(static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return static_cast<const RawToken&&>(raw).visit(static_cast<F&&>(fn));
    }

    constexpr bool is_eof() const noexcept
    {
        return raw_token().is_eof();
    }

    explicit constexpr operator bool() const noexcept
    {
        return !is_eof();
    }

    ELY_CONSTEXPR_VECTOR std::size_t size() const
    {
        std::size_t atmosphere_size = std::accumulate(
            leading_atmo.begin(),
            leading_atmo.end(),
            std::size_t{0},
            [](auto cur_sz, const auto& tok) { return cur_sz + tok.size(); });

        atmosphere_size = std::accumulate(
            trailing_atmo.begin(),
            trailing_atmo.end(),
            atmosphere_size,
            [](auto cur_sz, const auto& tok) { return cur_sz + tok.size(); });

        return atmosphere_size +
               visit([](const auto& x) -> std::size_t { return x.size(); });
    }
};
} // namespace ely