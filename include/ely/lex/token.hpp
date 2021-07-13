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

class Token2 : public token2::variant_type
{
    using base_ = token2::variant_type;

public:
    // using base_::base_;

    Token2() : base_(std::in_place_type<ely::token2::Eof>)
    {}

    template<typename T, typename... Args>
    explicit constexpr Token2(std::in_place_type_t<T> t, Args&&... args)
        : base_(t, static_cast<Args&&>(args)...)
    {}

    template<typename I>
    explicit constexpr Token2(const Lexeme<I>& lex)
        : base_(ely::visit([&](auto t) { return base_(t, t, lex.span); },
                           lex.kind))
    {}

    explicit constexpr operator bool() const noexcept
    {
        return !ely::holds_alternative<token2::Eof>(*this);
    }
};
} // namespace ely