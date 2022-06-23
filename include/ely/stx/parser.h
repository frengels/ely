#pragma once

#include "ely/ilist.h"
#include "ely/lex/token.hpp"
#include "ely/lex/traits.hpp"
#include "ely/stx/source.h"

#include "ely/export.h"

namespace ely
{
template<typename Lexer>
class parser
{
public:
    using lexer_type    = Lexer;
    using token_type    = ely::token_t<lexer_type>;
    using location_type = ely::location_t<token_type>;

private:
    [[no_unique_address]] lexer_type lex_;
    token_type                       lookahead_;

public:
    explicit constexpr parser(const lexer_type& l)
        : lex_(l), lookahead_(lex_.next())
    {}

    explicit constexpr parser(lexer_type&& l)
        : lex_(std::move(l)), lookahead_(lex_.next())
    {}

    template<typename SrcHandler>
    constexpr void parse_source(SrcHandler h)
    {
        while (true)
        {
            switch (peek_token().kind())
            {
            case token_type::lparen: {
                auto list_h = h.make_parens_list_handler();
                parse_parens_list(lookahead_, list_h);
            }
            break;
            case token_type::eof:
                return;
            default:
                assert(false && "unimplemented parse");
                break;
            }
        }
    }

    template<typename ListHandler>
    constexpr void parse_parens_list(location_type loc, ListHandler& h)
    {
        advance_token();

        switch (peek_token().kind())
        {
        case token_type::rparen:
            h.close_list();
        default:
            parse_datum(peek_token().location(), h.make_datum_handler());
            break;
        }
    }

    template<typename DatumHandler>
    constexpr void parse_datum(location_type loc, DatumHandler& h)
    {
        const auto& t = peek_token();

        switch (t.kind())
        {
        case token_type::lparen:

        case token_type::lbracket:

        case token_type::lbrace:

        case token_type::identifier:

        case token_type::string_literal:

        case token_type::int_literal:

        case token_type::decimal_literal:

        case token_type::char_literal:

        default:
            break;
        }
    }

private:
    constexpr const token_type& peek_token() const
    {
        return lookahead_;
    }

    constexpr void advance_token()
    {
        lookahead_ = lex_.next();
    }
};
} // namespace ely