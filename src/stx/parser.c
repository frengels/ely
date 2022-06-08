#include "ely/stx/parser.h"

#include <stdlib.h>

#define DEFAULT_TOKEN_BUF_SIZE 1024

typedef struct ely_stx_parser
{
    ely_lexer* lex;
    ely_token* token_buf;
    size_t     buf_cap;
} ely_stx_parser;

ely_stx_parser* ely_stx_parser_create(ely_lexer* lex)
{
    ely_stx_parser* parser = malloc(sizeof(*parser));
    parser->lex            = lex;
    parser->token_buf      = malloc(sizeof(ely_token) * DEFAULT_TOKEN_BUF_SIZE);
    parser->buf_cap        = DEFAULT_TOKEN_BUF_SIZE;

    return parser;
}

void ely_stx_parser_destroy(ely_stx_parser* parser)
{
    free(parser->token_buf);
    free(parser);
}