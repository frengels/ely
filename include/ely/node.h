#pragma once

#ifndef ELY_NODE_H
#define ELY_NODE_H

#include <stdint.h>
#include <stdlib.h>

#include "ely/config.h"
#include "ely/type.h"

struct ely_context;

#ifdef __cplusplus
extern "C" {
#endif

enum ely_node_kind
{
    // stx
    ELY_NODE_STX_FIRST = 0,
    ELY_NODE_STX_LIST  = ELY_NODE_STX_FIRST,
    ELY_NODE_STX_ID,

    // lits
    ELY_NODE_LIT_FIRST,
    ELY_NODE_LIT_STRING = ELY_NODE_LIT_FIRST,
    ELY_NODE_LIT_INT,
    ELY_NODE_LIT_DEC,

    ELY_NODE_LIT_S32,
    ELY_NODE_LIT_S64,
    ELY_NODE_LIT_U32,
    ELY_NODE_LIT_U64,
    ELY_NODE_LIT_F32,
    ELY_NODE_LIT_F64,

    ELY_NODE_LIT_LAST,
    ELY_NODE_STX_LAST = ELY_NODE_LIT_LAST,

    ELY_NODE_LIST = 100,
    // ELY_NODE_DEF, // replace with builtins
    // ELY_NODE_LET,
    // ELY_NODE_VAR,
    // ELY_NODE_FN,
    ELY_NODE_CALL_USER,
    ELY_NODE_CALL_BUILTIN,
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
// struct ely_let;
// struct ely_fn;
// struct ely_def;
// struct ely_var;
struct ely_call;
struct ely_user_call;
struct ely_prim_call;
// special literals which cannot be constructed from source
struct ely_s32_literal;
struct ely_s64_literal;
struct ely_u32_literal;
struct ely_u64_literal;
struct ely_f32_literal;
struct ely_f64_literal;

ELY_EXPORT uint32_t ely_node_ref(void* node);
ELY_EXPORT uint32_t ely_node_unref(void* node);

ELY_EXPORT struct ely_list* ely_list_create(struct ely_context* ctx);
ELY_EXPORT void ely_list_append(struct ely_list* list, struct ely_node* node);

ELY_EXPORT struct ely_stx_list* ely_stx_list_create(struct ely_context* ctx);
ELY_EXPORT void                 ely_stx_list_append(struct ely_stx_list* list,
                                                    struct ely_stx*      stx);

ELY_EXPORT struct ely_prim_call*
ely_prim_call_create(struct ely_context* ctx, const char* str, size_t len);
ELY_EXPORT void ely_prim_call_push_arg(struct ely_prim_call* call,
                                       struct ely_node*      n);

ELY_EXPORT struct ely_prim_call* ely_def_create(struct ely_context* ctx,
                                                const char*         name,
                                                size_t              len,
                                                struct ely_expr*    init)
{
    const char*           def_name = "def";
    struct ely_prim_call* def =
        ely_prim_call_create(ctx, def_name, __builtin_strlen(def_name));
    ely_node_ref(init);
    ely_prim_call_push_arg(def, (struct ely_node*) init);
}

ELY_EXPORT struct ely_string_literal*
ely_string_literal_create(struct ely_context* ctx,
                          const char*         text,
                          size_t              len);
ELY_EXPORT struct ely_int_literal*
ely_int_literal_create(struct ely_context* ctx, const char* str, size_t len);
ELY_EXPORT struct ely_dec_literal*
ely_dec_literal_create(struct ely_context* ctx, const char* str, size_t len);

ELY_EXPORT struct ely_s32_literal*
ely_s32_literal_create(struct ely_context* ctx, int32_t val);
ELY_EXPORT struct ely_s64_literal*
ely_s64_literal_create(struct ely_context* ctx, int64_t val);

ELY_EXPORT struct ely_let* ely_let_create(struct ely_context* ctx);
ELY_EXPORT void
ely_let_push(struct ely_let* l, struct ely_var* v, struct ely_expr* init);
ELY_EXPORT struct ely_fn* ely_fn_create(struct ely_context* ctx,
                                        struct ely_expr*    e);
ELY_EXPORT void           ely_fn_push_arg(struct ely_fn* f, struct ely_var* v);
ELY_EXPORT struct ely_var*
ely_var_create(struct ely_context* ctx, const char* name, size_t len);
ELY_EXPORT struct ely_string_view ely_var_name(const ely_var* v);
ELY_EXPORT struct ely_type        ely_var_type(const ely_var* v);

ELY_EXPORT struct ely_prim_call* ely_prim_call_create(struct ely_context* ctx,
                                                      enum ely_prim_kind  kind);

ELY_EXPORT void ely_prim_call_push_operand(struct ely_prim_call* pcall,
                                           struct ely_expr*      e);

#ifdef __cplusplus
}
#endif

#endif
