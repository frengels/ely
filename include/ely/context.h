#ifndef ELY_CONTEXT_H
#define ELY_CONTEXT_H

#include "ely/config.h"

#ifdef __cplusplus
extern "C" {
#endif

// this context is used when creating ast nodes, for memory management,
// representing the nodes, serializing etc
struct ely_context;

ELY_EXPORT struct ely_context* ely_context_create();

ELY_EXPORT void ely_context_acquire(struct ely_context* ctx);
ELY_EXPORT void ely_context_release(struct ely_context* ctx);

#ifdef __cplusplus
}
#endif

#endif