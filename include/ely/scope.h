#pragma once

#ifndef ELY_SCOPE_H
#define ELY_SCOPE_H

#include "ely/config.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ely_scope;

struct ely_value;

ELY_EXPORT struct ely_scope* ely_scope_create(struct ely_scope* parent,
                                              const char*       name);
ELY_EXPORT void              ely_scope_destroy(struct ely_scope* s);

ELY_EXPORT void
ely_scope_push(struct ely_scope* s, const char* name, struct ely_value* val);
ELY_EXPORT void ely_scope_lookup(const struct ely_scope* s,
                                 struct ely_string_view  str);

#ifdef __cplusplus
}
#endif

#endif