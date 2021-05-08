#include "ely/reader.h"

#include <stdlib.h>

void ely_stx_node_create(struct ElyStxNode* __restrict__ stx,
                          enum ElyStx kind,
                          void* __restrict__ data,
                          uint32_t len)
{
    stx->kind = kind;

    switch (stx->kind)
    {
    case ELY_STX_PARENS_LIST:
    case ELY_STX_BRACKET_LIST:
    case ELY_STX_BRACE_LIST:
    case ELY_STX_IDENTIFIER:
    case ELY_STX_KEYWORD_LIT:
    case ELY_STX_STRING_LIT:
    case ELY_STX_INT_LIT:
    case ELY_STX_FLOAT_LIT:
        stx->data = data;
        stx->len  = len;
    case ELY_STX_TRUE_LIT:
    case ELY_STX_FALSE_LIT:
        stx->data = NULL;
        stx->len  = 0;
    default:
        __builtin_unreachable();
    }
}

void ely_stx_node_destroy(struct ElyStxNode* stx)
{
    switch (stx->kind)
    {
    case ELY_STX_PARENS_LIST:
    case ELY_STX_BRACKET_LIST:
    case ELY_STX_BRACE_LIST:
    case ELY_STX_IDENTIFIER:
    case ELY_STX_KEYWORD_LIT:
    case ELY_STX_STRING_LIT:
    case ELY_STX_INT_LIT:
    case ELY_STX_FLOAT_LIT:
        // TODO: allow custom allocators
        free(stx->data);
    case ELY_STX_TRUE_LIT:
    case ELY_STX_FALSE_LIT:
    default:
        __builtin_unreachable();
    }
}