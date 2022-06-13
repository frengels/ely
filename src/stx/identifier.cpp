#include "ely/stx/identifier.hpp"

#include <stdlib.h>
#include <string.h>

ely_identifier
ely_identifier_create(const char* str, size_t len, const ely::position& pos)
{
    char* dst = static_cast<char*>(malloc(len + 1));
    memcpy(dst, str, len);
    dst[len] = '\0';

    ely_identifier res;
    res.pos        = pos;
    res.str        = dst;
    res.is_literal = false;

    return res;
}

void ely_identifier_destroy(ely_identifier* ident)
{
    free(ident->str);
}