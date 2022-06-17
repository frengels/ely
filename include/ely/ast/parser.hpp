#pragma once

#include "ely/ast/ast.hpp"
#include "ely/lex/lexer.hpp"
#include "ely/sema/sema.hpp"

namespace ely
{
namespace ast
{
class parser
{
    ely::lexer lex_;
    ely::sema  sema_;
    ely::token lookahead_;

public:
    parser(ely::lexer lex, ely::sema sem)
        : lex_(std::move(lex)), sema_(std::move(sem))
    {
        lookahead_ = lex.next();
    }

private:
    ely::token peek_token() const;
    void       advance_token();
    ely::token consume_token();

    module_* parse_module();

    type*       parse_type();
    var*        parse_var();
    definition* parse_definition();

    fn_expression*    parse_fn();
    begin_expression* parse_begin();
    call*             parse_call();
    expression*       parse_lparen();
    expression*       parse_expression();

    integer_literal* parse_int_literal();
    float_literal*   parse_float_literal();

    if_expression* parse_if();

    binding*              parse_binding();
    std::vector<binding*> parse_bindings();
    let_expression*       parse_let_expression();

    expression* parse_struct_expression();
    expression* parse_ptr_expression();

public:
    module_* parse();
};
} // namespace ast
} // namespace ely