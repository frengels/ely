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

static inline ElyExpr parse_if(ElyNodeParensList* plist, ElyNodeIdentifier* id)
{
    (void) plist;
    std::string_view id_strv{id->str, id->len};
    ELY_ASSERT(id_strv == if_id, "call doesn't start with `if`");

    ElyExpr res;
    return res;
}

static inline ElyExpr parse_call(ElyNodeParensList* plist,
                                 ElyNodeIdentifier* id)
{
    (void) plist;
    std::string_view id_strv{id->str, id->len};
    ELY_ASSERT(id_strv == call_id, "call doesn't start with `call`");

    ElyExpr res;
    return res;
}

static inline ElyExpr parse_ret(ElyNodeParensList* plist, ElyNodeIdentifier* id)
{
    (void) plist;
    std::string_view id_strv{id->str, id->len};
    ELY_ASSERT(id_strv == ret_id, "call doesn't start with `ret`");

    ElyExpr res;
    return res;
}

static inline ElyExpr parse_literal(ElyNodeParensList* plist,
                                    ElyNodeIdentifier* id)
{
    (void) plist;
    std::string_view id_strv{id->str, id->len};
    ELY_ASSERT(id_strv == literal_id, "call doesn't start with `literal$`");

    ElyExpr res;
    return res;
}

static inline ElyExpr parse_fn(ElyNodeParensList* plist, ElyNodeIdentifier* id)
{
    (void) plist;
    std::string_view id_strv{id->str, id->len};
    ELY_ASSERT(id_strv == literal_id, "call doesn't start with `fn`");

    ElyExpr res;
    return res;
}

static inline ElyExpr parse_expr_plist(ElyNodeParensList* plist)
{
    ElyNode* head = ely_container_of(plist->list.next, head, link);

    ELY_ASSERT(&head->link != &plist->list,
               "empty list is not a valid expression in this context");

    switch (head->type)
    {
    case ELY_STX_IDENTIFIER: {
        ElyNodeIdentifier* id = &head->id;
        std::string_view   id_strv{id->str, id->len};
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

static inline ElyExpr parse_expr_id(ElyNodeIdentifier* id)
{
    (void) id;

    ElyExpr res;
    return res;
}

ElyExpr ely_ast_parse_expr(ElyNode* n)
{
    ElyExpr res;

    switch (n->type)
    {
    case ELY_STX_PARENS_LIST: {
        ElyNodeParensList* plist = &n->parens_list;
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

ElyDef ely_ast_parse_toplevel(ElyNode* n)
{
    ElyDef res;

    return res;
}