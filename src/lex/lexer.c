#include "ely/lex/lexer.h"

#include <stdlib.h>

#include "common.h"
#include "ely/lex/token.h"

ely_lexer* ely_lexer_create(const char* src)
{
    ely_lexer* lex = malloc(sizeof(*lex));
    lex->src       = src;
    lex->offset    = 0;
    lex->line      = 1;
    lex->col       = 1;

    return lex;
}

void ely_lexer_destroy(ely_lexer* lex)
{
    free(lex);
}

ely_position ely_lexer_position(const ely_lexer* lex)
{
    return (ely_position){
        .offset = lex->offset, .line = lex->line, .col = lex->col};
}

bool ely_lexer_empty(const ely_lexer* lex)
{
    return lex->src[lex->offset] == '\0';
}
