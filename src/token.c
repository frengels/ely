#include "ely/token.h"

const char* ely_token_as_string(enum ElyTokenKind kind)
{
    switch (kind)
    {
#define X(item)                                                                \
    case item:                                                                 \
        return #item;
#include "ely/tokens.def"
    default:
        __builtin_unreachable();
    }
}