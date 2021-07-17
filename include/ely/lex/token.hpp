#pragma once

#include <array>
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

class Token : public token::variant_type
{
    using base_ = token::variant_type;

public:
    Token() : base_(std::in_place_type<ely::token::Eof>)
    {}

    template<typename T, typename... Args>
    explicit constexpr Token(std::in_place_type_t<T> t, Args&&... args)
        : base_(t, static_cast<Args&&>(args)...)
    {}

    template<typename I>
    explicit constexpr Token(const Lexeme<I>& lex)
        : base_(ely::visit([&](auto t) { return base_(t, t, lex.span); },
                           lex.kind))
    {}

    explicit constexpr operator bool() const noexcept
    {
        return !ely::holds_alternative<token::Eof>(*this);
    }
};

class LeadingAtmosphereToken : public ely::Variant<token::Whitespace,
                                                   token::Tab,
                                                   token::Comment,
                                                   token::NewlineCr,
                                                   token::NewlineLf,
                                                   token::NewlineCrlf>
{
    using base_ = ely::Variant<token::Whitespace,
                               token::Tab,
                               token::Comment,
                               token::NewlineCr,
                               token::NewlineLf,
                               token::NewlineCrlf>;

public:
    static constexpr std::size_t variant_size = 6;

    using base_::base_;
};

class TrailingAtmosphereTokenView
    : public ely::Variant<token::Whitespace, token::Tab, token::Comment>
{

    using base_ = ely::Variant<token::Whitespace, token::Tab, token::Comment>;

public:
    static constexpr std::size_t variant_size = 3;

    using base_::base_;
};

template<template<typename> class It>
class TrailingAtmosphereView
{
    using base_iterator = It<Token>;

public:
    class sentinel;

    class iterator
    {
        friend sentinel;

    public:
        using value_type        = TrailingAtmosphereTokenView;
        using reference         = value_type;
        using pointer           = void;
        using difference_type   = std::ptrdiff_t;
        using iterator_category = std::bidirectional_iterator_tag;

    private:
        base_iterator it_;

    public:
        iterator() = default;

        explicit constexpr iterator(base_iterator it) : it_(it)
        {}

        constexpr bool operator==(const iterator& other) const noexcept
        {
            return it_ == other.it_;
        }

        constexpr bool operator!=(const iterator& other) const noexcept
        {
            return it_ != other.it_;
        }

        constexpr iterator& operator++()
        {
            ++it_;
        }

        constexpr iterator operator++(int)
        {
            return iterator{it_++};
        }

        constexpr iterator& operator--()
        {
            --it_;
        }

        constexpr iterator operator--(int)
        {
            return iterator{it_--};
        }

        constexpr reference operator*() const noexcept
        {
            return ely::visit(
                [&](auto&& x) {
                    using ty = ely::remove_cvref_t<decltype(x)>;

                    if constexpr(ely::is_same_one_of_v<ty, )
                },
                *it_);
        }
    };

    class sentinel
    {
    public:
        sentinel() = default;

        friend constexpr bool operator==(const sentinel&,
                                         const iterator& it) noexcept
        {
            return ely::visit(
                [](const auto& x) { return !x.is_trailing_atmosphere(); },
                it.it_);
        }
    };

    using value_type      = typename iterator::value_type;
    using reference       = value_type&;
    using const_reference = const value_type&;

private:
    base_iterator it_;

public:
    constexpr TrailingAtmosphereView(base_iterator it) : it_(it)
    {}

    constexpr iterator begin() const noexcept
    {
        iterator{it_};
    }

    constexpr sentinel end() const noexcept
    {
        sentinel{};
    }
};

class ConcreteToken : public token::variant_type
{
    using base_ = token::variant_type;

public:
    using base_::base_;
};
} // namespace ely