#include "ely/reader.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ely/buffer.h"
#include "ely/defines.h"
#include "ely/lexer.h"

static inline ElyNode* read_paren_list(ElyReader* __restrict__ reader,
                                       ElyLexer* __restrict__ lexer,
                                       ElyBuffer* __restrict__ token_buffer)
{

    while (token_buffer->begin < token_buffer->end)
    {
        ElyToken tok = ((ElyToken*) token_buffer->data)[token_buffer->begin];

        switch (tok.kind)
        {
        case ELY_TOKEN_LPAREN:
            break;
        case ELY_TOKEN_RPAREN:
            // end current parens_list finally

            break;
        case ELY_TOKEN_RBRACE:
        case ELY_TOKEN_RBRACKET:
            assert(false && "expected ')' to match '('");
        }

        ++token_buffer->begin;
    }
}

/*
static inline ElyStxNode*
read_bracket_list(ElyReader* reader, ElyToken* tokens, uint32_t len)
{
    (void) reader;
    (void) tokens;
    (void) len;
}

static inline ElyStxNode*
read_brace_list(ElyReader* reader, ElyToken* tokens, uint32_t len)
{
    (void) reader;
    (void) tokens;
    (void) len;
}
*/

void ely_node_create(ElyNode* node, enum ElyStx type, const ElyStxLocation* loc)
{
    node->link.next = NULL;
    node->link.prev = NULL;
    node->type      = type;
    memcpy(&node->loc, loc, sizeof(ElyStxLocation));
}

uint32_t ely_node_sizeof(const ElyNode* node)
{
    uint32_t data_size;
    switch (node->type)
    {
    case ELY_STX_PARENS_LIST:
        data_size = sizeof(ElyNodeParensList);
    case ELY_STX_BRACKET_LIST:
        data_size = sizeof(ElyNodeBracketList);
    case ELY_STX_BRACE_LIST:
        data_size = sizeof(ElyNodeBraceList);
    case ELY_STX_IDENTIFIER:
        data_size =
            ely_node_identifier_sizeof((const ElyNodeIdentifier*) node->data);
    case ELY_STX_KEYWORD_LIT:
        data_size =
            ely_node_keyword_lit_sizeof((const ElyNodeKeywordLit*) node->data);
    case ELY_STX_STRING_LIT:
        data_size =
            ely_node_string_lit_sizeof((const ElyNodeStringLit*) node->data);
    case ELY_STX_INT_LIT:
        data_size = ely_node_int_lit_sizeof((const ElyNodeIntLit*) node->data);
    case ELY_STX_FLOAT_LIT:
        data_size =
            ely_node_float_lit_sizeof((const ElyNodeFloatLit*) node->data);
    case ELY_STX_TRUE_LIT:
        data_size =
            ely_node_true_lit_sizeof((const ElyNodeTrueLit*) node->data);
    case ELY_STX_FALSE_LIT:
        data_size =
            ely_node_false_lit_sizeof((const ElyNodeFalseLit*) node->data);
    default:
        __builtin_unreachable();
    }

    return data_size + sizeof(ElyNode);
}

uint32_t ely_node_identifier_sizeof(const ElyNodeIdentifier* node)
{
    return sizeof(ElyNodeIdentifier) + node->len;
}

uint32_t ely_node_keyword_lit_sizeof(const ElyNodeKeywordLit* node)
{
    return sizeof(ElyNodeKeywordLit) + node->len;
}

uint32_t ely_node_string_lit_sizeof(const ElyNodeStringLit* node)
{
    return sizeof(ElyNodeStringLit) + node->len;
}

uint32_t ely_node_int_lit_sizeof(const ElyNodeIntLit* node)
{
    return sizeof(ElyNodeIntLit) + node->len;
}

uint32_t ely_node_float_lit_sizeof(const ElyNodeFloatLit* node)
{
    return sizeof(ElyNodeFloatLit) + node->len;
}

uint32_t ely_node_true_lit_sizeof(const ElyNodeTrueLit* node)
{
    (void) node;
    return sizeof(ElyNodeTrueLit);
}

uint32_t ely_node_false_lit_sizeof(const ElyNodeFalseLit* node)
{
    (void) node;
    return sizeof(ElyNodeFalseLit);
}

void ely_stx_node_create(ElyStxNode* __restrict__ stx,
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

void ely_stx_node_destroy(ElyStxNode* stx)
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
        break;
    case ELY_STX_TRUE_LIT:
    case ELY_STX_FALSE_LIT:
        break;
    default:
        __builtin_unreachable();
    }
}

