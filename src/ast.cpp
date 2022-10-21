#include <cassert>

#include <algorithm>

#include "ely/ast.h"
#include "ely/ilist.h"

struct ely_node
{
    ely_node_base base;
    uint32_t      ref_count;
    ely_ilist     link{};
    ely_context*  ctx;
    ely_node*     expanded_from;

    ely_node(ely_context& ctx, ely_node_kind kind)
        : ely_node(ctx, kind, nullptr)
    {}

    ely_node(ely_context& ctx, ely_node_kind kind, ely_node* expanded)
        : base{kind}, ref_count(1), ctx(std::addressof(ctx)),
          expanded_from(expanded)
    {
        if (expanded_from)
            ely_node_ref(static_cast<void*>(expanded_from));
    }

    ~ely_node()
    {
        assert(ref_count == 0 &&
               "may not destroy node whilst references are alive");
        if (expanded_from)
            ely_node_deref(static_cast<void*>(expanded_from));
    }

    uint32_t ref()
    {
        return ++ref_count;
    }

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

    ely_stx_list(ely_context& ctx) : base(ctx, ELY_NODE_STX_LIST)
    {
        ely_ilist_init(&head);
    }

    ~ely_stx_list()
    {
        ely_node* n;
        ELY_ILIST_FOR_EACH(n, &head, link)
        {
            n->deref();
        }
    }

    void push_back(ely_node* node)
    {
        node->ref();
        ely_ilist_append(&head, &node->link);
    }
};

struct ely_stx_id : ely_stx
{
    using base = ely_stx;

    char*  name;
    size_t len;

    ely_stx_id(ely_context& ctx, const char* name, size_t len)
        : base(ctx, ELY_NODE_STX_ID), name(new char[len + 1]), len(len)
    {
        std::copy(name, name + len, this->name);
        this->name[len] = '\0';
    }

    ~ely_stx_id()
    {
        delete[] name;
    }
};

struct ely_def : ely_node
{
    char*     name;
    size_t    len;
    ely_expr* init;

    ely_def(ely_context& ctx, const char* name, size_t len, ely_expr* init)
        : ely_node(ctx, ELY_NODE_DEF), name(new char[len]), len(len), init(init)
    {
        std::copy(name, name + len, this->name);
        ely_node_ref(static_cast<void*>(init));
    }

    ~ely_def()
    {
        ely_node_deref(static_cast<void*>(init));
        delete[] name;
    }
};

struct ely_list : ely_expr
{
    using base = ely_expr;

    ely_ilist head;

    ely_list(ely_context& ctx) : base(ctx, ELY_NODE_LIST)
    {
        ely_ilist_init(&head);
    }

    ~ely_list()
    {
        ely_node* n;
        ELY_ILIST_FOR_EACH(n, &head, link)
        {
            n->deref();
        }
    }

    void push_back(ely_node* node)
    {
        node->ref();
        ely_ilist_append(&head, &node->link);
    }
};

struct ely_literal : ely_expr
{
    using base = ely_expr;

    char*  str;
    size_t len;

    ely_literal(ely_context&  ctx,
                ely_node_kind kind,
                const char*   str,
                size_t        len)
        : base(ctx, kind), str(new char[len + 1]), len(len)
    {
        std::copy(str, str + len, this->str);
        this->str[this->len] = '\0';
    }

    ~ely_literal()
    {
        delete[] str;
    }
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

    ely_var(ely_context& ctx, const char* name, size_t len)
        : base(ctx, ELY_NODE_VAR), name(new char[len + 1]), len(len)
    {
        std::copy(name, name + len, this->name);
        this->name[len] = '\0';
    }

    ~ely_var()
    {
        delete[] name;
    }
};

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
        case ELY_NODE_VAR:
            delete static_cast<ely_var*>(this);
            break;
        }
    }

    return new_ref_count;
}

uint32_t ely_node_ref(void* n)
{
    ely_node* node = static_cast<ely_node*>(n);
    return node->ref();
}

uint32_t ely_node_deref(void* n)
{
    ely_node* node = static_cast<ely_node*>(n);
    return node->deref();
}

ely_list* ely_list_create(ely_context* ctx)
{
    return new ely_list(*ctx);
}

ely_def*
ely_def_create(ely_context* ctx, char* name, size_t len, ely_expr* init)
{
    return new ely_def(*ctx, name, len, init);
}

ely_string_literal*
ely_string_literal_create(ely_context* ctx, const char* text, size_t len)
{
    return new ely_string_literal(*ctx, ELY_NODE_LIT_STRING, text, len);
}

ely_int_literal*
ely_int_literal_create(ely_context* ctx, const char* str, size_t len)
{
    return new ely_int_literal(*ctx, ELY_NODE_LIT_INT, str, len);
}

ely_dec_literal*
ely_dec_literal_create(ely_context* ctx, const char* str, size_t len)
{
    return new ely_dec_literal(*ctx, ELY_NODE_LIT_DEC, str, len);
}

ely_var* ely_var_create(ely_context* ctx, const char* name, size_t len)
{
    return new ely_var(*ctx, name, len);
}
