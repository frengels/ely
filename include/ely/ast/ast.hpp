#pragma once

#include <string_view>
#include <vector>

#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/APSInt.h>
#include <llvm/Support/SMLoc.h>

namespace ely
{
class type;
class expression;

class definition
{
    llvm::SMLoc      loc_;
    std::string_view name_;
    expression*      init_;

public:
    definition(std::string_view name, expression& init, llvm::SMLoc loc)
        : loc_(loc), name_(name), init_(std::addressof(init))
    {}

    llvm::SMLoc location() const
    {
        return loc_;
    }

    std::string_view name() const
    {
        return name_;
    }

    const expression& init() const
    {
        return *init_;
    }
};

class expression
{
public:
    enum expr_kind
    {
        module_,
        let,
        begin,
        fn,
        type,
        int_lit,
        float_lit,
        if_,
        call,
        struct_,
        ptr,
    };

private:
    expr_kind   kind_;
    llvm::SMLoc loc_;

public:
    expression(expr_kind k, llvm::SMLoc loc) : kind_(k), loc_(loc)
    {}

    constexpr expr_kind kind() const
    {
        return kind_;
    }

    llvm::SMLoc location() const
    {
        return loc_;
    }
};

class module_ : public expression
{
    std::vector<definition*> definitions_;

public:
    module_(std::vector<definition*> definitions, llvm::SMLoc loc)
        : expression(expression::module_, loc),
          definitions_(std::move(definitions))
    {}

    const std::vector<definition*>& definitions() const
    {
        return definitions_;
    }

    static constexpr bool classof(const expression* e)
    {
        return e->kind() == expression::module_;
    }
};

class parameter
{
    llvm::SMLoc      loc_;
    std::string_view name_;
    type*            ty_;

public:
    parameter(std::string_view name, type* ty, llvm::SMLoc loc)
        : loc_(loc), name_(name), ty_(ty)
    {}

    std::string_view name() const
    {
        return name_;
    }

    type& ty() const
    {
        return *ty_;
    }

    llvm::SMLoc location() const
    {
        return loc_;
    }
};

using var = parameter;

class binding
{
    var*        v_;
    expression* init_;

public:
    binding(var& v, expression& init)
        : v_(std::addressof(v)), init_(std::addressof(init))
    {}

    llvm::SMLoc location() const
    {
        return v_->location();
    }

    std::string_view name() const
    {
        return v_->name();
    }

    const type& ty() const
    {
        return v_->ty();
    }

    const expression& init() const
    {
        return *init_;
    }
};

class let_expression : public expression
{
    std::vector<binding*> bindings_;
    expression*           body_;

public:
    let_expression(std::vector<binding*> bindings,
                   expression&           body,
                   llvm::SMLoc           loc)
        : expression(expression::let, loc), bindings_(std::move(bindings)),
          body_(std::addressof(body))
    {}

    const std::vector<binding*>& bindings() const
    {
        return bindings_;
    }

    const expression& body() const
    {
        return *body_;
    }

    static constexpr bool classof(const expression* e)
    {
        return e->kind() == expression::let;
    }
};

class begin_expression : public expression
{
    std::vector<expression*> exprs_;

public:
    begin_expression(std::vector<expression*> exprs, llvm::SMLoc loc)
        : expression(expression::begin, loc), exprs_(std::move(exprs))
    {}

    const std::vector<expression*>& expressions() const
    {
        return exprs_;
    }

    static constexpr bool classof(const expression* e)
    {
        return e->kind() == expression::begin;
    }
};

class fn_expression : public expression
{
    std::string_view        name_;
    std::vector<parameter*> params_;
    expression*             body_;

public:
    fn_expression(std::string_view        name,
                  std::vector<parameter*> params,
                  expression&             body,
                  llvm::SMLoc             loc)
        : expression(expression::fn, loc), name_(name),
          params_(std::move(params)), body_(std::addressof(body))
    {}

    std::string_view name() const
    {
        return name_;
    }

    const std::vector<parameter*>& params() const
    {
        return params_;
    }

    const expression* body() const
    {
        return body_;
    }

    static constexpr bool classof(const expression* e)
    {
        return e->kind() == expression::fn;
    }
};

class integer_literal : public expression
{
    llvm::APSInt val_;

public:
    integer_literal(llvm::APSInt val, llvm::SMLoc loc)
        : expression(expression::int_lit, loc), val_(std::move(val))
    {}

    const llvm::APSInt& value() const
    {
        return val_;
    }

    static constexpr bool classof(const expression* e)
    {
        return e->kind() == expression::int_lit;
    }
};

class float_literal : public expression
{
    llvm::APFloat val_;

public:
    float_literal(llvm::APFloat val, llvm::SMLoc loc)
        : expression(expression::float_lit, loc), val_(std::move(val))
    {}

    const llvm::APFloat& value() const
    {
        return val_;
    }

    static constexpr bool classof(const expression* e)
    {
        return e->kind() == expression::float_lit;
    }
};

class if_expression : public expression
{
    expression* cond_;
    expression* then_;
    expression* alt_;

public:
    if_expression(expression& cond,
                  expression& then,
                  expression& alt,
                  llvm::SMLoc loc)
        : expression(expression::if_, loc), cond_(std::addressof(cond)),
          then_(std::addressof(then)), alt_(std::addressof(alt))
    {}

    expression& cond()
    {
        return *cond_;
    }

    expression& then()
    {
        return *then_;
    }

    expression& alt()
    {
        return *alt_;
    }

    static constexpr bool classof(const expression* e)
    {
        return e->kind() == expression::if_;
    }
};

class call : public expression
{
    expression*              callee_;
    std::vector<expression*> operands_;

public:
    call(expression& callee, std::vector<expression*> operands, llvm::SMLoc loc)
        : expression(expression::call, loc), callee_(std::addressof(callee)),
          operands_(std::move(operands))
    {}

    expression& callee()
    {
        return *callee_;
    }

    const std::vector<expression*>& operands() const
    {
        return operands_;
    }

    static constexpr bool classof(const expression* e)
    {
        return e->kind() == expression::call;
    }
};

class field
{
    std::string_view name_;
    expression*      type_;
};

class struct_expression : public expression
{
    std::string_view    name_;
    std::vector<field*> fields_;

public:
    struct_expression(std::string_view    name,
                      std::vector<field*> fields,
                      llvm::SMLoc         loc)
        : expression(expression::struct_, loc)
    {}

    constexpr std::string_view name() const
    {
        return name_;
    }

    const std::vector<field*>& fields() const
    {
        return fields_;
    }
};

class ptr_expression : public expression
{
    expression* target_;

public:
    ptr_expression(expression& target, llvm::SMLoc loc)
        : expression(expression::ptr, loc)
    {}

    expression& target()
    {
        return *target_;
    }
};
} // namespace ely