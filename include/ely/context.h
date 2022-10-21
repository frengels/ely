#ifndef ELY_CONTEXT_H
#define ELY_CONTEXT_H

#include "ely/config.h"

typedef struct ely_context ely_context;

ELY_EXPORT ely_context* ely_context_create();
ELY_EXPORT void         ely_context_destroy(ely_context* ctx);

#endif