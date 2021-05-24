#pragma once

#include <cstdint>

#include "ely/tokenizer.hpp"

namespace ely
{
namespace node
{
using BoolLit    = ely::token::BoolLit;
using Identifier = ely::token::Identifier;
using IntLit     = ely::token::IntLit;
using FloatLit   = ely::token::FloatLit;
using CharLit    = ely::token::CharLit;
using StringLit  = ely::token::StringLit;
using KeywordLit = ely::token::KeywordLit;

class ParensList
{};

class BracketList
{};

class BraceList
{};
} // namespace node

namespace detail
{
union NodeUnion
{
#define DEFINE_CONSTRUCTOR(variant, member)                                    \
    template<typename... Args>                                                 \
    explicit constexpr NodeUnion(std::in_place_type_t<variant>,                \
                                 Args&&... args)                               \
        : member(static_cast<Args&&>(args)...)                                 \
    {}

    DEFINE_CONSTRUCTOR(node::Identifier, identifier)
    DEFINE_CONSTRUCTOR(node::IntLit, int_lit)
    DEFINE_CONSTRUCTOR(node::FloatLit, float_lit)
    DEFINE_CONSTRUCTOR(node::CharLit, char_lit)
    DEFINE_CONSTRUCTOR(node::StringLit, string_lit)
    DEFINE_CONSTRUCTOR(node::KeywordLit, keyword_lit)
    DEFINE_CONSTRUCTOR(node::BoolLit, bool_lit)
    DEFINE_CONSTRUCTOR(node::ParensList, parens_list)
    DEFINE_CONSTRUCTOR(node::BracketList, bracket_list)
    DEFINE_CONSTRUCTOR(node::BraceList, brace_list)

    node::Identifier  identifier;
    node::IntLit      int_lit;
    node::FloatLit    float_lit;
    node::CharLit     char_lit;
    node::StringLit   string_lit;
    node::KeywordLit  keyword_lit;
    node::BoolLit     bool_lit;
    node::ParensList  parens_list;
    node::BracketList bracket_list;
    node::BraceList   brace_list;
};
} // namespace detail

enum class NodeKind : uint8_t
{
    ParensList,
    BracketList,
    BraceList,

#define CAST_FROM_TOKENKIND(x)                                                 \
    x = static_cast<std::underlying_type_t<TokenKind>>(TokenKind::x)
    CAST_FROM_TOKENKIND(Identifier),
    CAST_FROM_TOKENKIND(IntLit),
    CAST_FROM_TOKENKIND(FloatLit),
    CAST_FROM_TOKENKIND(CharLit),
    CAST_FROM_TOKENKIND(StringLit),
    CAST_FROM_TOKENKIND(KeywordLit),
    CAST_FROM_TOKENKIND(BoolLit),
#undef CAST_FROM_TOKENKIND
};

class Node
{
    NodeKind          kind;
    detail::NodeUnion value;
};
} // namespace ely