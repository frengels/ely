#include <cassert>

#include <algorithm>

#include "ely/type.hpp"

#include "node.hpp"

ely_node::ely_node(ely_context& ctx, ely_node_kind kind)
    : ely_node(ctx, kind, nullptr)
{}

ely_node::ely_node(ely_context& ctx, ely_node_kind kind, ely_node* expanded)
    : base{kind}, ref_count(1), ctx(std::addressof(ctx)),
      expanded_from(expanded)
{
    if (expanded_from)
        ely_node_ref(static_cast<void*>(expanded_from));
}

ely_node::~ely_node()
{
    assert(ref_count == 0 &&
           "may not destroy node whilst references are alive");
    if (expanded_from)
        ely_node_unref(static_cast<void*>(expanded_from));
}

uint32_t ely_node::ref()
{
    return ++ref_count;
}

uint32_t ely_node::deref()
{
    uint32_t new_ref_count = --ref_count;

    if (new_ref_count == 0)
    {
        switch (this->base.kind)
        {
        case ELY_NODE_DEF:
            delete static_cast<ely_def*>(this);
            break;
        case ELY_NODE_STX_LIST:
            delete static_cast<ely_stx_list*>(this);
            break;
        case ELY_NODE_STX_ID:
            delete static_cast<ely_stx_id*>(this);
            break;
        case ELY_NODE_LIST:
            delete static_cast<ely_list*>(this);
            break;
        case ELY_NODE_LIT_STRING:
            delete static_cast<ely_string_literal*>(this);
            break;
        case ELY_NODE_LIT_INT:
            delete static_cast<ely_int_literal*>(this);
            break;
        case ELY_NODE_LIT_DEC:
            delete static_cast<ely_dec_literal*>(this);
            break;
        case ELY_NODE_LIT_S32:
            delete static_cast<ely_s32_literal*>(this);
            break;
        case ELY_NODE_LIT_S64:
            delete static_cast<ely_s64_literal*>(this);
            break;
        case ELY_NODE_LIT_U32:
            delete static_cast<ely_u32_literal*>(this);
            break;
        case ELY_NODE_LIT_U64:
            delete static_cast<ely_u64_literal*>(this);
            break;
        case ELY_NODE_LIT_F32:
            delete static_cast<ely_f32_literal*>(this);
            break;
        case ELY_NODE_LIT_F64:
            delete static_cast<ely_f64_literal*>(this);
            break;
        case ELY_NODE_VAR:
            delete static_cast<ely_var*>(this);
            break;
        case ELY_NODE_CALL:
            delete static_cast<ely_call*>(this);
            break;
        default:
            assert(0 && "Unhandled node type in deref");
            break;
        }
    }

    return new_ref_count;
}

ely_stx_list::ely_stx_list(ely_context& ctx) : base(ctx, ELY_NODE_STX_LIST)
{
    ely_ilist_init(&head);
}

ely_stx_list::~ely_stx_list()
{
    ely_node* n;
    ELY_ILIST_FOR_EACH(n, &head, link)
    {
        n->deref();
    }
}

void ely_stx_list::push_back(ely_stx* stx)
{
    stx->ref();
    ely_ilist_append(&head, &stx->link);
}

ely_stx_id::ely_stx_id(ely_context& ctx, const char* name, std::size_t len)
    : base(ctx, ELY_NODE_STX_ID), name(ely_string_create_len(name, len))
{}

ely_stx_id::~ely_stx_id()
{
    ely_string_destroy(this->name);
}

ely_def::ely_def(ely_context& ctx, const char* name, size_t len, ely_expr* init)
    : ely_node(ctx, ELY_NODE_DEF), name(new char[len]), len(len), init(init)
{
    std::copy(name, name + len, this->name);
    ely_node_ref(static_cast<void*>(init));
}

ely_def::~ely_def()
{
    ely_node_unref(static_cast<void*>(init));
    delete[] name;
}

ely_list::ely_list(ely_context& ctx) : base(ctx, ELY_NODE_LIST)
{
    ely_ilist_init(&head);
}

