#pragma once

#include "ely/lex/tokens.hpp"
#include "ely/variant.hpp"

namespace ely
{

namespace stx
{
namespace detail
{
template<template<typename> class It, typename... Ts>
class TokenVariant : public ely::Variant<It<Ts>...>
{
    using base_ = ely::Variant<It<Ts>...>;

public:
    using base_::base_;

    constexpr 
};
} // namespace detail

template<template<typename> class It>
class Literal : public detail::IterVariant<It,
                                           token::IntLit,
                                           token::FloatLit,
                                           token::CharLit,
                                           token::StringLit,
                                           token::KeywordLit,
                                           token::BoolLit,
                                           token::UnterminatedStringLit>
{
    using base_ = detail::IterVariant<It,
                                      token::IntLit,
                                      token::FloatLit,
                                      token::CharLit,
                                      token::StringLit,
                                      token::KeywordLit,
                                      token::BoolLit,
                                      token::UnterminatedStringLit>;

public:
    using base_::base_;
};
} // namespace stx
} // namespace ely