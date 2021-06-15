#pragma once

#include <memory>
#include <span>
#include <variant>

#include "ely/defines.h"
#include "ely/ilist.hpp"
#include "ely/token.hpp"
#include "ely/variant.hpp"
#include "ely/vector.hpp"

namespace ely
{
namespace ast
{
class LexicalContext
{};

class Poison
{
    [[no_unique_address]] LexicalContext ctx_;

public:
    constexpr Poison strip() const
    {
        return *this;
    }

    constexpr const LexicalContext& lexical_context() const
    {
        return ctx_;
    }
};

template<typename... Args>
using Variant = ely::Variant<Poison, Args...>;

class Ast;
class Val;
class Syntax;

class Name
{
private:
    std::string val_;

public:
    explicit ELY_CONSTEXPR_STRING Name(std::string n) : val_(std::move(n))
    {}

    template<typename... Args>
    constexpr Name(std::in_place_t, Args&&... args)
        : val_(static_cast<Args&&>(args)...)
    {}

    constexpr std::string& get() & noexcept
    {
        return val_;
    }

    constexpr const std::string& get() const& noexcept
    {
        return val_;
    }

    constexpr std::string&& get() && noexcept
    {
        return std::move(val_);
    }

    constexpr const std::string&& get() const&& noexcept
    {
        return std::move(val_);
    }
};

class Symbol
{
private:
    Name name_;

public:
    explicit ELY_CONSTEXPR_STRING Symbol(Name n) : name_(std::move(n))
    {}

    constexpr Name& name() & noexcept
    {
        return name_;
    }

    constexpr const Name& name() const& noexcept
    {
        return name_;
    }

    constexpr Name&& name() && noexcept
    {
        return std::move(name_);
    }

    constexpr const Name&& name() const&& noexcept
    {
        return std::move(name_);
    }
};

class Var
{
private:
    Name name_;

public:
    explicit ELY_CONSTEXPR_STRING Var(Name name) : name_(std::move(name))
    {}

    constexpr Name& name() & noexcept
    {
        return name_;
    }

    constexpr const Name& name() const& noexcept
    {
        return name_;
    }

    constexpr Name&& name() && noexcept
    {
        return std::move(name_);
    }

