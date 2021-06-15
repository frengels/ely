#pragma once

#include <tuple>
#include <type_traits>

#include "ely/token.hpp"
#include "ely/variant.hpp"
#include "ely/vector.hpp"

namespace ely
{
namespace stx
{
class Syntax;
class List;
class Literal;
class Identifier;
class Eof;

class MissingRParen
{
public:
    using poison_tag = void;

public:
    static constexpr std::size_t size() noexcept
    {
        return 0;
    }
};

class MissingRBracket
{
public:
    using poison_tag = void;

public:
    static constexpr std::size_t size() noexcept
    {
        return 0;
    }
};

class MissingRBrace
{
public:
    using poison_tag = void;

public:
    static constexpr std::size_t size() noexcept
    {
        return 0;
    }
};

class List
{
private:
    ely::TokenVariant<ely::token::LParen>
        lparen_; // owns atmosphere before and after
    // this token can be Poison if no ending parenthesis is found
    ely::TokenVariant<ely::token::RParen, MissingRParen>
        rparen_; // owns atmosphere before and after

    ely::Vector<stx::Syntax> values_{};
    // summed size of the inner part of the stx node (  |__|__|__)
    std::size_t values_size_{0};
    bool        value_poisoned_{false};

public:
    template<typename... LArgs, typename... RArgs>
    constexpr List(std::tuple<LArgs...>       lparen_args,
                   std::tuple<RArgs...>       rparen_args,
                   ely::Vector<stx::Syntax>&& values,
                   std::size_t                values_size,
                   bool                       value_poisoned)
        : lparen_(std::apply(
              [](auto&&... args) {
                  return decltype(lparen_){
                      static_cast<decltype(args)&&>(args)...};
              },
              std::move(lparen_args))),
          rparen_(std::apply(
              [](auto&&... args) {
                  return decltype(rparen_){
                      static_cast<decltype(args)&&>(args)...};
              },
              std::move(rparen_args))),
          values_(std::move(values)), values_size_(values_size),
          value_poisoned_(value_poisoned)
    {}

    constexpr bool is_poison() const noexcept
    {
        return value_poisoned_ || lparen_.is_poison() || rparen_.is_poison();
    }

    constexpr std::size_t leading_size() const
    {
        return lparen_.leading_size();
    }

    constexpr std::size_t trailing_size() const
    {
        return rparen_.trailing_size();
    }

    constexpr std::size_t size() const
    {
        return lparen_.size() + rparen_.size() + values_size_;
    }

    // this is the span of the list including parentheses
    constexpr std::size_t inner_size() const
    {
        return lparen_.inner_size() + lparen_.trailing_size() +
               rparen_.inner_size() + rparen_.leading_size() + values_size_;
    }

    template<typename... Args>
    constexpr void emplace_back(Args&&... args);
};

class Literal
{
private:
    ely::TokenVariant<token::IntLit,
                      token::FloatLit,
                      token::CharLit,
                      token::StringLit,
                      token::KeywordLit,
                      token::BoolLit,
                      token::UnterminatedStringLit>
        lit_;

public:
    template<typename T, typename... Args>
    explicit constexpr Literal(
        ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
        ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing,
        std::in_place_type_t<T>                             t,
        Args&&... args)
        : lit_(std::move(leading),
               std::move(trailing),
               t,
               static_cast<Args&&>(args)...)
    {}

    constexpr const auto& token() const&
    {
        return lit_;
    }

    constexpr bool is_poison() const noexcept
    {
        return lit_.is_poison();
    }

    constexpr std::size_t size() const noexcept
    {
        return token().size();
    }

    constexpr std::size_t leading_size() const
    {
        return token().leading_size();
    }

    constexpr std::size_t trailing_size() const
    {
        return token().trailing_size();
    }

    constexpr std::size_t inner_size() const
    {
        return token().inner_size();
    }
};

class Identifier
{
private:
    ely::TokenVariant<token::Identifier, token::InvalidNumberSign> tok_;

public:
    template<typename T, typename... Args>
    constexpr Identifier(
        ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
        ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing,
        std::in_place_type_t<T>                             t,
        Args&&... args)
        : tok_(std::move(leading),
               std::move(trailing),
               t,
               static_cast<Args&&>(args)...)
    {}

    constexpr const auto& token() const&
    {
        return tok_;
    }

    constexpr bool is_poison() const noexcept
    {
        return tok_.is_poison();
    }

    constexpr std::size_t size() const noexcept
    {
        return token().size();
    }

    constexpr std::size_t leading_size() const
    {
        return token().leading_size();
    }

    constexpr std::size_t trailing_size() const
    {
        return token().trailing_size();
    }

    constexpr std::size_t inner_size() const noexcept
    {
        return token().inner_size();
    }
};

class Eof
{
private:
    ely::TokenVariant<token::Eof> tok_;

public:
    constexpr Eof(AtmosphereList<AtmospherePosition::Leading>&&  leading,
                  AtmosphereList<AtmospherePosition::Trailing>&& trailing,
                  token::Eof&&                                   eof)
        : tok_(std::move(leading),
               std::move(trailing),
               std::in_place_type<token::Eof>,
               std::move(eof))
    {}

    constexpr const ely::TokenVariant<token::Eof>& token() const&
    {
        return tok_;
    }

    constexpr bool is_poison() const
    {
        return false;
    }

    constexpr std::size_t size() const noexcept
    {
        return token().size();
    }

    constexpr std::size_t leading_size() const
    {
        return token().leading_size();
    }

    constexpr std::size_t trailing_size() const
    {
        return token().trailing_size();
    }

    constexpr std::size_t inner_size() const noexcept
    {
        return token().inner_size();
    }
};

class Syntax
{
private:
    ely::Variant<List, Literal, Identifier, Eof> variant_;

public:
    template<typename T, typename... Args>
    explicit constexpr Syntax(std::in_place_type_t<T> t, Args&&... args)
        : variant_(t, static_cast<Args&&>(args)...)
    {}

    constexpr bool is_list() const
    {
        return holds_alternative<List>(variant_);
    }

    constexpr bool is_literal() const
    {
        return holds_alternative<Literal>(variant_);
    }

    constexpr bool is_identifier() const
    {
        return holds_alternative<Identifier>(variant_);
    }

    constexpr bool is_eof() const
    {
        return holds_alternative<Eof>(variant_);
    }

    explicit constexpr operator bool() const noexcept
    {
        return !is_eof();
    }

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

    constexpr bool is_poison() const noexcept
    {
        return visit([](const auto& stx) { return stx.is_poison(); });
    }

    constexpr std::size_t size() const noexcept
    {
        return visit([](const auto& stx) { return stx.size(); });
    }

    constexpr std::size_t leading_size()
    {
        return visit([](const auto& stx) { return stx.leading_size(); });
    }

    constexpr std::size_t trailing_size()
    {
        return visit([](const auto& stx) { return stx.trailing_size(); });
    }

    constexpr std::size_t inner_size() const noexcept
    {
        return visit([](const auto& stx) { return stx.inner_size(); });
    }
};

template<typename... Args>
constexpr void List::emplace_back(Args&&... args)
{
    stx::Syntax& stx = values_.emplace_back(static_cast<Args&&>(args)...);
    values_size_ += stx.size();
}
} // namespace stx
} // namespace ely