void ely_reader_create(ElyReader* reader, const char* filename)
{
    reader->filename        = filename;
    reader->current_pos.row = 1;
    reader->current_pos.col = 1;
    reader->current_byte    = 0;
}

ElyNode* ely_reader_read(ElyReader* __restrict__ reader,
                         ElyLexer* __restrict__ lexer,
                         ElyBuffer* __restrict__ token_buffer)
{
    while (token_buffer->begin < token_buffer->end)
    {
        ElyToken tok = ((ElyToken*) token_buffer->data)[token_buffer->begin];

        ElyPosition next_pos = {.row = reader->current_pos.row,
                                .col = reader->current_pos.col += tok.len};

        switch (tok.kind)
        {
        case ELY_TOKEN_WHITESPACE:
        case ELY_TOKEN_TAB:
            break;
        case ELY_TOKEN_NEWLINE_CR:
        case ELY_TOKEN_NEWLINE_LF:
        case ELY_TOKEN_NEWLINE_CRLF:
            ++reader->current_pos.row;
            reader->current_pos.col = 1;
            goto skip_assign_next_pos;
        case ELY_TOKEN_COMMENT:
            break;
        case ELY_TOKEN_LPAREN:
            ++token_buffer->begin;
            ELY_MUSTTAIL return read_paren_list(reader, lexer, token_buffer);
        case ELY_TOKEN_LBRACKET:
            // ELY_MUSTTAIL return read_bracket_list(reader, lexer, tokens, pos,
            // len);
        case ELY_TOKEN_LBRACE:
            // ELY_MUSTTAIL return read_brace_list(reader, lexer, tokens, pos,
            // len);
        case ELY_TOKEN_RPAREN:
        case ELY_TOKEN_RBRACKET:
            assert(false && "Unexpected closing");
        case ELY_TOKEN_RBRACE:

        case ELY_TOKEN_ID: {
            uint32_t fixed_size = sizeof(ElyNode) + sizeof(ElyNodeIdentifier);
            uint32_t str_size   = tok.len;
            ElyNode* node       = malloc(fixed_size + str_size);
            ElyStxLocation loc  = {.filename   = reader->filename,
                                  .start_byte = reader->current_byte,
                                  .end_byte   = reader->current_byte + tok.len,
                                  .start_pos  = reader->current_pos,
                                  .end_pos    = next_pos};
            ely_node_create(node, ELY_STX_IDENTIFIER, &loc);
            ElyNodeIdentifier* id_node = node->data;
            memcpy(id_node->str,
                   &lexer->src[loc.start_byte],
                   sizeof(char) * (loc.end_byte - loc.start_byte));
        }
        case ELY_TOKEN_INT_LIT: {
            uint32_t fixed_size = sizeof(ElyNode) + sizeof(ElyNodeIntLit);
            uint32_t str_size   = tok.len;
            ElyNode* node       = malloc(fixed_size + str_size);
        }
        case ELY_TOKEN_FLOAT_LIT: {
            ElyStxNode* node = malloc(sizeof(ElyStxNode));
            ely_stx_node_create(node, ELY_STX_FLOAT_LIT, NULL, 0);
            return node;
        }
        case ELY_TOKEN_STRING_LIT: {
            ElyStxNode* node = malloc(sizeof(ElyStxNode));
            ely_stx_node_create(node, ELY_STX_STRING_LIT, NULL, 0);
            return node;
        }
        case ELY_TOKEN_KEYWORD_LIT: {
            ElyStxNode* node = malloc(sizeof(ElyStxNode));
            ely_stx_node_create(node, ELY_STX_KEYWORD_LIT, NULL, 0);
            return node;
        }
        case ELY_TOKEN_TRUE_LIT: {
            ElyStxNode* node = malloc(sizeof(ElyStxNode));
            ely_stx_node_create(node, ELY_STX_TRUE_LIT, NULL, 0);
            return node;
        }
        case ELY_TOKEN_FALSE_LIT: {
            ElyStxNode* node = malloc(sizeof(ElyStxNode));
            ely_stx_node_create(node, ELY_STX_FALSE_LIT, NULL, 0);
            return node;
        }
        case ELY_TOKEN_EOF:
            return NULL;
        default:
            __builtin_unreachable();
        }

        reader->current_pos = next_pos;
    skip_assign_next_pos:
        ++token_buffer->begin;
    }
}
