#pragma once

#ifndef ELY_TYPE_H
#define ELY_TYPE_H

#include "ely/config.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum ely_type_kind
{
    ELY_TYPE_POISON = 0,

    ELY_TYPE_U32,
    ELY_TYPE_U64,
    ELY_TYPE_S32,
    ELY_TYPE_S64,

    ELY_TYPE_F32,
    ELY_TYPE_F64,

    ELY_TYPE_STRING_LIT,
    ELY_TYPE_INT_LIT,
    ELY_TYPE_DEC_LIT,
};

struct ely_type
{
    enum ely_type_kind kind;
    union
    {
        char empty;
    } as;
};

ELY_ALWAYS_INLINE struct ely_type ely_type_create(enum ely_type_kind kind)
{
    struct ely_type res;
    res.kind = kind;
    return res;
}

ELY_EXPORT bool        ely_type_eq(const ely_type* lhs, const ely_type* rhs);
ELY_ALWAYS_INLINE bool ely_type_ne(const ely_type* lhs, const ely_type* rhs)
{
    return !ely_type_eq(lhs, rhs);
}

#ifdef __cplusplus
}
#endif

#endif