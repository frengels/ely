#ifndef ELY_RUNTIME_H
#define ELY_RUNTIME_H

#include <cstdio>

#include "ely/config.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ely_expr;

struct ely_runtime;
struct ely_value;

ELY_EXPORT struct ely_runtime* ely_runtime_create();
ELY_EXPORT void                ely_runtime_destroy(struct ely_runtime* rt);

ELY_EXPORT struct ely_value* ely_runtime_eval(struct ely_runtime* rt,
                                              struct ely_expr*    e);
ELY_EXPORT void              ely_runtime_push_def(struct ely_runtime* rt,
                                                  const char*         name,
                                                  struct ely_value*   val);

ELY_EXPORT void   ely_value_print(const struct ely_value* v, FILE* f);
ELY_EXPORT size_t ely_value_to_chars(const struct ely_value* v,
                                     char*                   str,
                                     size_t                  len);

#ifdef __cplusplus
}
#endif

#endif