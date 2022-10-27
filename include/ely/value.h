#pragma once

#ifndef ELY_VALUE_H
#define ELY_VALUE_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "ely/config.h"
#include "ely/string.h"
#include "ely/type.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ely_value_kind
{
    // poisoned due to runtime error
    ELY_VALUE_POISON,

    ELY_VALUE_U64,
    ELY_VALUE_U32,
    ELY_VALUE_S64,
    ELY_VALUE_S32,

    ELY_VALUE_F32,
    ELY_VALUE_F64,

    ELY_VALUE_STRING_LIT,
    ELY_VALUE_INT_LIT,
    ELY_VALUE_DEC_LIT,
};

struct ely_value
{
    ely_value_kind kind;

    union
    {
        uint64_t   uval;
        int64_t    sval;
        float      f;
        double     d;
        ely_string lit;
    } as;
};

ELY_ALWAYS_INLINE struct ely_value ely_value_create_poison()
{
    return {ELY_VALUE_POISON, {}};
}

ELY_ALWAYS_INLINE struct ely_value ely_value_create_u64(uint64_t val)
{
    return {ELY_VALUE_U64, {.uval = val}};
}

ELY_ALWAYS_INLINE struct ely_value ely_value_create_u32(uint32_t val)
{
    return {ELY_VALUE_U32, {.uval = (uint64_t) val}};
}

ELY_ALWAYS_INLINE struct ely_value ely_value_create_s64(int64_t val)
{
    return ely_value{ELY_VALUE_S64, {.sval = val}};
}

ELY_ALWAYS_INLINE struct ely_value ely_value_create_s32(int32_t val)
{
    return {ELY_VALUE_S32, {.sval = (int64_t) val}};
}

ELY_ALWAYS_INLINE struct ely_value ely_value_create_f64(double d)
{
    return {ELY_VALUE_F64, {.d = d}};
}

ELY_ALWAYS_INLINE struct ely_value ely_value_create_f32(float f)
{
    return {ELY_VALUE_F32, {.f = f}};
}

ELY_ALWAYS_INLINE struct ely_value ely_value_create_int_literal(const char* str,
                                                                size_t      len)
{
    return {ELY_VALUE_INT_LIT, {.lit = ely_string_create_len(str, len)}};
}

ELY_ALWAYS_INLINE struct ely_value ely_value_create_dec_literal(const char* str,
                                                                size_t      len)
{
    return {ELY_VALUE_DEC_LIT, {.lit = ely_string_create_len(str, len)}};
}

ELY_ALWAYS_INLINE struct ely_value
ely_value_create_string_literal(const char* str, size_t len)
{
    return {ELY_VALUE_STRING_LIT, {.lit = ely_string_create_len(str, len)}};
}

ELY_ALWAYS_INLINE void ely_value_destroy(const struct ely_value* v)
{
    switch (v->kind)
    {
    case ELY_VALUE_STRING_LIT:
    case ELY_VALUE_INT_LIT:
    case ELY_VALUE_DEC_LIT:
        ely_string_destroy(v->as.lit);
        break;
    default:
        break;
    }
}

ELY_ALWAYS_INLINE struct ely_type ely_value_type(const struct ely_value* v)
{
    switch (v->kind)
    {
    case ELY_VALUE_POISON:
        return ely_type_create_poison();
    case ELY_VALUE_U64:
        return ely_type_create_u64();
    case ELY_VALUE_U32:
        return ely_type_create_u32();
    case ELY_VALUE_S64:
        return ely_type_create_s64();
    case ELY_VALUE_S32:
        return ely_type_create_s32();
    case ELY_VALUE_F64:
        return ely_type_create_f64();
    case ELY_VALUE_F32:
        return ely_type_create_f32();
    case ELY_VALUE_STRING_LIT:
        return ely_type_create_string_literal();
    case ELY_VALUE_INT_LIT:
        return ely_type_create_int_literal();
    case ELY_VALUE_DEC_LIT:
        return ely_type_create_dec_literal();
    default:
        assert(0 && "unreachable");
        return ely_type_create_poison();
    }
}

ELY_ALWAYS_INLINE bool ely_value_is_u64(const struct ely_value* v)
{
    return v->kind == ELY_VALUE_U64;
}

ELY_ALWAYS_INLINE bool ely_value_is_u32(const struct ely_value* v)
{
    return v->kind == ELY_VALUE_U32;
}

ELY_ALWAYS_INLINE bool ely_value_is_s64(const struct ely_value* v)
{
    return v->kind == ELY_VALUE_S64;
}

ELY_ALWAYS_INLINE bool ely_value_is_s32(const struct ely_value* v)
{
    return v->kind == ELY_VALUE_S32;
}

ELY_ALWAYS_INLINE bool ely_value_is_f64(const struct ely_value* v)
{
    return v->kind == ELY_VALUE_F64;
}

ELY_ALWAYS_INLINE bool ely_value_is_f32(const struct ely_value* v)
{
    return v->kind == ELY_VALUE_F32;
}

ELY_ALWAYS_INLINE uint64_t ely_value_get_u64(const struct ely_value* v)
{
    assert(ely_value_is_u64(v));
    return v->as.uval;
}

ELY_ALWAYS_INLINE uint32_t ely_value_get_u32(const struct ely_value* v)
{
    assert(ely_value_is_u32(v));
    return (uint32_t) v->as.uval;
}

ELY_ALWAYS_INLINE int64_t ely_value_get_s64(const struct ely_value* v)
{
    assert(ely_value_is_s64(v));
    return v->as.sval;
}

ELY_ALWAYS_INLINE int32_t ely_value_get_s32(const struct ely_value* v)
{
    assert(ely_value_is_s32(v));
    return (int32_t) v->as.sval;
}

ELY_ALWAYS_INLINE double ely_value_get_f64(const struct ely_value* v)
{
    assert(ely_value_is_f64(v));
    return v->as.d;
}

ELY_ALWAYS_INLINE float ely_value_get_f32(const struct ely_value* v)
{
    assert(ely_value_is_f32(v));
    return v->as.f;
}

ELY_EXPORT size_t ely_value_to_chars(const struct ely_value* v,
                                     char*                   buf,
                                     size_t                  buf_len);
ELY_EXPORT int    ely_value_print(const struct ely_value* v, FILE* f);

#ifdef __cplusplus
}
#endif

#endif
