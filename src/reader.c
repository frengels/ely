#include "ely/reader.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ely/buffer.h"
#include "ely/defines.h"
#include "ely/lexer.h"

#define CHAR_LIT_OFFSET 2
#define KEYWORD_LIT_OFFSET 2

typedef struct SkipResult
{
    uint32_t tokens_skipped;
    uint32_t bytes_skipped;
} SkipResult;

/// returns the number of tokens that were skipped
static ELY_ALWAYS_INLINE SkipResult skip_atmosphere(const ElyToken* tokens,
                                                    uint32_t        len)
{
    SkipResult res;
    res.bytes_skipped  = 0;
    res.tokens_skipped = 0;

    for (; res.tokens_skipped < len; ++res.tokens_skipped)
    {
        ElyToken tok = tokens[res.tokens_skipped];

        if (!ely_token_is_atmosphere(tok.kind))
        {
            return res;
        }

        res.bytes_skipped += tok.len;
    }

    return res;
}

static inline ElyReadResult reader_read_impl(ElyReader* reader,
                                             const char* __restrict__ src,
                                             ElyNode*        parent,
                                             const ElyToken* tokens,
                                             uint32_t        len,
                                             uint32_t        idx);

static inline ElyReadResult
continue_read_parens_list(ElyReader* reader,
                          const char* __restrict__ src,
                          ElyNode*        self,
                          const ElyToken* tokens,
                          uint32_t        len,
                          uint32_t        idx)
{
    ElyNodeParensList* plist = (ElyNodeParensList*) self->data;

    for (; idx < len; ++idx)
    {
        SkipResult skipped = skip_atmosphere(&tokens[idx], len - idx);
        idx += skipped.tokens_skipped;
        reader->current_byte += skipped.bytes_skipped;

        ElyToken tok = tokens[idx];

        switch (tok.kind)
        {
        case ELY_TOKEN_WHITESPACE:
        case ELY_TOKEN_TAB:
        case ELY_TOKEN_NEWLINE_CR:
        case ELY_TOKEN_NEWLINE_LF:
        case ELY_TOKEN_NEWLINE_CRLF:
        case ELY_TOKEN_COMMENT:
            __builtin_unreachable();
            assert(false && "There shouldn't be any atmosphere here");
        case ELY_TOKEN_RPAREN: {
            ++idx;
            reader->current_byte += tok.len;
            self->loc.end_byte = reader->current_byte;
            ElyReadResult res  = {
                .node            = self,
                .tokens_consumed = idx,
            };
            return res;
        }
        case ELY_TOKEN_RBRACE:
        case ELY_TOKEN_RBRACKET:
            // TODO: handle errors
            assert(false && "expected ')', didn't get that");
            __builtin_unreachable();
            break;
        case ELY_TOKEN_EOF:
            assert(false && "missing ')'");
            break;
        default: {
            // TODO: make this iterative instead of recursive by manually
            // keeping track of nesting depth. This would minimize any chance of
            // stack exhaustion.
            ElyReadResult next_node =
                reader_read_impl(reader, src, self, tokens, len, idx);
            ely_list_insert(plist->list.prev, &next_node.node->link);
            idx = next_node.tokens_consumed - 1; // this sets the correct index
            break;
        }
        }
    }

    reader->unfinished_node = self;

    ElyReadResult res = {
        .node            = ((void*) 0x1),
        .tokens_consumed = idx,
    };

    return res;
}

static inline ElyReadResult read_parens_list(ElyReader* reader,
                                             const char* __restrict__ src,
                                             ElyNode*        parent,
                                             const ElyToken* tokens,
                                             uint32_t        len,
                                             uint32_t        idx)
{
    uint32_t start_byte = reader->current_byte -
                          1; // '(' is 1 byte therefore subtract to get start
    uint32_t alloc_size = sizeof(ElyNode) + sizeof(ElyNodeParensList);

    ElyStxLocation stx_loc = {
        .filename = reader->filename, .start_byte = start_byte
        // .end_byte = undef
    };

    ElyNode*           node  = malloc(alloc_size);
    ElyNodeParensList* plist = (ElyNodeParensList*) node->data;
    ely_node_create(node, parent, ELY_STX_PARENS_LIST, &stx_loc);
    ely_list_create(&plist->list);

    ELY_MUSTTAIL return continue_read_parens_list(
        reader, src, node, tokens, len, idx);
}

