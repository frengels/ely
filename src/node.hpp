#pragma once

#include <cstddef>
#include <cstdint>

#include "ely/ilist.h"
#include "ely/node.h"
#include "ely/string.h"

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

    ely_string name;

    ely_stx_id(ely_context& ctx, const char* name, std::size_t len);
    ~ely_stx_id();
};

struct ely_def : ely_node
{
    char*       name;
    std::size_t len;
    ely_expr*   init;

    ely_def(ely_context& ctx,
            const char*  name,
            std::size_t  len,
            ely_expr*    init);
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

    ely_literal(ely_context& ctx, ely_node_kind kind);
};

struct ely_string_literal : ely_literal
{
    using base = ely_literal;

    ely_string text;

    ely_string_literal(ely_context& ctx, const char* text, std::size_t len);
    ~ely_string_literal();
};

struct ely_int_literal : ely_literal
{
    using base = ely_literal;

    ely_string str;

    ely_int_literal(ely_context& ctx, const char* str, std::size_t len);
    ~ely_int_literal();
};

struct ely_dec_literal : ely_literal
{
    using base = ely_literal;

    ely_string str;

    ely_dec_literal(ely_context& ctx, const char* str, std::size_t len);
    ~ely_dec_literal();
};

struct ely_s32_literal : ely_literal
{
    using base = ely_literal;

    std::int32_t val;

    ely_s32_literal(ely_context& ctx, std::int32_t val);
};

struct ely_s64_literal : ely_literal
{
    using base = ely_literal;

    std::int64_t val;

    ely_s64_literal(ely_context& ctx, std::int64_t val);
};

struct ely_u32_literal : ely_literal
{
    using base = ely_literal;

    std::uint32_t val;

    ely_u32_literal(ely_context& ctx, std::uint32_t val);
};

struct ely_u64_literal : ely_literal
{
    using base = ely_literal;

    std::uint64_t val;

    ely_u64_literal(ely_context& ctx, std::uint64_t val);
};

struct ely_f32_literal : ely_literal
{
    using base = ely_literal;

    float val;

    ely_f32_literal(ely_context& ctx, float val);
};

struct ely_f64_literal : ely_literal
{
    using base = ely_literal;

    double val;

    ely_f64_literal(ely_context& ctx, double val);
};

struct ely_fn : ely_expr
{
    using base = ely_expr;

    ely_ilist args_head; // ely_var
    ely_expr* e;

    ely_fn(ely_context& ctx, ely_expr* e);
    ~ely_fn();

    void push_arg(ely_var* v);
};

struct ely_var : ely_expr
{
    using base = ely_expr;

    ely_string name_;

    ely_var(ely_context& ctx, const char* name, std::size_t len);
    ~ely_var();

    ely_string_view name() const;
    ely_type        type() const;
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