#ifndef ELY_CONTEXT_H
#define ELY_CONTEXT_H

#include "ely/config.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ely_context;

ELY_EXPORT struct ely_context* ely_context_create();
ELY_EXPORT void                ely_context_destroy(struct ely_context* ctx);

#ifdef __cplusplus
}
#endif

#endif