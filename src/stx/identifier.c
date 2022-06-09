#include "ely/stx/identifier.h"

#include <stdlib.h>
#include <string.h>

ely_identifier
ely_identifier_create(const char* str, size_t len, const ely_position* pos)
{
    char* dst = malloc(len + 1);
    memcpy(dst, str, len);
    dst[len] = '\0';

    ely_identifier res;
    res.pos        = *pos;
    res.str        = dst;
    res.is_literal = false;

    return res;
}

void ely_identifier_destroy(ely_identifier* ident)
{
    free(ident->str);
}