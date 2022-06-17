#include "ely/ast/parser.hpp"

#include <llvm/ADT/APFloat.h>

namespace ely
{
namespace ast
{
ely::token parser::peek_token() const
{
    return lookahead_;
}

void parser::advance_token()
{
    lookahead_ = lex_.next();
}

ely::token parser::consume_token()
{
    auto res = peek_token();
    advance_token();
    return res;
}

module_* parser::parse_module()
{}

var* parser::parse_var()
{
    auto tok = peek_token(); // expecting `[`

    if (tok.kind() != token_type::lbracket)
    {
        return nullptr;
    }

    auto loc = tok.location();

    advance_token();

    tok = consume_token(); // expecting id name
    if (tok.kind() != token_type::identifier)
    {
        return nullptr;
    }

    auto name = tok.content();

    auto ty = parse_type();

    if (!ty)
    {
        return nullptr;
    }

    tok = consume_token(); // expecting `]`

    if (tok.kind() != token_type::rbracket)
    {
        return nullptr;
    }

    // return new var()
}

definition* parser::parse_definition()
{
    auto tok = peek_token();
    if (tok.kind() != token_type::lparen)
    {
        // TODO: error recovery
        return nullptr;
    }

    auto loc = tok.location();
    advance_token();

    tok = consume_token();
    if (tok.kind() != token_type::identifier)
    {
        return nullptr;
    }

    if (tok.content() != "define")
    {
        return nullptr;
    }
}

if_expression* parser::parse_if()
{
    auto tok = peek_token();
    assert(tok.content() == "if");
    auto if_id_loc = tok.location();
    advance_token();

    expression* cond = parse_expression();

    if (!cond)
    {
        return nullptr;
    }

    expression* then = parse_expression();

    if (!then)
    {
        return nullptr;
    }

    expression* alt = parse_expression();

    if (!alt)
    {
        return nullptr;
    }

    tok = consume_token();

    if (tok.kind() != ely::token_type::rparen)
    {
        return nullptr;
    }

    return new if_expression(*cond, *then, *alt, if_id_loc);
}

begin_expression* parser::parse_begin()
{
    auto tok = peek_token();
    assert(tok.content() == "begin");
    auto begin_id_loc = tok.location();
    advance_token();

    std::vector<expression*> exprs{};

    tok = peek_token();

    while (tok.kind() != ely::token_type::rparen)
    {
        expression* e = parse_expression();
        exprs.push_back(e);
        tok = peek_token();
    }

    return new begin_expression(std::move(exprs), begin_id_loc);
}

binding* parser::parse_binding()
{
    auto tok = peek_token();

    if (tok.kind() != token_type::lparen)
    {
        return nullptr;
    }
    advance_token();

    var* v = parse_var();

    if (!v)
    {
        return nullptr;
    }

    expression* init = parse_expression();

    if (!init)
    {
        return nullptr;
    }

    return new binding(*v, *init);
}

std::vector<binding*> parser::parse_bindings()
{
    auto tok = peek_token();
    if (tok.kind() != token_type::lparen)
    {
        return {};
    }
    advance_token();

    std::vector<binding*> bindings{};

    tok = peek_token();

    while (tok.kind() == token_type::lparen)
    {
        binding* b = parse_binding();
        if (!b)
        {
            return {};
        }

        bindings.push_back(b);
        tok = peek_token();
    }

    if (tok.kind() != token_type::rparen)
    {
        return {};
    }
    advance_token();

    return bindings;
}

let_expression* parser::parse_let_expression()
{
    auto tok = peek_token();
    assert(tok.content() == "let");
    auto let_id_loc = tok.location();
    advance_token();

    std::vector<binding*> bindings{};

    tok = peek_token();

    if (tok.kind() != token_type::lparen) // first (
    {
        return nullptr;
    }
    advance_token();

    tok = peek_token();
    if (tok.kind() != token_type::lparen) // second (
    {
        return nullptr;
    }
}

fn_expression* parser::parse_fn()
{
    auto tok = peek_token();
    assert(tok.content() == "fn");
    auto id_loc = tok.location();
    advance_token();

    std::vector<parameter*> params{};

    tok = consume_token();

    if (tok.kind() != token_type::lparen)
    {
        return nullptr;
    }

    tok = peek_token();
    while (tok.kind() != token_type::rparen)
    {
        auto v = parse_var();
        if (!v)
        {
            return nullptr;
        }
        params.push_back(v);
        tok = peek_token();
    }
    advance_token();

    auto body = parse_expression();

    if (!body)
    {
        return nullptr;
    }

    return new fn_expression{{}, std::move(params), *body, id_loc};
}

call* parser::parse_call()
{
    auto tok = peek_token();

    auto id_loc = tok.location();
    // we're past the first ( at this point

    auto callee = parse_expression();
    if (!callee)
    {
        return nullptr;
    }

    std::vector<expression*> operands{};

    tok = peek_token();
    while (tok.kind() != token_type::rparen)
    {
        auto e = parse_expression();
        if (!e)
        {
            return nullptr;
        }
        operands.push_back(e);
        tok = peek_token();
    }

    return new call(*callee, std::move(operands), id_loc);
}

expression* parser::parse_lparen()
{
    auto tok = peek_token();

    if (tok.kind() != token_type::lparen)
    {
        return nullptr;
    }
    auto loc = tok.location();
    advance_token();

    tok = peek_token();

    // expected callee
    if (tok.kind() == token_type::rparen)
    {
        return nullptr;
    }
    else if (tok.kind() == token_type::identifier)
    {
        auto str = tok.content();

        assert(str != "define");
        if (str == "fn")
        {
            return parse_fn();
        }
        else if (str == "begin")
        {
            return parse_begin();
        }
        else if (str == "if")
        {
            return parse_if();
        }
        else if (str == "let")
        {
            return parse_let_expression();
        }
        else if (str == "struct")
        {
            return parse_struct_expression();
        }
        else if (str == "ptr")
        {
            return parse_ptr_expression();
        }
    }

    return parse_call();
}

integer_literal* parser::parse_int_literal()
{
    auto tok = peek_token();

    auto loc = tok.location();

    if (tok.kind() != token_type::int_literal)
    {
        return nullptr;
    }

    std::string_view str = tok.content();

    auto sref = llvm::StringRef{str.data(), str.size()};
    auto val  = llvm::APInt(64, sref, 10);

    advance_token();

    return new integer_literal{llvm::APSInt(val, false), loc};
}

float_literal* parser::parse_float_literal()
{
    auto tok = peek_token();
    auto loc = tok.location();

    if (tok.kind() != token_type::decimal_literal)
    {
        return nullptr;
    }

    std::string_view str = tok.content();

    auto sref = llvm::StringRef{str.data(), str.size()};
    auto val  = llvm::APFloat(llvm::APFloatBase::IEEEsingle(), sref);

    advance_token();

    return new float_literal{val, loc};
}

expression* parser::parse_expression()
{
    auto tok = peek_token();

    if (tok.kind() == token_type::lparen)
    {
        return parse_call();
    }
    else if (tok.kind() == token_type::int_literal)
    {
        return parse_int_literal();
    }
    else if (tok.kind() == token_type::decimal_literal)
    {
        return parse_float_literal();
    }
}

module_* parser::parse()
{
    return parse_module();
}
} // namespace ast
} // namespace ely