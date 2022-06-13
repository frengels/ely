#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#include <ely/lex/lexer.hpp>
#include <ely/lex/token.hpp>

#define TOKENS_LEN 1024

int main(int argc, char** argv)
{
    ely::token  tokens_dst[TOKENS_LEN];
    ely::lexer* lex     = new ely::lexer("hello");
    uint32_t    written = lex->scan_tokens(tokens_dst, TOKENS_LEN);
    assert(written == 1);
    assert(tokens_dst[0].type == ely::token_type::identifier);
    assert(lex->empty());

    delete lex;
    // assert(scan(NULL, "hello").type == ELY_TOKEN_IDENTIFIER);
    // fprintf(stderr, "%d\n", scan(NULL, "").type);

    // assert(scan(NULL, "(").type == ELY_TOKEN_LPAREN);
    // assert(scan(NULL, "1234").type == ELY_TOKEN_INT);
    // assert(scan(NULL, "1234.4321").type == ELY_TOKEN_DEC);
}