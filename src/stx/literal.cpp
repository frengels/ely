#include "ely/stx/literal.hpp"

#include <stdlib.h>
#include <string.h>

static inline ely_literal create_literal(const char*          str,
                                         size_t               len,
                                         const ely::position& pos,
                                         ely_literal_type     ty)
{
    char* dst = static_cast<char*>(malloc(len + 1));
    memcpy(dst, str, len);
    dst[len] = '\0';

    ely_literal res;
    res.pos      = pos;
    res.type     = ty;
    res.data.str = dst;

    return res;
}

ely_literal
ely_literal_create_int(const char* str, size_t len, const ely::position& pos)
{
    return create_literal(str, len, pos, ELY_LITERAL_INT);
}

ely_literal
ely_literal_create_dec(const char* str, size_t len, const ely::position& pos)
{
    return create_literal(str, len, pos, ELY_LITERAL_DEC);
}

ely_literal
ely_literal_create_string(const char* str, size_t len, const ely::position& pos)
{
    return create_literal(str, len, pos, ELY_LITERAL_STRING);
}

ely_literal
ely_literal_create_char(const char* str, size_t len, const ely::position& pos)
{
    return create_literal(str, len, pos, ELY_LITERAL_CHAR);
}

ely_literal ely_literal_create_bool(bool b, const ely::position& pos)
{
    ely_literal res;
    res.pos    = pos;
    res.type   = ELY_LITERAL_BOOL;
    res.data.b = b;

    return res;
}

void ely_literal_destroy(ely_literal* lit)
{
    switch (lit->type)
    {
    case ELY_LITERAL_INT:
    case ELY_LITERAL_DEC:
    case ELY_LITERAL_CHAR:
    case ELY_LITERAL_STRING:
        free(lit->data.str);
        break;
    case ELY_LITERAL_BOOL:
    default:
        break;
    }
}