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