    constexpr const Name&& name() const&& noexcept
    {
        return std::move(name_);
    }
};

class Literal
{
private:
    ast::Variant<ely::token::IntLit,
                 ely::token::FloatLit,
                 ely::token::CharLit,
                 ely::token::StringLit,
                 ely::token::KeywordLit,
                 ely::token::BoolLit>
        variants_;

public:
    template<typename T, typename... Args>
    explicit constexpr Literal(std::in_place_type_t<T> t, Args&&... args)
        : variants_(t, static_cast<Args&&>(args)...)
    {}

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return ely::visit(variants_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return ely::visit(variants_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return ely::visit(std::move(variants_), static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return ely::visit(std::move(variants_), static_cast<F&&>(fn));
    }
};

class Atom
{
private:
    ast::Variant<Symbol, Literal> variants_;

public:
    template<typename T, typename... Args>
    explicit constexpr Atom(std::in_place_type_t<T> t, Args&&... args)
        : variants_(t, static_cast<Args&&>(args)...)
    {}

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return ely::visit(variants_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return ely::visit(variants_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return ely::visit(std::move(variants_), static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return ely::visit(std::move(variants_), static_cast<F&&>(fn));
    }
};

class Call
{
private:
    std::unique_ptr<Ast> callee_;
    ely::Vector<Ast>     args_;

public:
    ELY_CONSTEXPR_VECTOR Call(std::unique_ptr<Ast> callee,
                              ely::Vector<Ast>     args)
        : callee_(std::move(callee)), args_(std::move(args))
    {}

    Ast& callee() &
    {
        return *callee_;
    }

    const Ast& callee() const&
    {
        return *callee_;
    }

    constexpr const ely::Vector<Ast>& args() const
    {
        return args_;
    }
};

class Fn
{
private:
    ely::Vector<Var>     args_;
    std::unique_ptr<Ast> body_;

public:
    ELY_CONSTEXPR_VECTOR Fn(ely::Vector<Var> args, std::unique_ptr<Ast> body)
        : args_(std::move(args)), body_(std::move(body))
    {}

    const Ast& body() const& noexcept
    {
        return *body_;
    }

    constexpr const ely::Vector<Var>& args() const& noexcept
    {
        return args_;
    }
};

class List
{
private:
    ely::Vector<Val> values_;

public:
    explicit ELY_CONSTEXPR_VECTOR List(ely::Vector<Val> values)
        : values_(std::move(values))
    {}

    constexpr const ely::Vector<Val>& values() const&
    {
        return values_;
    }
};

class SyntaxList
{
private:
    [[no_unique_address]] LexicalContext ctx_;
    ely::Vector<Syntax>                  values_;

public:
    ELY_CONSTEXPR_VECTOR SyntaxList(LexicalContext      ctx,
                                    ely::Vector<Syntax> values)
        : ctx_(std::move(ctx)), values_(std::move(values))
    {}

    constexpr const LexicalContext& lexical_context() const
    {
        return ctx_;
    }

    constexpr const ely::Vector<Syntax>& values() const&
    {
        return values_;
    }

    ELY_CONSTEXPR_VECTOR List strip() const;
};

class SyntaxLiteral
{
private:
    [[no_unique_address]] LexicalContext ctx_;
    Literal                              lit_;

public:
    ELY_CONSTEXPR_STRING SyntaxLiteral(LexicalContext ctx, Literal lit)
        : ctx_(std::move(ctx)), lit_(std::move(lit))
    {}

    constexpr const LexicalContext& lexical_context() const
    {
        return ctx_;
    }

    ELY_CONSTEXPR_STRING Literal strip() const
    {
        return lit_;
    }
};

class Identifier
{
private:
    [[no_unique_address]] LexicalContext ctx_;
    Symbol                               sym_;

public:
    ELY_CONSTEXPR_STRING Identifier(LexicalContext ctx, Symbol sym)
        : ctx_(std::move(ctx)), sym_(std::move(sym))
    {}

    constexpr const LexicalContext& lexical_context() const
    {
        return ctx_;
    }

    ELY_CONSTEXPR_STRING Symbol strip() const&
    {
        return sym_;
    }
};

class Syntax
{
private:
    ast::Variant<SyntaxLiteral, Identifier, SyntaxList> variants_;

public:
    template<typename T, typename... Args>
    explicit constexpr Syntax(std::in_place_type_t<T> t, Args&&... args)
        : variants_(t, static_cast<Args&&>(args)...)
    {}

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return ely::visit(variants_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return ely::visit(variants_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return ely::visit(std::move(variants_), static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return ely::visit(std::move(variants_), static_cast<F&&>(fn));
    }

    constexpr const LexicalContext& lexical_context() const
    {
        return visit([](const auto& stx) -> const LexicalContext& {
            return stx.lexical_context();
        });
    }

    // strips the lexical context recursively, used for parsing quote
    ast::Variant<Symbol, Literal, List> strip() const&
    {
        return visit([&](const auto& x) {
            return ast::Variant<Symbol, Literal, List>{x.strip()};
        });
    }
};

class Val
{
private:
    ast::Variant<Fn, Atom, List, Syntax> variants_;

public:
    template<typename T, typename... Args>
    explicit constexpr Val(std::in_place_type_t<T> t, Args&&... args)
        : variants_(t, static_cast<Args&&>(args)...)
    {}

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return ely::visit(variants_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return ely::visit(variants_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return ely::visit(std::move(variants_), static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return ely::visit(std::move(variants_), static_cast<F&&>(fn));
    }
};

class Ast
{
private:
    ast::Variant<Call, Var, Val> variants_;

public:
    template<typename T, typename... Args>
    constexpr Ast(std::in_place_type_t<T>, Args&&... args)
        : variants_(std::in_place_type<T>, static_cast<Args&&>(args)...)
    {}

    template<typename F>
    constexpr decltype(auto) visit(F&& fn) &
    {
        return ely::visit(variants_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr decltype(auto) visit(F&& fn) const&
    {
        return ely::visit(variants_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr decltype(auto) visit(F&& fn) &&
    {
        return ely::visit(std::move(variants_), static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr decltype(auto) visit(F&& fn) const&&
    {
        return ely::visit(std::move(variants_), static_cast<F&&>(fn));
    }
};

List SyntaxList::strip() const
{
    ely::Vector<Val> vals;

    for (const auto& stx : values())
    {
        vals.emplace_back(
            ely::visit(stx.strip(), [&](auto&& sym_lit_list) -> Val {
                using strip_ty = std::remove_cvref_t<decltype(sym_lit_list)>;

                if constexpr (std::is_same_v<List, strip_ty>)
                {
                    return Val(
                        std::in_place_type<List>,
                        static_cast<decltype(sym_lit_list)&&>(sym_lit_list));
                }
                else
                {
                    return Val(
                        std::in_place_type<Atom>,
                        std::in_place_type<strip_ty>,
                        static_cast<decltype(sym_lit_list)&&>(sym_lit_list));
                }
            }));
    }

    return List(std::move(vals));
}
} // namespace ast
} // namespace ely