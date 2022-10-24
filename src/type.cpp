#include "ely/type.h"

bool ely_type_match(const ely_type* lhs, const ely_type* rhs)
{
    if (lhs->kind == ELY_TYPE_GENERIC || rhs->kind == ELY_TYPE_GENERIC)
    {
        return true;
    }

    return ely_type_eq(lhs, rhs);
}

bool ely_type_eq(const ely_type* lhs, const ely_type* rhs)
{
    return lhs->kind == rhs->kind;
}