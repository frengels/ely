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
    enum kind
    {
        let,
        begin,
        fn,
        type,
        int_lit,
        float_lit,
        if_,
        call,
    };

private:
    kind        kind_;
    llvm::SMLoc loc_;

public:
    expression(kind k, llvm::SMLoc loc) : kind_(k), loc_(loc)
    {}

    llvm::SMLoc location() const
    {
        return loc_;
    }
};

class binding
{
    llvm::SMLoc      loc_;
    std::string_view name_;
    type*            ty_;
    expression*      init_;

public:
    binding(std::string_view name, type& ty, expression& init, llvm::SMLoc loc)
        : loc_(loc), name_(name), ty_(std::addressof(ty)),
          init_(std::addressof(init))
    {}

    llvm::SMLoc location() const
    {
        return loc_;
    }

    std::string_view name() const
    {
        return name_;
    }

    const type& ty() const
    {
        return *ty_;
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

    llvm::SMLoc location() const
    {
        return loc_;
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
};

class if_ : public expression
{
    expression* cond_;
    expression* then_;
    expression* alt_;

public:
    if_(expression& cond, expression& then, expression& alt, llvm::SMLoc loc)
        : expression(expression::if_, loc), cond_(std::addressof(cond)),
          then_(std::addressof(then)), alt_(std::addressof(alt))
    {}
};

class call : public expression
{
    fn_expression*           callee_;
    std::vector<expression*> operands_;

public:
    call(fn_expression&           callee,
         std::vector<expression*> operands,
         llvm::SMLoc              loc)
        : expression(expression::call, loc), callee_(std::addressof(callee)),
          operands_(std::move(operands))
    {}

    fn_expression& callee()
    {
        return *callee_;
    }

    const std::vector<expression*>& operands() const
    {
        return operands_;
    }
};
} // namespace ely