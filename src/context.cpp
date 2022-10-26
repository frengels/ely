#include "ely/context.h"

#include <cstdint>

struct ely_context
{
    std::uint32_t ref_count{1};

    ely_context() = default;
};

ely_context* ely_context_create()
{
    return new ely_context();
}

void ely_context_acquire(ely_context* ctx)
{
    ++ctx->ref_count;
}

void ely_context_release(ely_context* ctx)
{
    if (--ctx->ref_count == 0)
    {
        delete ctx;
    }
}