ely_list::~ely_list()
{
    ely_node* n;
    ELY_ILIST_FOR_EACH(n, &head, link)
    {
        n->deref();
    }
}

void ely_list::push_back(ely_node* node)
{
    node->ref();
    ely_ilist_append(&head, &node->link);
}

ely_literal::ely_literal(ely_context& ctx, ely_node_kind kind) : base(ctx, kind)
{}

ely_string_literal::ely_string_literal(ely_context& ctx,
                                       const char*  text,
                                       std::size_t  len)
    : base(ctx, ELY_NODE_LIT_STRING), text(ely_string_create_len(text, len))
{}

ely_string_literal::~ely_string_literal()
{
    ely_string_destroy(this->text);
}

ely_int_literal::ely_int_literal(ely_context& ctx,
                                 const char*  str,
                                 std::size_t  len)
    : base(ctx, ELY_NODE_LIT_INT), str(ely_string_create_len(str, len))
{}

ely_int_literal::~ely_int_literal()
{
    ely_string_destroy(str);
}

ely_dec_literal::ely_dec_literal(ely_context& ctx,
                                 const char*  str,
                                 std::size_t  len)
    : base(ctx, ELY_NODE_LIT_DEC), str(ely_string_create_len(str, len))
{}

ely_dec_literal::~ely_dec_literal()
{
    ely_string_destroy(str);
}

ely_s32_literal::ely_s32_literal(ely_context& ctx, std::int32_t val)
    : base(ctx, ELY_NODE_LIT_S32), val(val)
{}

ely_s64_literal::ely_s64_literal(ely_context& ctx, std::int64_t val)
    : base(ctx, ELY_NODE_LIT_S64), val(val)
{}

ely_u32_literal::ely_u32_literal(ely_context& ctx, std::uint32_t val)
    : base(ctx, ELY_NODE_LIT_U32), val(val)
{}

ely_u64_literal::ely_u64_literal(ely_context& ctx, std::uint64_t val)
    : base(ctx, ELY_NODE_LIT_U64), val(val)
{}

ely_f32_literal::ely_f32_literal(ely_context& ctx, float val)
    : base(ctx, ELY_NODE_LIT_F32), val(val)
{}

ely_f64_literal::ely_f64_literal(ely_context& ctx, double val)
    : base(ctx, ELY_NODE_LIT_F64), val(val)
{}

ely_let::ely_let(ely_context& ctx, ely_expr* e) : base(ctx, ELY_NODE_LET), e(e)
{
    ely_ilist_init(&vars_head);
    ely_ilist_init(&inits_head);
}

ely_let::~ely_let()
{
    ely_node_unref(e);

    ely_node* n; // to avoid offsetof issues
    ELY_ILIST_FOR_EACH(n, &vars_head, link)
    {
        ely_node_unref(n);
    }

    ely_expr* e;
    ELY_ILIST_FOR_EACH(e, &inits_head, link)
    {
        ely_node_unref(e);
    }
}

void ely_let::push(ely_var* v, ely_expr* init)
{
    ely_node_ref(v);
    ely_node_ref(init);

    ely_ilist_append(&vars_head, &static_cast<ely_node*>(v)->link);
    ely_ilist_append(&inits_head, &init->link);
}

ely_fn::ely_fn(ely_context& ctx, ely_expr* e) : base(ctx, ELY_NODE_FN), e(e)
{
    ely_ilist_init(&args_head);
    ely_node_ref(e);
}

ely_fn::~ely_fn()
{
    ely_node_unref(e);

    ely_node* n; // as ely_node to avoid offsetof issues
    ELY_ILIST_FOR_EACH(n, &args_head, link)
    {
        ely_node_unref(n);
    }
}

void ely_fn::push_arg(ely_var* v)
{
    ely_node_ref(v);
    ely_ilist_append(&args_head, &v->link);
}

