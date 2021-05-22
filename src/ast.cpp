#include "ely/ast.h"

#include <cstdio>
#include <exception>
#include <string_view>

#include <assert.h>
#include <string.h>

#include "ely/assert.h"

constexpr std::string_view ret_id     = "ret";
constexpr std::string_view fn_id      = "fn";
constexpr std::string_view if_id      = "if";
constexpr std::string_view call_id    = "call";
constexpr std::string_view literal_id = "literal$";

constexpr std::string_view def_id = "def";

static inline ElyExpr parse_if(const ElyNodeParensList* plist,
                               const ElyNodeIdentifier* id)
{
    (void) plist;
    std::string_view id_strv{id->str, id->len};
    ELY_ASSERT(id_strv == if_id, "call doesn't start with `if`");

    ElyExpr res;
    return res;
}

static inline ElyExpr parse_call(const ElyNodeParensList* plist,
                                 const ElyNodeIdentifier* id)
{
    (void) plist;
    std::string_view id_strv{id->str, id->len};
    ELY_ASSERT(id_strv == call_id, "call doesn't start with `call`");

    ElyExpr res;
    return res;
}

static inline ElyExpr parse_ret(const ElyNodeParensList* plist,
                                const ElyNodeIdentifier* id)
{
    (void) plist;
    std::string_view id_strv{id->str, id->len};
    ELY_ASSERT(id_strv == ret_id, "call doesn't start with `ret`");

    ElyExpr res;
    return res;
}

static inline ElyExpr parse_literal(const ElyNodeParensList* plist,
                                    const ElyNodeIdentifier* id)
{
    (void) plist;
    std::string_view id_strv{id->str, id->len};
    ELY_ASSERT(id_strv == literal_id, "call doesn't start with `literal$`");

    ElyExpr res;
    return res;
}

static inline ElyExpr parse_fn(const ElyNodeParensList* plist,
                               const ElyNodeIdentifier* id)
{
    (void) plist;
    std::string_view id_strv{id->str, id->len};
    ELY_ASSERT(id_strv == literal_id, "call doesn't start with `fn`");

    ElyExpr res;
    return res;
}

static inline ElyExpr parse_expr_plist(const ElyNodeParensList* plist)
{
    const ElyNode* head = ely_container_of(plist->list.next, head, link);

    ELY_ASSERT(&head->link != &plist->list,
               "empty list is not a valid expression in this context");

    switch (head->type)
    {
    case ELY_STX_IDENTIFIER: {
        const ElyNodeIdentifier* id = &head->id;
        std::string_view         id_strv{id->str, id->len};
        if (id_strv == literal_id)
        {
            return parse_literal(plist, id);
        }
        else if (id_strv == call_id)
        {
            return parse_call(plist, id);
        }
        else if (id_strv == fn_id)
        {
            return parse_fn(plist, id);
        }
        else if (id_strv == if_id)
        {
            return parse_if(plist, id);
        }
        else if (id_strv == ret_id)
        {
            return parse_ret(plist, id);
        }
        else
        {
            std::fprintf(
                stderr, "found unknown form `%.*s`\n", id->len, id->str);
            ELY_UNIMPLEMENTED("must handle error properly");
        }
    }
    break;
    default:
        ELY_ASSERT_ALWAYS(false, "invalid list head detected");
    }
}

static inline ElyExpr parse_expr_id(const ElyNodeIdentifier* id)
{
    (void) id;

    ElyExpr res;
    return res;
}

ElyExpr ely_ast_parse_expr(const ElyNode* n)
{
    ElyExpr res;

    switch (n->type)
    {
    case ELY_STX_PARENS_LIST: {
        const ElyNodeParensList* plist = &n->parens_list;
        return parse_expr_plist(plist);
    }
    case ELY_STX_BRACKET_LIST:
        break;
    case ELY_STX_BRACE_LIST:
        break;
    case ELY_STX_IDENTIFIER:
        parse_expr_id(&n->id);
        break;
    case ELY_STX_KEYWORD_LIT:
        break;
    case ELY_STX_STRING_LIT:
    case ELY_STX_INT_LIT:
    case ELY_STX_FLOAT_LIT:
        ELY_UNIMPLEMENTED("literal has to appear in literal expression, make "
                          "this a proper error and pretend it's correct");

        break;
    case ELY_STX_TRUE_LIT: {
        ElyBoolLit blit = {.node = n, .val = true};
        ElyLitExpr lit  = {.kind = ELY_LIT_BOOL, .bool_lit = blit};
        res.kind        = ELY_EXPR_LIT;
        res.lit_expr    = lit;
    }
    break;
    case ELY_STX_FALSE_LIT: {
        ElyBoolLit blit = {.node = n, .val = false};
        ElyLitExpr lit  = {.kind = ELY_LIT_BOOL, .bool_lit = blit};
        res.kind        = ELY_EXPR_LIT;
        res.lit_expr    = lit;
    }
    break;
    default:
        __builtin_unreachable();
    }

    return res;
}

ElyDef parse_toplevel_def(const ElyNodeParensList& plist,
                          const ElyNodeIdentifier& id)
{
    std::string_view id_strv{id.str, id.len};
    ELY_ASSERT(id_strv == def_id, "call doesn't start with `def`");

    ElyDef res;

    return res;
}

ElyDef parse_toplevel_plist(const ElyNodeParensList& plist)
{
    ElyDef res;

    const ElyNode* head = ely_container_of(plist.list.next, head, link);

    switch (head->type)
    {
    case ELY_STX_IDENTIFIER: {
        const ElyNodeIdentifier& id = head->id;
        std::string_view         id_strv{id.str, id.len};

        if (id_strv == def_id)
        {
            return parse_toplevel_def(plist, id);
        }
    }
    break;
    default: {
        ElyString str = ely_node_to_string(head);
        std::fprintf(stderr,
                     "unexpected toplevel form\n%s\n",
                     static_cast<int>(str.len),
                     str.data);
        ELY_UNIMPLEMENTED("elegantly handle invalid toplevel forms");
    }
    break;
    }

    return res;
}

ElyDef ely_ast_parse_toplevel(const ElyNode* n)
{
    ElyDef res;

    switch (n->type)
    {
    case ELY_STX_PARENS_LIST: {
        auto& plist = n->parens_list;
        parse_toplevel_plist(plist);
    }
    break;
    default: {
        ElyString node_str = ely_node_to_string(n);
        std::fprintf(stderr,
                     "Didn't expect toplevel node:\n%.*s\n",
                     static_cast<int>(node_str.len),
                     node_str.data);
        ely_string_destroy(&node_str);
        ELY_UNIMPLEMENTED("graciously handle with error output");
    }
    break;
    }

    return res;
}