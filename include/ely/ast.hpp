#pragma once

#include <string_view>

#include "ely/variant.hpp"
#include "ely/vector.hpp"

namespace ely
{
namespace ast
{
struct SourceLocation
{
    std::string_view filename;
    uint32_t         line;
    uint32_t         col;
};

class Literal
{};

class Name
{};

class Define
{
    Name name_;
    Ast  init_;
};

class Let
{
    std::vector<Var> bindings_;
    std::vector<Ast> inits_;
};

class Fn
{
    ely::Vector<Var> args_;
    Ast              body_;
};

class Value
{};

class Var
{
    Name name;
};

class Call
{
    ely::Vector<Ast> callee_and_args_;
};

class Ast
{
    ely::Variant<Var, Call, Value, Let> variant_;
};
} // namespace ast
} // namespace ely