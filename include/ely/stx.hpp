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

class MissingRParen
{
public:
    static constexpr std::size_t size() noexcept
    {
        return 0;
    }
};

class MissingRBracket
{
public:
    static constexpr std::size_t size() noexcept
    {
        return 0;
    }
};

class MissingRBrace
{
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

    std::vector<stx::Syntax> values_{};
    // summed size of the inner part of the stx node (  |__|__|__)
    std::size_t values_size_{0};
    bool        value_poisoned_{false};

public:
    template<typename L, typename R>
    constexpr List(L&& lparen, R&& rparen)
        : lparen_(static_cast<L&&>(lparen)), rparen_(static_cast<R&&>(rparen))
    {}

    constexpr bool poisoned() const
    {
        return value_poisoned_ || holds<MissingRParen>(rparen_);
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
    ELY_CONSTEXPR_VECTOR void emplace_back(Args&&... args);
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

    constexpr bool poisoned() const
    {
        return holds<token::UnterminatedStringLit>(token());
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
    ely::TokenVariant<token::Identifier> tok_;

public:
    constexpr const auto& token() const&
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
    constexpr const ely::TokenVariant<token::Eof>& token() const&
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

    explicit constexpr operator bool() const noexcept
    {
        return holds_alternative<Eof>(variant_);
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

template<typename... Args>
ELY_CONSTEXPR_VECTOR void List::emplace_back(Args&&... args)
{
    stx::Syntax& stx = values_.emplace_back(static_cast<Args&&>(args)...);
    values_size_ += stx.size();
}
} // namespace stx
} // namespace ely