#ifndef ELY_RUNTIME_H
#define ELY_RUNTIME_H

#include <cstdio>

#include "ely/config.h"
#include "ely/type.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ely_expr;

struct ely_runtime;
struct ely_value;

ELY_EXPORT struct ely_runtime* ely_runtime_create();
ELY_EXPORT void                ely_runtime_destroy(struct ely_runtime* rt);

ELY_EXPORT struct ely_value ely_runtime_eval(struct ely_runtime* rt,
                                             struct ely_expr*    e);
ELY_EXPORT void             ely_runtime_push_def(struct ely_runtime* rt,
                                                 const char*         name,
                                                 struct ely_value*   val);
ELY_EXPORT int
ely_runtime_emit_err(struct ely_runtime* rt, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif