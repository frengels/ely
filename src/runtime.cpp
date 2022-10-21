#include "ely/runtime.h"

#include <cstdlib>

struct ely_runtime
{};

ely_runtime* ely_runtime_create()
{
    ely_runtime* rt =
        static_cast<ely_runtime*>(std::malloc(sizeof(ely_runtime)));

    return rt;
}

void ely_runtime_destroy(ely_runtime* rt)
{
    std::free(static_cast<void*>(rt));
}