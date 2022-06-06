#ifndef ELY_STX_CONTEXT_H
#define ELY_STX_CONTEXT_H

#include "ely/stx/identifier.h"
#include "ely/stx/list.h"
#include "ely/stx/literal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    ely_stx_literal (*create_string_literal)(void*       self,
                                             const char* str,
                                             size_t      len);
    ely_stx_literal (*create_bool_literal)(void* self, bool b);
    ely_stx_literal (*create_number_literal)(void*       self,
                                             const char* str,
                                             size_t      len);
    ely_stx_identifier (*create_identifier)(void*       self,
                                            const char* str,
                                            size_t      len);
} ely_stx_context_interface;

typedef struct
{
    ely_stx_context_interface* interface;
    void*                      impl;
} ely_stx_context;

static inline ely_stx_literal
ely_stx_create_string_literal(ely_stx_context ctx, const char* str, size_t len)
{
    return ctx.interface->create_string_literal(ctx.impl, str, len);
}

static inline ely_stx_literal
ely_stx_create_number_literal(ely_stx_context ctx, const char* str, size_t len)
{
    return ctx.interface->create_number_literal(ctx.impl, str, len);
}

static inline ely_stx_literal ely_stx_create_bool_literal(ely_stx_context ctx,
                                                          bool            b)
{
    return ctx.interface->create_bool_literal(ctx.impl, b);
}

static inline ely_stx_identifier
ely_stx_create_identifier(ely_stx_context ctx, const char* str, size_t len)
{
    return ctx.interface->create_identifier(ctx.impl, str, len);
}

#ifdef __cplusplus
}
#endif

#endif