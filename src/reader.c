#include "ely/reader.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

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
        assert(data == NULL);
        assert(len == 0);
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
        free(stx->data);
    case ELY_STX_TRUE_LIT:
    case ELY_STX_FALSE_LIT:
    default:
        __builtin_unreachable();
    }
}

void ely_reader_create(struct ElyReader* reader, const char* filename)
{
    reader->filename        = filename;
    reader->current_pos.row = 1;
    reader->current_pos.col = 1;
    reader->current_byte    = 0;
}

struct ElyStxNode*
ely_reader_read(struct ElyReader* reader, struct ElyToken* tokens, uint32_t len)
{
    for (uint32_t i = 0; i != len; ++i)
    {
        struct ElyToken tok = tokens[i];
        switch (tok.kind)
        {
        case ELY_TOKEN_WHITESPACE:
        case ELY_TOKEN_TAB:
            break;
        case ELY_TOKEN_NEWLINE_CR:
        case ELY_TOKEN_NEWLINE_LF:
        case ELY_TOKEN_NEWLINE_CRLF:
            ++reader->current_pos.row;
            break;
        case ELY_TOKEN_COMMENT:

        case ELY_TOKEN_LPAREN:
        case ELY_TOKEN_RPAREN:
        case ELY_TOKEN_LBRACKET:
        case ELY_TOKEN_RBRACKET:
        case ELY_TOKEN_LBRACE:
        case ELY_TOKEN_RBRACE:

        case ELY_TOKEN_ID:

        case ELY_TOKEN_INT_LIT:
        case ELY_TOKEN_FLOAT_LIT:
        case ELY_TOKEN_STRING_LIT:
        case ELY_TOKEN_KEYWORD_LIT:
        case ELY_TOKEN_TRUE_LIT: {
            struct ElyStxNode* node = malloc(sizeof(struct ElyStxNode));
            ely_stx_node_create(node, ELY_STX_TRUE_LIT, NULL, 0);
            return node;
        }
        case ELY_TOKEN_FALSE_LIT: {
            struct ElyStxNode* node = malloc(sizeof(struct ElyStxNode));
            ely_stx_node_create(node, ELY_STX_FALSE_LIT, NULL, 0);
            return node;
        }
        case ELY_TOKEN_EOF:
            return NULL;
        default:
            __builtin_unreachable();
        }

        reader->current_byte += tok.len;
    }
}