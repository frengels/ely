#pragma once

#include <memory>
#include <string_view>

#include "ely/defines.h"
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

class Type;
class Expr;
class Var;

class IntegerType
{
private:
    uint16_t bits_;

public:
    explicit constexpr IntegerType(uint16_t bits) noexcept : bits_(bits)
    {}
};

enum class FloatType
{
    // TODO add other fp types
    F32,
    F64,
};

class StructType
{
    ely::Vector<Type> types;
};

class FnType
{};

class Type : public ely::Variant<IntegerType, FloatType, StructType, FnType>
{};

class Var
{
    std::string name;
    Type        ty;
};

class Literal
{
    Type ty;
};

class Let
{
    ely::Vector<Var>  bindings_;
    ely::Vector<Expr> inits_;
};

class Fn
{
    ely::Vector<Var>      args_;
    std::unique_ptr<Expr> body_;
};

class Call
{
    ely::Vector<Expr> callee_and_args_;
};

class Expr : public ely::Variant<Var, Call, Literal, Let>
{
private:
    using base_ = ely::Variant<Var, Call, Literal, Let>;

public:
    using base_::base_;

    using base_::visit;

    constexpr bool is_var() const
    {
        return holds_alternative<Var>(*this);
    }

    constexpr bool is_call() const
    {
        return holds_alternative<Call>(*this);
    }

    constexpr bool is_literal() const
    {
        return holds_alternative<Literal>(*this);
    }

    constexpr bool is_let() const
    {
        return holds_alternative<Let>(*this);
    }
};

class Define
{
private:
    Var  var_;
    Expr init_;

public:
    ELY_CONSTEXPR_STRING Define(Var&& v, Expr&& init)
        : var_(std::move(v)), init_(std::move(init))
    {}
};

class Module
{
private:
    std::string name_{"<undefined>"};
    // TODO imports
    ely::Vector<Define> definitions_;

public:
    Module() = default;

    explicit ELY_CONSTEXPR_STRING Module(std::string&& name)
        : name_(std::move(name))
    {}

    constexpr auto& definitions() &
    {
        return definitions_;
    }

    constexpr const auto& definitions() const&
    {
        return definitions_;
    }
};
} // namespace ast
} // namespace ely