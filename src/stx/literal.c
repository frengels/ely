#include "ely/stx/literal.h"

#include <stdlib.h>
#include <string.h>

static inline ely_stx_literal create_literal(const char*          str,
                                             size_t               len,
                                             const ely_position*  pos,
                                             ely_stx_literal_type ty)
{
    char* dst = malloc(len + 1);
    memcpy(dst, str, len);
    dst[len] = '\0';

    ely_stx_literal res;
    res.pos      = *pos;
    res.type     = ty;
    res.data.str = dst;

    return res;
}

ely_stx_literal
ely_stx_literal_create_int(const char* str, size_t len, const ely_position* pos)
{
    return create_literal(str, len, pos, ELY_STX_LITERAL_INT);
}

ely_stx_literal
ely_stx_literal_create_dec(const char* str, size_t len, const ely_position* pos)
{
    return create_literal(str, len, pos, ELY_STX_LITERAL_DEC);
}

ely_stx_literal ely_stx_literal_create_string(const char*         str,
                                              size_t              len,
                                              const ely_position* pos)
{
    return create_literal(str, len, pos, ELY_STX_LITERAL_STRING);
}

ely_stx_literal ely_stx_literal_create_char(const char*         str,
                                            size_t              len,
                                            const ely_position* pos)
{
    return create_literal(str, len, pos, ELY_STX_LITERAL_CHAR);
}

ely_stx_literal ely_stx_literal_create_bool(bool b, const ely_position* pos)
{
    ely_stx_literal res;
    res.pos    = *pos;
    res.type   = ELY_STX_LITERAL_BOOL;
    res.data.b = b;

    return res;
}

void ely_stx_literal_destroy(ely_stx_literal* lit)
{
    switch (lit->type)
    {
    case ELY_STX_LITERAL_INT:
    case ELY_STX_LITERAL_DEC:
    case ELY_STX_LITERAL_CHAR:
    case ELY_STX_LITERAL_STRING:
        free(lit->data.str);
    case ELY_STX_LITERAL_BOOL:
    default:
        break;
    }
}