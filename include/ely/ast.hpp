#pragma once

#include <memory>
#include <span>
#include <variant>
#include <vector>

#include "ely/defines.h"
#include "ely/ilist.hpp"
#include "ely/variant.hpp"

namespace ely
{
namespace ast
{
class Ast;
class Val;

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
    ely::Variant<ely::token::IntLit,
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
    ely::Variant<Symbol, Literal> variants_;

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
    std::vector<Ast>     args_;

public:
    ELY_CONSTEXPR_VECTOR Call(std::unique_ptr<Ast> callee,
                              std::vector<Ast>     args)
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

    constexpr const std::vector<Ast>& args() const
    {
        return args_;
    }
};

class Fn
{
private:
    std::vector<Var>     args_;
    std::unique_ptr<Ast> body_;

public:
    ELY_CONSTEXPR_VECTOR Fn(std::vector<Var> args, std::unique_ptr<Ast> body)
        : args_(std::move(args)), body_(std::move(body))
    {}

    const Ast& body() const& noexcept
    {
        return *body_;
    }

    constexpr const std::vector<Var>& args() const& noexcept
    {
        return args_;
    }
};

class List
{
private:
    std::vector<Val> values_;

public:
    explicit ELY_CONSTEXPR_VECTOR List(std::vector<Val> values)
        : values_(std::move(values))
    {}

    constexpr const std::vector<Val>& values() const&
    {
        return values_;
    }
};

class LexicalContext
{};

class SyntaxList
{
private:
    LexicalContext      ctx_;
    std::vector<Syntax> values_;

public:
    ELY_CONSTEXPR_VECTOR SyntaxList(LexicalContext      ctx,
                                    std::vector<Syntax> values)
        : ctx_(std::move(ctx)), values_(std::move(values))
    {}

    constexpr const LexicalContext& lexical_context() const
    {
        return ctx_;
    }

    constexpr const std::vector<Val>& values() const&
    {
        return values_;
    }

    ELY_CONSTEXPR_VECTOR List strip() const
    {
        std::vector<Val> vals;

        for (const auto& stx : values_)
        {
            vals.emplace_back(
                ely::visit(stx.strip(), [&](auto&& sym_lit_list) -> Val {
                    using strip_ty =
                        std::remove_cvref_t<decltype(sym_lit_list)>;

                    return Val(
                        std::in_place_type<strip_ty>,
                        static_cast<decltype(sym_lit_list)&&>(sym_lit_list));
                }));
        }

        return List(std::move(vals));
    }
};

class SyntaxLiteral
{
private:
    LexicalContext ctx_;
    Literal        lit_;

public:
    ELY_CONSTEXPR_STRING SyntaxLiteral(LexicalContext ctx, Literal lit)
        : ctx_(std::move(ctx)), lit_(std::move(lit))
    {}

    constexpr const LexicalContext& lexical_context() const
    {
        return ctx_;
    }

    constexpr Literal strip() const
    {
        return lit_;
    }
};

class Identifier
{
private:
    LexicalContext ctx_;
    Symbol         sym_;

public:
    constexpr(LexicalContext ctx, Symbol sym)
        : ctx_(std::move(ctx)), sym_(std::move(sym))
    {}

    constexpr const LexicalContext& lexical_context() const
    {
        return ctx_;
    }

    constexpr Symbol strip() const&
    {
        return sym_;
    }
};

class Syntax
{
private:
    ely::Variant<SyntaxLiteral, Identifier, SyntaxList> variants_;

public:
    ELY_CONSTEXPR_STRING Syntax(Literal a, LexicalContext ctx)
        : variants_(std::move(a)), ctx_(std::move(ctx))
    {}

    ELY_CONSTEXPR_STRING Syntax(Symbol s, LexicalContext ctx)
        : variants_(std::move(s)), ctx_(std::move(ctx))
    {}

    ELY_CONSTEXPR_VECTOR Syntax(SyntaxList sl, LexicalContext ctx)
        : variants_(std::move(sl)), ctx_(std::move(ctx))
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
    ely::Variant<Symbol, Literal, List> strip() const&
    {
        return visit([&](const auto& x) { return ely::Variant{x.strip()}; });
    }
};

class Val
{
private:
    ely::Variant<Fn, Atom, List, Syntax> variants_;

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
    ely::Variant<Call, Var, Val> variants_;

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
} // namespace ast
} // namespace ely