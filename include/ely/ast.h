#pragma once

#ifndef ELY_NODE_H
#define ELY_NODE_H

#include <stdint.h>
#include <stdlib.h>

#include "ely/config.h"

struct ely_context;

#ifdef __cplusplus
extern "C" {
#endif

enum ely_node_kind
{
    ELY_NODE_DEF,
    ELY_NODE_STX_LIST,
    ELY_NODE_STX_ID,
    ELY_NODE_LIST,
    ELY_NODE_LIT_STRING,
    ELY_NODE_LIT_INT,
    ELY_NODE_LIT_DEC,
    ELY_NODE_VAR,
    ELY_NODE_CALL,
    ELY_NODE_PRIM_CALL,
};

enum ely_prim_kind
{
    // literal conversions
    ELY_PRIM_F32,
    ELY_PRIM_F64,

    ELY_PRIM_U64,
    ELY_PRIM_U32,
    ELY_PRIM_U16,
    ELY_PRIM_U8,

    ELY_PRIM_I64,
    ELY_PRIM_I32,
    ELY_PRIM_I16,
    ELY_PRIM_I8,
};

struct ely_node_base
{
    uint16_t kind;
};

struct ely_node;
struct ely_expr;
struct ely_stx;
struct ely_stx_list;
struct ely_stx_id;
struct ely_string_literal;
struct ely_int_literal;
struct ely_dec_literal;
struct ely_literal;
struct ely_list;
struct ely_fn;
struct ely_def;
struct ely_var;
struct ely_call;
struct ely_prim_call;

ELY_EXPORT uint32_t ely_node_ref(void* node);
ELY_EXPORT uint32_t ely_node_deref(void* node);

ELY_EXPORT struct ely_list* ely_list_create(struct ely_context* ctx);
ELY_EXPORT void ely_list_append(struct ely_list* list, struct ely_node* node);

ELY_EXPORT struct ely_stx_list* ely_stx_list_create(struct ely_context* ctx);
ELY_EXPORT void                 ely_stx_list_append(struct ely_stx_list* list,
                                                    struct ely_stx*      stx);

ELY_EXPORT struct ely_def* ely_def_create(struct ely_context* ctx,
                                          const char*         name,
                                          size_t              len,
                                          ely_expr*           init);

ELY_EXPORT struct ely_string_literal*
ely_string_literal_create(struct ely_context* ctx,
                          const char*         text,
                          size_t              len);
ELY_EXPORT struct ely_int_literal*
ely_int_literal_create(struct ely_context* ctx, const char* str, size_t len);
ELY_EXPORT struct ely_dec_literal*
ely_dec_literal_create(struct ely_context* ctx, const char* str, size_t len);

ELY_EXPORT struct ely_var*
ely_var_create(struct ely_context* ctx, const char* name, size_t len);

ELY_EXPORT struct ely_prim_call* ely_prim_call_create(struct ely_context* ctx,
                                                      enum ely_prim_kind  kind);

ELY_EXPORT void ely_prim_call_push_operand(struct ely_prim_call* pcall,
                                           ely_expr*             e);

#ifdef __cplusplus
}
#endif

#endif
