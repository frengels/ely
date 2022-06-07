#include "ely/lex/token.h"

const char* ely_token_type_to_string(ely_token_type ty)
{
    switch (ty)
    {
#define X(val, str)                                                            \
    case val:                                                                  \
        return str;
#include "ely/lex/token.def"
#undef X
    default:
        return "<unknown>";
    }
}

int ely_token_print(FILE* f, const ely_token* tok)
{
    return fprintf(f,
                   "(token \"%s\" [len %d] [content %*.s] [pos %d:%d])",
                   ely_token_type_to_string(tok->type),
                   tok->len,
                   tok->len,
                   tok->start,
                   tok->pos.line,
                   tok->pos.col);
}