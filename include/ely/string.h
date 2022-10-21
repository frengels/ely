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
    char*  str;
    size_t len;
};

ELY_EXPORT struct ely_string
                             ely_string_create(const char* s);
ELY_EXPORT struct ely_string ely_string_create_len(const char* s, size_t len);
ELY_EXPORT void              ely_string_destroy(ely_string s);

#ifdef __cplusplus
}
#endif

#endif
