#include "ely/type.h"

bool ely_type_eq(const ely_type* lhs, const ely_type* rhs)
{
    return lhs->kind == rhs->kind;
}