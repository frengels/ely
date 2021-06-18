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
    Name name;
    Ast  init;
};

class Let
{};

class Fn
{};

class Value
{};

class Var
{};

class Ast
{};
} // namespace ast
} // namespace ely