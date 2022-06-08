#include "ely/stx/identifier.h"

#include <stdlib.h>
#include <string.h>

ely_stx_identifier
ely_stx_identifier_create(const char* str, size_t len, const ely_position* pos)
{
    char* dst = malloc(len + 1);
    memcpy(dst, str, len);
    dst[len] = '\0';

    ely_stx_identifier res;
    res.pos        = *pos;
    res.str        = dst;
    res.is_literal = false;

    return res;
}

void ely_stx_identifier_destroy(ely_stx_identifier* ident)
{
    free(ident->str);
}