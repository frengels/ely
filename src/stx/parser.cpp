#include "ely/stx/parser.h"

#include <cassert>
#include <cstdlib>

#include "ely/lex/lexer.hpp"
#include "ely/stx/datum.hpp"

#define DEFAULT_TOKEN_BUF_SIZE 1024

typedef struct ely_stx_parser
{
    ely::lexer  lex;
    ely::token* token_buf;
    uint32_t    buf_cap;
    uint32_t    buf_head;
    uint32_t    buf_tail;
} ely_stx_parser;

ely_stx_parser* ely_stx_parser_create(ely::lexer lex)
{
    ely_stx_parser* parser =
        static_cast<ely_stx_parser*>(malloc(sizeof(*parser)));
    parser->lex     = lex;
    parser->buf_cap = DEFAULT_TOKEN_BUF_SIZE;
    parser->token_buf =
        static_cast<ely::token*>(malloc(sizeof(ely::token) * parser->buf_cap));
    parser->buf_head = 0;
    parser->buf_tail = 0;

    return parser;
}

void ely_stx_parser_destroy(ely_stx_parser* parser)
{
    free(parser->token_buf);
    free(parser);
}

static inline ely::token peek_token(ely_stx_parser* parser)
{
    return parser->token_buf[parser->buf_head];
}

static inline void advance_token(ely_stx_parser* parser)
{
    ++parser->buf_head;
}

static inline bool at_end(ely_stx_parser* parser)
{
    return parser->buf_head == parser->buf_tail;
}

static inline ely_datum* parse_parens_list(ely_stx_parser* parser)
{
    return nullptr;
}

static inline ely_datum* parse_bracket_list(ely_stx_parser* parser)
{
    return nullptr;
}

static inline ely_datum* parse_brace_list(ely_stx_parser* parser)
{
    return nullptr;
}

void ely_stx_parser_parse(ely_ilist* res_list, ely_stx_parser* parser)
{
    ely_ilist_init(res_list);

    parser->buf_tail =
        parser->lex.scan_tokens(parser->token_buf, parser->buf_cap);

    ely::token tok = peek_token(parser);

    ely_datum* datum;

    using ely::token_type;

    switch (tok.type)
    {
    case token_type::eof:
        return;
    case token_type::lparen:
        datum = parse_parens_list(parser);
        break;
    case token_type::lbracket:
        datum = parse_bracket_list(parser);
        break;
    case token_type::lbrace:
        datum = parse_brace_list(parser);
        break;
    case token_type::identifier: {
        auto str = std::string(tok.start, tok.len);
        datum    = ely_datum_create_identifier_str(std::move(str), tok.pos);
    }
    break;
    case token_type::string_literal:
        datum = ely_datum_create_string_literal(tok.start, tok.len, tok.pos);
        break;
    case token_type::int_literal:
        datum = ely_datum_create_int_literal(tok.start, tok.len, tok.pos);
        break;
    case token_type::decimal_literal:
        datum = ely_datum_create_dec_literal(tok.start, tok.len, tok.pos);
        break;
    case token_type::char_literal:
        datum = ely_datum_create_char_literal(tok.start, tok.len, tok.pos);
        break;
    case token_type::unknown_char:
    case token_type::unterminated_string:
    default:
        assert(false && "Unimplemented parsing cases");
        break;
    }

    ely_ilist_insert(res_list, &datum->link);
}