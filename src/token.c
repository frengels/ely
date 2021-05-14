#include "ely/token.h"

const char* ely_token_as_string(enum ElyTokenKind kind)
{
    switch (kind)
    {
#define X(enu, item, _1)                                                       \
    case ELY_TOKEN_##enu:                                                      \
        return #item;
#include "ely/tokens.def"
    default:
        __builtin_unreachable();
    }
}

const char* ely_token_as_pretty_string(enum ElyTokenKind kind)
{
    switch (kind)
    {
#define X(enu, _1, name)                                                       \
    case ELY_TOKEN_##enu:                                                      \
        return name;
#include "ely/tokens.def"
    default:
        __builtin_unreachable();
    }
}