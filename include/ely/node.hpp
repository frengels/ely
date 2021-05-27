#pragma once

#include <cstdint>
#include <list>

#include "ely/token.hpp"
#include "ely/variant.hpp"

namespace ely
{
class Node;
namespace node
{
using BoolLit    = ely::token::BoolLit;
using Identifier = ely::token::Identifier;
using IntLit     = ely::token::IntLit;
using FloatLit   = ely::token::FloatLit;
using CharLit    = ely::token::CharLit;
using StringLit  = ely::token::StringLit;
using KeywordLit = ely::token::KeywordLit;

using Eof    = ely::token::Eof;
using Poison = ely::token::Poison;

template<typename L, typename R>
class List
{
private:
    std::list<Node>         children_;
    [[no_unique_address]] L left_closing_;
    [[no_unique_address]] R right_closing_;

public:
    constexpr List(std::list<Node> children, L left_closing, R right_closing)
        : children_(std::move(children)),
          left_closing_(std::move(left_closing)),
          right_closing_(std::move(right_closing))
    {}

    constexpr const std::list<Node>& children() const& noexcept
    {
        return children_;
    }

    constexpr std::list<Node>& children() & noexcept
    {
        return children_;
    }

    constexpr const std::list<Node>&& children() const&& noexcept
    {
        return std::move(children_);
    }

    constexpr std::list<Node>&& children() && noexcept
    {
        return std::move(children_);
    }
};

using ParensList  = List<token::LParen, token::RParen>;
using BracketList = List<token::LBracket, token::RBracket>;
using BraceList   = List<token::LBrace, token::RBrace>;
} // namespace node

class Node
{
private:
    using VariantType = ely::Variant<node::Identifier,
                                     node::IntLit,
                                     node::FloatLit,
                                     node::CharLit,
                                     node::StringLit,
                                     node::KeywordLit,
                                     node::BoolLit,
                                     node::Eof,
                                     node::Poison,
                                     node::ParensList,
                                     node::BracketList,
                                     node::BraceList>;

private:
    VariantType variant_;

public:
    template<typename T, typename... Args>
    explicit constexpr Node(std::in_place_type_t<T>, Args&&... args)
        : variant_(std::in_place_type<T>, static_cast<Args&&>(args)...)
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
        return visit([](const auto& x) -> bool {
            using ty = std::remove_cvref_t<decltype(x)>;
            return std::is_same_v<ty, node::Eof>;
        });
    }

    explicit constexpr operator bool() const noexcept
    {
        return !is_eof();
    }
};
} // namespace ely