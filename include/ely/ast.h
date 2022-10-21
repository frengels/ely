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
    ELY_NODE_VAR
};

struct ely_node_base
{
    uint16_t kind;
};

struct ely_node;
struct ely_expr;
struct ely_stx;
struct ely_string_literal;
struct ely_int_literal;
struct ely_dec_literal;
struct ely_literal;
struct ely_list;
struct ely_fn;
struct ely_def;

struct ely_var;

ELY_EXPORT uint32_t ely_node_ref(void* node);
ELY_EXPORT uint32_t ely_node_deref(void* node);

ELY_EXPORT struct ely_list* ely_list_create(struct ely_context* ctx);

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

#ifdef __cplusplus
}
#endif

#endif
