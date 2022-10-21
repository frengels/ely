#include "ely/context.h"

struct ely_context
{};

ely_context* ely_context_create()
{
    return new ely_context();
}

void ely_context_destroy(ely_context* ctx)
{
    delete ctx;
}