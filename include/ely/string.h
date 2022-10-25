#pragma once

#ifndef ELY_STRING_H
#define ELY_STRING_H

#include <stddef.h>

#include "ely/config.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ely_string
{
    char*  s;
    size_t len;
};

struct ely_string_view
{
    const char* s;
    size_t      len;
};

ELY_EXPORT struct ely_string ely_string_create(const char* s);
ELY_EXPORT struct ely_string ely_string_create_len(const char* s, size_t len);
ELY_EXPORT void              ely_string_destroy(ely_string s);

ELY_ALWAYS_INLINE struct ely_string_view ely_string_view_create(const char* s)
{
    return {s, __builtin_strlen(s)};
}

ELY_ALWAYS_INLINE struct ely_string_view
ely_string_view_create_len(const char* s, size_t len)
{
    return {s, len};
}

ELY_ALWAYS_INLINE struct ely_string_view
ely_string_view_from_string(struct ely_string str)
{
    return {str.s, str.len};
}

#ifdef __cplusplus
}
#endif

#endif
