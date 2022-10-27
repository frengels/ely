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

    // an unspecified type, subject to type resolution
    ELY_TYPE_GENERIC,
    // ELY_TYPE_TYPE, // TODO

    ELY_TYPE_VOID,

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

ELY_ALWAYS_INLINE struct ely_type ely_type_create_poison()
{
    return {ELY_TYPE_POISON, {}};
}

ELY_ALWAYS_INLINE struct ely_type ely_type_create_generic()
{
    return {ELY_TYPE_GENERIC, {}};
}

ELY_ALWAYS_INLINE struct ely_type ely_type_create_u64()
{
    return {ELY_TYPE_U64, {}};
}

ELY_ALWAYS_INLINE struct ely_type ely_type_create_u32()
{
    return {ELY_TYPE_U32, {}};
}

ELY_ALWAYS_INLINE struct ely_type ely_type_create_s64()
{
    return {ELY_TYPE_S64, {}};
}

ELY_ALWAYS_INLINE struct ely_type ely_type_create_s32()
{
    return {ELY_TYPE_S32, {}};
}

ELY_ALWAYS_INLINE struct ely_type ely_type_create_f64()
{
    return {ELY_TYPE_F64, {}};
}

ELY_ALWAYS_INLINE struct ely_type ely_type_create_f32()
{
    return {ELY_TYPE_F32, {}};
}

ELY_ALWAYS_INLINE struct ely_type ely_type_create_string_literal()
{
    return {ELY_TYPE_STRING_LIT, {}};
}

ELY_ALWAYS_INLINE struct ely_type ely_type_create_int_literal()
{
    return {ELY_TYPE_INT_LIT, {}};
}

ELY_ALWAYS_INLINE struct ely_type ely_type_create_dec_literal()
{
    return {ELY_TYPE_DEC_LIT, {}};
}

ELY_ALWAYS_INLINE bool ely_type_eq(const struct ely_type* lhs,
                                   const struct ely_type* rhs)
{
    return lhs->kind == rhs->kind;
}

ELY_ALWAYS_INLINE bool ely_type_ne(const struct ely_type* lhs,
                                   const struct ely_type* rhs)
{
    return !ely_type_eq(lhs, rhs);
}

ELY_ALWAYS_INLINE bool ely_type_match(const ely_type* lhs, const ely_type* rhs)
{
    if (lhs->kind == ELY_TYPE_GENERIC && rhs->kind == ELY_TYPE_GENERIC)
    {
        return false;
    }

    if (lhs->kind == ELY_TYPE_GENERIC || rhs->kind == ELY_TYPE_GENERIC)
    {
        return true;
    }

    return ely_type_eq(lhs, rhs);
}

#ifdef __cplusplus
}
#endif

#endif