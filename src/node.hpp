#pragma once

#include <cstdint>

#include "ely/ilist.h"
#include "ely/node.h"

struct ely_node
{
    ely_node_base base;
    uint32_t      ref_count;
    ely_ilist     link{};
    ely_context*  ctx;
    ely_node*     expanded_from;

    ely_node(ely_context& ctx, ely_node_kind kind);
    ely_node(ely_context& ctx, ely_node_kind kind, ely_node* expanded);
    ~ely_node();

    uint32_t ref();
    uint32_t deref();
};

struct ely_expr : ely_node
{
    using ely_node::ely_node;
};

struct ely_stx : ely_expr
{
    using ely_expr::ely_expr;
};

struct ely_stx_list : ely_stx
{
    using base = ely_stx;

    ely_ilist head;

    ely_stx_list(ely_context& ctx);
    ~ely_stx_list();

    void push_back(ely_stx* stx);
};

struct ely_stx_id : ely_stx
{
    using base = ely_stx;

    char*  name;
    size_t len;

    ely_stx_id(ely_context& ctx, const char* name, size_t len);
    ~ely_stx_id();
};

struct ely_def : ely_node
{
    char*     name;
    size_t    len;
    ely_expr* init;

    ely_def(ely_context& ctx, const char* name, size_t len, ely_expr* init);
    ~ely_def();
};

struct ely_list : ely_expr
{
    using base = ely_expr;

    ely_ilist head;

    ely_list(ely_context& ctx);
    ~ely_list();

    void push_back(ely_node* node);
};

struct ely_literal : ely_expr
{
    using base = ely_expr;

    char*  str;
    size_t len;

    ely_literal(ely_context&  ctx,
                ely_node_kind kind,
                const char*   str,
                size_t        len);
    ~ely_literal();
};

struct ely_string_literal : ely_literal
{
    using ely_literal::ely_literal;
};

struct ely_int_literal : ely_literal
{
    using ely_literal::ely_literal;
};

struct ely_dec_literal : ely_literal
{
    using ely_literal::ely_literal;
};

struct ely_var : ely_expr
{
    using base = ely_expr;

    char*  name;
    size_t len;

    ely_var(ely_context& ctx, const char* name, size_t len);
    ~ely_var();
};

struct ely_call : ely_expr
{
    using base = ely_expr;

    ely_expr* op;
    ely_ilist operands_head;

    ely_call(ely_context& ctx);
    ~ely_call();

    void push_operand(ely_expr* operand);
};

struct ely_prim_call : ely_expr
{
    using base = ely_expr;

    ely_prim_kind kind;
    ely_ilist     operands_head;

    ely_prim_call(ely_context& ctx, ely_prim_kind kind);
    ~ely_prim_call();

    void push_operand(ely_expr* operand);
};