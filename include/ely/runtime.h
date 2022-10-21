#ifndef ELY_RUNTIME_H
#define ELY_RUNTIME_H

#include "ely/config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_runtime ely_runtime;

ELY_EXPORT ely_runtime* ely_runtime_create();
ELY_EXPORT void         ely_runtime_destroy(ely_runtime* rt);

ELY_EXPORT void ely_runtime_eval();

#ifdef __cplusplus
}
#endif

#endif