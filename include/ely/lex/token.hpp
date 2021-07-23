#pragma once

#include <array>
#include <functional>
#include <string>

#include "ely/lex/lexeme.hpp"
#include "ely/lex/span.hpp"
#include "ely/lex/tokens.hpp"
#include "ely/utility.hpp"
#include "ely/variant.hpp"

namespace ely
{
namespace detail
{
template<typename T>
struct what_in_place_type;

template<typename T>
struct what_in_place_type<std::in_place_type_t<T>>
{
    using type = T;
};

template<typename T>
using what_in_place_type_t = typename what_in_place_type<T>::type;
} // namespace detail

template<typename... Ts>
class TokenVariant : public ely::Variant<Ts...>
{
    using base_ = ely::Variant<Ts...>;

public:
    TokenVariant() = default;

    template<typename T, typename... Args>
    explicit constexpr TokenVariant(std::in_place_type_t<T> t, Args&&... args)
        : base_(t, static_cast<Args&&>(args)...)
    {}

    template<typename I>
    explicit constexpr TokenVariant(const Lexeme<I>& lex)
        : base_(ely::visit([&](auto t) { return base_(t, t, lex.span); },
                           lex.kind))
    {}

    explicit constexpr operator bool() const noexcept
    {
        return !ely::holds_alternative<token::Eof>(*this);
    }
};

template<typename Token, typename... Ts>
class TokenVariantView
{
public:
    using token_type = Token;

private:
    token_type* tok_;

public:
    TokenVariantView() = default;

    constexpr TokenVariantView(Token& t) : tok_(std::addressof(t))
    {
        ELY_ASSERT((ely::holds_alternative<Ts>(t) || ...),
                   "Token is not holding one of the expected variants");
    }

    template<typename R, typename F>
    friend constexpr R visit(F&& fn, const TokenVariantView& tvv)
    {
        ely::visit<R>(
            [&](auto&& x) -> R {
                using ty = ely::remove_cvref_t<decltype(x)>;

                if constexpr (ely::is_same_one_of_v<ty, Ts...>)
                {
                    return std::invoke(static_cast<F&&>(fn),
                                       static_cast<decltype(x)&&>(x));
                }
                else
                {
                    __builtin_unreachable();
                }
            },
            *tvv.tok_);
    }

    template<typename F>
    friend constexpr decltype(auto) visit(F&& fn, const TokenVariantView& tvv)
    {
        using R = std::invoke_result_t<F, ely::first_element_t<Ts...>&>;

        return visit<R>(static_cast<F&&>(fn), tvv);
    }
};

template<typename It, typename... Ts>
class TokenVariantIterator
{
    using iter_traits   = std::iterator_traits<It>;
    using base_iterator = It;

    using Self = TokenVariantIterator;

public:
    using value_type =
        TokenVariantView<typename iter_traits::value_type, Ts...>;
    using reference         = value_type;
    using pointer           = void;
    using difference_type   = typename iter_traits::difference_type;
    using iterator_category = typename iter_traits::iterator_category;

private:
    base_iterator it_;

public:
    TokenVariantIterator() = default;

    constexpr TokenVariantIterator(base_iterator base_it) : it_(base_it)
    {}

    constexpr base_iterator base() const
    {
        return it_;
    }

    template<typename... Us>
    friend constexpr bool operator==(const Self&                            lhs,
                                     const TokenVariantIterator<It, Us...>& rhs)
    {
        return lhs.base() == rhs.base();
    }

    template<typename... Us>
    friend constexpr bool operator!=(const Self&                            lhs,
                                     const TokenVariantIterator<It, Us...>& rhs)
    {
        return lhs.base() != rhs.base();
    }

    template<typename... Us>
    friend constexpr bool operator<(const Self&                            lhs,
                                    const TokenVariantIterator<It, Us...>& rhs)
    {
        return lhs.base() < rhs.base();
    }

    template<typename... Us>
    friend constexpr bool operator<=(const Self&                            lhs,
                                     const TokenVariantIterator<It, Us...>& rhs)
    {
        return lhs.base() <= rhs.base();
    }

    template<typename... Us>
    friend constexpr bool operator>(const Self&                            lhs,
                                    const TokenVariantIterator<It, Us...>& rhs)
    {
        return lhs.base() > rhs.base();
    }

    template<typename... Us>
    friend constexpr bool operator>=(const Self&                            lhs,
                                     const TokenVariantIterator<It, Us...>& rhs)
    {
        return lhs.base() >= rhs.base();
    }

    constexpr Self& operator++()
    {
        ++it_;
        return *this;
    }

    constexpr Self operator++(int)
    {
        return Self{it_++};
    }

    constexpr Self& operator--()
    {
        --it_;
        return *this;
    }

    constexpr Self operator--(int)
    {
        return Self{it_--};
    }

    constexpr reference operator*() const
    {
        return reference{*it_};
    }
};

class Token : public token::token_types::template apply_all<ely::TokenVariant>
{
    using base_ = token::token_types::template apply_all<ely::TokenVariant>;

public:
    using base_::base_;

    constexpr Token() : base_(std::in_place_type<ely::token::Eof>)
    {}
};
} // namespace ely