void ely_node_create(ElyNode*              node,
                     ElyNode*              parent,
                     enum ElyStx           type,
                     const ElyStxLocation* loc)
{
    node->link.next = NULL;
    node->link.prev = NULL;
    node->parent    = parent;
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
    case ELY_STX_CHAR_LIT:
        data_size =
            ely_node_char_lit_sizeof((const ElyNodeCharLit*) node->data);
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

uint32_t ely_node_char_lit_sizeof(const ElyNodeCharLit* node)
{
    return sizeof(ElyNodeCharLit) + node->len;
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

void ely_reader_create(ElyReader* reader, const char* filename)
{
    reader->filename     = filename;
    reader->current_byte = 0;
}

static inline ElyNode* read_identifier(const char* __restrict__ src,
                                       ElyNode*              parent,
                                       ElyToken              tok,
                                       const ElyStxLocation* stx_loc)
{
    uint32_t alloc_size = sizeof(ElyNode) + sizeof(ElyNodeIdentifier);
    // we shouldn't blindly allocate the length of the token since we probably
    // don't need near that much capacity.
    // Therefore we'll scan for the vertical tab character 0x7c

    bool contains_vbar = false;

    for (uint32_t i = 0; i < tok.len; ++i)
    {
        char c = src[stx_loc->start_byte + i];

        if (c == '|')
        {
            contains_vbar = true;
            break;
        }
    }

    ElyNode* node;

    if (!contains_vbar)
    {
        uint32_t char_alloc_size = tok.len;
        node                     = malloc(alloc_size + char_alloc_size);
        ElyNodeIdentifier* ident = (ElyNodeIdentifier*) node->data;
        memcpy(ident->str, &src[stx_loc->start_byte], tok.len);
        ely_node_create(node, parent, ELY_STX_IDENTIFIER, stx_loc);
    }
    else
    {
        assert(false && "cannot yet handle literal identifiers");
        __builtin_unreachable();
    }

    return node;
}

static inline ElyNode* read_string_lit(const char* __restrict__ src,
                                       ElyNode*              parent,
                                       ElyToken              tok,
                                       const ElyStxLocation* stx_loc)
{
    uint32_t alloc_size = sizeof(ElyNode) + sizeof(ElyNodeStringLit);
    uint32_t str_len    = tok.len - 2; // for leading and trailing quotes

    ElyNode*          node     = malloc(alloc_size + str_len);
    ElyNodeStringLit* str_node = (ElyNodeStringLit*) node->data;

    memcpy(str_node->str, &src[stx_loc->start_byte + 1], str_len);
    ely_node_create(node, parent, ELY_STX_STRING_LIT, stx_loc);
    return node;
}

static inline ElyNode* read_int_lit(const char* __restrict__ src,
                                    ElyNode*              parent,
                                    ElyToken              tok,
                                    const ElyStxLocation* stx_loc)
{
    uint32_t alloc_size = sizeof(ElyNode) + sizeof(ElyNodeIntLit);
    uint32_t str_len    = tok.len;

    ElyNode*       node     = malloc(alloc_size + str_len);
    ElyNodeIntLit* int_node = (ElyNodeIntLit*) node->data;

    ely_node_create(node, parent, ELY_STX_INT_LIT, stx_loc);
    memcpy(int_node->str, &src[stx_loc->start_byte], str_len);
    return node;
}

static inline ElyNode* read_float_lit(const char* __restrict__ src,
                                      ElyNode*              parent,
                                      ElyToken              tok,
                                      const ElyStxLocation* stx_loc)
{
    uint32_t alloc_size = sizeof(ElyNode) + sizeof(ElyNodeFloatLit);
    uint32_t str_len    = tok.len;

    ElyNode*         node       = malloc(alloc_size + str_len);
    ElyNodeFloatLit* float_node = (ElyNodeFloatLit*) node->data;

    ely_node_create(node, parent, ELY_STX_FLOAT_LIT, stx_loc);
    memcpy(float_node->str, &src[stx_loc->start_byte], str_len);
    return node;
}

static inline ElyNode* read_char_lit(const char* __restrict__ src,
                                     ElyNode*              parent,
                                     ElyToken              tok,
                                     const ElyStxLocation* stx_loc)
{
    uint32_t alloc_size = sizeof(ElyNode) + sizeof(ElyNodeCharLit);
    uint32_t str_len    = tok.len - CHAR_LIT_OFFSET;

    ElyNode*        node      = malloc(alloc_size + str_len);
    ElyNodeCharLit* char_node = (ElyNodeCharLit*) node->data;

    ely_node_create(node, parent, ELY_STX_CHAR_LIT, stx_loc);
    memcpy(
        char_node->str, &src[stx_loc->start_byte + CHAR_LIT_OFFSET], str_len);
    return node;
}

static inline ElyNode* read_keyword_lit(const char* __restrict__ src,
                                        ElyNode*              parent,
                                        ElyToken              tok,
                                        const ElyStxLocation* stx_loc)
{
    uint32_t alloc_size = sizeof(ElyNode) + sizeof(ElyNodeKeywordLit);
    uint32_t str_len    = tok.len - KEYWORD_LIT_OFFSET;

    ElyNode*           node    = malloc(alloc_size + str_len);
    ElyNodeKeywordLit* kw_node = (ElyNodeKeywordLit*) node->data;

    ely_node_create(node, parent, ELY_STX_KEYWORD_LIT, stx_loc);
    memcpy(
        kw_node->str, &src[stx_loc->start_byte + KEYWORD_LIT_OFFSET], str_len);
    return node;
}

static inline ElyNode* read_true_lit(ElyNode*              parent,
                                     const ElyStxLocation* stx_loc)
{
    _Static_assert(sizeof(ElyNodeTrueLit) == 0,
                   "ElyNodeTrueLit is no longer empty");

    ElyNode* node = malloc(sizeof(ElyNode));
    ely_node_create(node, parent, ELY_STX_TRUE_LIT, stx_loc);
    return node;
}

static inline ElyNode* read_false_lit(ElyNode*              parent,
                                      const ElyStxLocation* stx_loc)
{
    _Static_assert(sizeof(ElyNodeFalseLit) == 0,
                   "ElyNodeFalseLit is no longer empty");

    ElyNode* node = malloc(sizeof(ElyNode));
    ely_node_create(node, parent, ELY_STX_FALSE_LIT, stx_loc);
    return node;
}

// after all atmosphere has been skipped
static inline ElyReadResult reader_read_impl(ElyReader* reader,
                                             const char* __restrict__ src,
                                             ElyNode*        parent,
                                             const ElyToken* tokens,
                                             uint32_t        len,
                                             uint32_t        idx)
{
    ElyToken tok = tokens[idx++];

    uint32_t end_byte = reader->current_byte + tok.len;

    ElyStxLocation stx_loc = {
        .filename   = reader->filename,
        .start_byte = reader->current_byte,
        .end_byte   = end_byte,
    };

    reader->current_byte = end_byte;

    switch (tok.kind)
    {
    case ELY_TOKEN_WHITESPACE:
    case ELY_TOKEN_TAB:
    case ELY_TOKEN_NEWLINE_CR:
    case ELY_TOKEN_NEWLINE_LF:
    case ELY_TOKEN_NEWLINE_CRLF:
    case ELY_TOKEN_COMMENT:
        __builtin_unreachable();
        assert(false && "There shouldn't be any atmosphere here");

    case ELY_TOKEN_LPAREN:
        ELY_MUSTTAIL return read_parens_list(
            reader, src, parent, tokens, len, idx);
    case ELY_TOKEN_LBRACKET:
    case ELY_TOKEN_LBRACE: {
        assert(false && "not yet implemented");
        ElyReadResult res = {.node = NULL, .tokens_consumed = idx};
        return res;
    }
    case ELY_TOKEN_RPAREN:
    case ELY_TOKEN_RBRACKET:
    case ELY_TOKEN_RBRACE: {
        assert(false && "Unexpected closing");
        ElyReadResult res = {.node = NULL, .tokens_consumed = idx};
        return res;
    }
    case ELY_TOKEN_ID: {
        ElyNode*      node = read_identifier(src, parent, tok, &stx_loc);
        ElyReadResult res  = {.node = node, .tokens_consumed = idx};
        return res;
    }
    case ELY_TOKEN_INT_LIT: {
        ElyNode*      node = read_int_lit(src, parent, tok, &stx_loc);
        ElyReadResult res  = {.node = node, .tokens_consumed = idx};
        return res;
    }
    case ELY_TOKEN_FLOAT_LIT: {
        ElyNode*      node = read_float_lit(src, parent, tok, &stx_loc);
        ElyReadResult res  = {.node = node, .tokens_consumed = idx};
        return res;
    }
    case ELY_TOKEN_STRING_LIT: {
        ElyNode*      node = read_string_lit(src, parent, tok, &stx_loc);
        ElyReadResult res  = {.node = node, .tokens_consumed = idx};
        return res;
    }
    case ELY_TOKEN_CHAR_LIT: {
        ElyNode*      node = read_char_lit(src, parent, tok, &stx_loc);
        ElyReadResult res  = {.node = node, .tokens_consumed = idx};
        return res;
    }
    case ELY_TOKEN_KEYWORD_LIT: {
        ElyNode*      node = read_keyword_lit(src, parent, tok, &stx_loc);
        ElyReadResult res  = {.node = node, .tokens_consumed = idx};
        return res;
    }
    case ELY_TOKEN_TRUE_LIT: {
        ElyNode*      node = read_true_lit(parent, &stx_loc);
        ElyReadResult res  = {.node = node, .tokens_consumed = idx};
        return res;
    }
    case ELY_TOKEN_FALSE_LIT: {
        ElyNode*      node = read_false_lit(parent, &stx_loc);
        ElyReadResult res  = {.node = node, .tokens_consumed = idx};
        return res;
    }
    case ELY_TOKEN_EOF: {
        ElyReadResult res = {.node = NULL, .tokens_consumed = idx};
        return res;
    }
    default:
        __builtin_unreachable();
    }
}

ElyReadResult reader_continue_unfinished(ElyReader* reader,
                                         const char* __restrict__ src,
                                         const ElyToken* tokens,
                                         uint32_t        len)
{
    switch (reader->unfinished_node->type)
    {
    case ELY_STX_PARENS_LIST:
        return continue_read_parens_list(
            reader, src, reader->unfinished_node, tokens, len, 0);
    case ELY_STX_BRACKET_LIST:

    case ELY_STX_BRACE_LIST:

    default:
        __builtin_unreachable();
    }
}

ElyReadResult ely_reader_read(ElyReader* reader,
                              const char* __restrict__ src,
                              const ElyToken* tokens,
                              uint32_t        len)
{
    if (len == 0)
    {
        ElyReadResult res = {.node = NULL, .tokens_consumed = 0};
        return res;
    }

    if (reader->unfinished_node)
    {
        reader_continue_unfinished(reader, src, tokens, len);
    }

    SkipResult skipped   = skip_atmosphere(tokens, len);
    reader->current_byte = skipped.bytes_skipped;

    return reader_read_impl(
        reader, src, NULL, tokens, len, skipped.tokens_skipped);
}