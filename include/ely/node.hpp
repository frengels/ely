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
                                     node::ParensList,
                                     node::BracketList,
                                     node::BraceList>;

    VariantType variant_;
};
} // namespace ely