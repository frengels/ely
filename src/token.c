#include "ely/token.h"

#include <string.h>

ElyStringView ely_token_as_string(enum ElyTokenKind kind)
{
    switch (kind)
    {
#define X(enu, item, _1)                                                       \
    case ELY_TOKEN_##enu: {                                                    \
        ElyStringView res = {.data = #item, .len = strlen(#item)};             \
        return res;                                                            \
    }

#include "ely/tokens.def"
    default:
        __builtin_unreachable();
    }
}

ElyStringView ely_token_as_pretty_string(enum ElyTokenKind kind)
{
    switch (kind)
    {
#define X(enu, _1, name)                                                       \
    case ELY_TOKEN_##enu: {                                                    \
        ElyStringView res = {.data = name, .len = strlen(name)};               \
        return res;                                                            \
    }

#include "ely/tokens.def"
    default:
        __builtin_unreachable();
    }
}