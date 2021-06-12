#pragma once

#include <type_traits>
#include <vector>

#include "ely/token.hpp"
#include "ely/variant.hpp"

namespace ely
{
namespace stx
{
class Syntax;
class List;
class Literal;
class Identifier;
class Eof;

namespace poison
{
// contains all kinds of poison variants which the expander will have to deal
// with and output errors for, basically delaying all action to as late as
// possible
}

class Poison
{};

template<typename... Toks>
using TokenVariant = ely::TokenVariant<Toks...>;

template<typename... Toks>
using PTokenVariant = TokenVariant<Poison, Toks...>;

class List
{
private:
    TokenVariant<ely::token::LParen>
        lparen_; // owns atmosphere before and after
    // this token can be Poison if no ending parenthesis is found
    PTokenVariant<ely::token::RParen>
        rparen_{}; // owns atmosphere before and after

    std::vector<stx::Syntax> values_{};
    // summed size of the inner part of the stx node (  |__|__|__)
    std::size_t values_size_{0};
    bool        value_poisoned_{false};

public:
    template<typename L>
    constexpr List(L&& lparen) : lparen_(static_cast<L&&>(lparen))
    {}

    constexpr bool poisoned() const
    {
        value_poisoned_ || holds<Poison>(rparen_);
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
    constexpr void emplace_back(Args&&... args)
    {
        stx::Syntax& stx = values_.emplace_back(static_cast<Args&&>(args)...);
        values_size_ += stx.size();
    }
};

class Literal
{
private:
    stx::PTokenVariant<token::IntLit,
                       token::FloatLit,
                       token::CharLit,
                       token::StringLit,
                       token::KeywordLit,
                       token::BoolLit>
        lit_;

public:
    constexpr const auto& token() const&
    {
        return lit_;
    }

    constexpr bool poisoned() const
    {
        return holds<Poison>(token());
    }

    constexpr std::size_t size() const noexcept
    {
        return token().size();
    }

    constexpr std::size_t leading_size()
    {
        return token().leading_size();
    }

    constexpr std::size_t trailing_size()
    {
        return token().trailing_size();
    }

    constexpr std::size_t inner_size()
    {
        return token().inner_size();
    }
};

class Identifier
{
private:
    stx::PTokenVariant<token::Identifier> tok_;

public:
    constexpr const auto& token() const&
    {
        return tok_;
    }

    constexpr bool poisoned() const
    {
        return holds<Poison>(token());
    }

    constexpr std::size_t size() const noexcept
    {
        token().size();
    }

    constexpr std::size_t leading_size()
    {
        return token().leading_size();
    }

    constexpr std::size_t trailing_size()
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
    stx::TokenVariant<token::Eof> tok_;

public:
    constexpr const TokenVariant<token::Eof>& token() const&
    {
        return tok_;
    }

    constexpr bool poisoned() const
    {
        return false;
    }

    constexpr std::size_t size() const noexcept
    {
        return token().size();
    }

    constexpr std::size_t leading_size()
    {
        return token().leading_size();
    }

    constexpr std::size_t trailing_size()
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

    explicit constexpr operator bool() const noexcept
    {
        return holds<Eof>(variant_);
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
} // namespace stx
} // namespace ely