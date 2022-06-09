#include "ely/stx/parser.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "ely/lex/lexer.h"
#include "ely/stx/datum.h"

#define DEFAULT_TOKEN_BUF_SIZE 1024

typedef struct ely_stx_parser
{
    ely_lexer* lex;
    ely_token* token_buf;
    uint32_t   buf_cap;
    uint32_t   buf_head;
    uint32_t   buf_tail;
} ely_stx_parser;

ely_stx_parser* ely_stx_parser_create(ely_lexer* lex)
{
    ely_stx_parser* parser = malloc(sizeof(*parser));
    parser->lex            = lex;
    parser->buf_cap        = DEFAULT_TOKEN_BUF_SIZE;
    parser->token_buf      = malloc(sizeof(ely_token) * parser->buf_cap);
    parser->buf_head       = 0;
    parser->buf_tail       = 0;

    return parser;
}

void ely_stx_parser_destroy(ely_stx_parser* parser)
{
    free(parser->token_buf);
    free(parser);
}

static inline ely_token peek_token(ely_stx_parser* parser)
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

void ely_stx_parser_parse(ely_ilist* res_list, ely_stx_parser* parser)
{
    ely_ilist_init(res_list);

    parser->buf_tail =
        ely_lexer_scan_tokens(parser->lex, parser->token_buf, parser->buf_cap);
}