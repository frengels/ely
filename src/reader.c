#include "ely/reader.h"

#include <stdlib.h>

void elyc_stx_node_create(struct ElycStxNode* __restrict__ stx,
                          enum ElycStx kind,
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

void elyc_stx_node_destroy(struct ElycStxNode* stx)
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