ely_var::ely_var(ely_context& ctx, const char* name, size_t len)
    : base(ctx, ELY_NODE_VAR), name_(ely_string_create_len(name, len))
{}

ely_var::~ely_var()
{
    ely_string_destroy(name_);
}

ely_string_view ely_var::name() const
{
    return ely_string_view_from_string(name_);
}

ely_type ely_var::type() const
{
    return ely::get_type_generic();
}

ely_call::ely_call(ely_context& ctx) : base(ctx, ELY_NODE_CALL)
{}

ely_call::~ely_call()
{
    ely_node_unref(op);
    ely_expr* e;
    ELY_ILIST_FOR_EACH(e, &operands_head, link)
    {
        e->deref();
    }
}

ely_prim_call::ely_prim_call(ely_context& ctx, ely_prim_kind kind)
    : base(ctx, ELY_NODE_PRIM_CALL), kind(kind)
{
    ely_ilist_init(&operands_head);
}

ely_prim_call::~ely_prim_call()
{
    ely_expr* e;
    ELY_ILIST_FOR_EACH(e, &operands_head, link)
    {
        e->deref();
    }
}

void ely_prim_call::push_operand(ely_expr* e)
{
    e->ref();
    ely_ilist_append(&operands_head, &e->link);
}

void ely_call::push_operand(ely_expr* e)
{
    e->ref();
    ely_ilist_append(&operands_head, &e->link);
}

uint32_t ely_node_ref(void* n)
{
    ely_node* node = static_cast<ely_node*>(n);
    return node->ref();
}

uint32_t ely_node_unref(void* n)
{
    ely_node* node = static_cast<ely_node*>(n);
    return node->deref();
}

ely_list* ely_list_create(ely_context* ctx)
{
    return new ely_list(*ctx);
}

void ely_list_append(ely_list* list, ely_node* node)
{
    list->push_back(node);
}

ely_stx_list* ely_stx_list_create(ely_context* ctx)
{
    return new ely_stx_list(*ctx);
}

void ely_stx_list_append(ely_stx_list* list, ely_stx* stx)
{
    list->push_back(stx);
}

ely_def*
ely_def_create(ely_context* ctx, char* name, size_t len, ely_expr* init)
{
    return new ely_def(*ctx, name, len, init);
}

ely_string_literal*
ely_string_literal_create(ely_context* ctx, const char* text, size_t len)
{
    return new ely_string_literal(*ctx, text, len);
}

ely_int_literal*
ely_int_literal_create(ely_context* ctx, const char* str, size_t len)
{
    return new ely_int_literal(*ctx, str, len);
}

ely_dec_literal*
ely_dec_literal_create(ely_context* ctx, const char* str, size_t len)
{
    return new ely_dec_literal(*ctx, str, len);
}

ely_s32_literal* ely_s32_literal_create(ely_context* ctx, std::int32_t val)
{
    return new ely_s32_literal(*ctx, val);
}

ely_s64_literal* ely_s64_literal_create(ely_context* ctx, std::int64_t val)
{
    return new ely_s64_literal(*ctx, val);
}

ely_let* ely_let_create(ely_context* ctx, ely_expr* e)
{
    return new ely_let(*ctx, e);
}

void ely_let_push(ely_let* l, ely_var* v, ely_expr* init)
{
    l->push(v, init);
}

ely_fn* ely_fn_create(ely_context* ctx, ely_expr* e)
{
    return new ely_fn(*ctx, e);
}

ely_var* ely_var_create(ely_context* ctx, const char* name, size_t len)
{
    return new ely_var(*ctx, name, len);
}

ely_string_view ely_var_name(const ely_var* v)
{
    return v->name();
}

ely_type ely_var_type(const ely_var* v)
{
    return v->type();
}

struct ely_prim_call* ely_prim_call_create(struct ely_context* ctx,
                                           enum ely_prim_kind  kind)
{
    return new ely_prim_call(*ctx, kind);
}

void ely_prim_call_push_operand(ely_prim_call* pcall, ely_expr* e)
{
    pcall->push_operand(e);
}