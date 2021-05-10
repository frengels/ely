#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ely/export.h"
#include "ely/list.h"
#include "ely/token.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ElyLexer  ElyLexer;
typedef struct ElyBuffer ElyBuffer;

enum ElyStx
{
    ELY_STX_PARENS_LIST,
    ELY_STX_BRACKET_LIST,
    ELY_STX_BRACE_LIST,

    ELY_STX_IDENTIFIER,

    ELY_STX_KEYWORD_LIT,
    ELY_STX_STRING_LIT,
    ELY_STX_INT_LIT,
    ELY_STX_FLOAT_LIT,
    ELY_STX_TRUE_LIT,
    ELY_STX_FALSE_LIT,
};

typedef struct ElyPosition
{
    uint32_t row;
    uint32_t col;
} ElyPosition;

typedef struct ElyStxLocation
{
    const char* filename;
    uint32_t    start_byte;
    uint32_t    end_byte;
    ElyPosition start_pos;
    ElyPosition end_pos;
} ElyStxLocation;

typedef struct ElyNode
{
    ElyList        link;
    enum ElyStx    type;
    ElyStxLocation loc;
    // this doesn't need an explicit alignment parameter as it should follow
    // loc which has void* alignment.
    char data[];
} ElyNode;

void ely_node_create(ElyNode*              node,
                     enum ElyStx           type,
                     const ElyStxLocation* loc);

uint32_t ely_node_sizeof(const ElyNode* node);

typedef struct ElyStxNode
{
    void*    data;
    uint32_t len;
    // both of these get packed into the 4 bytes behind len
    enum ElyStx kind;
    bool        is_stx;

    ElyStxLocation location;
} ElyStxNode;

typedef struct ElyNodeParensList
{
    ElyList list;
} ElyNodeParensList;

typedef struct ElyNodeBracketList
{
    ElyList list;
} ElyNodeBracketList;

typedef struct ElyNodeBraceList
{
    ElyList list;
} ElyNodeBraceList;

typedef struct ElyNodeIdentifier
{
    uint32_t len;
    // TODO: intern the identifier name
    char str[];
} ElyNodeIdentifier;

uint32_t ely_node_identifier_sizeof(const ElyNodeIdentifier* node);

typedef struct ElyNodeKeywordLit
{
    uint32_t len;
    char     str[];
} ElyNodeKeywordLit;

uint32_t ely_node_keyword_lit_sizeof(const ElyNodeKeywordLit* node);

typedef struct ElyNodeStringLit
{
    size_t len;
    char   str[];
} ElyNodeStringLit;

uint32_t ely_node_string_lit_sizeof(const ElyNodeStringLit* node);

typedef struct ElyNodeIntLit
{
    uint32_t len;
    char     str[];
} ElyNodeIntLit;

uint32_t ely_node_int_lit_sizeof(const ElyNodeIntLit* node);

typedef struct ElyNodeFloatLit
{
    uint32_t len;
    char     str[];
} ElyNodeFloatLit;

uint32_t ely_node_float_lit_sizeof(const ElyNodeFloatLit* node);

typedef struct ElyNodeTrueLit
{
} ElyNodeTrueLit;

uint32_t ely_node_true_lit_sizeof(const ElyNodeTrueLit*);

typedef struct ElyNodeFalseLit
{
} ElyNodeFalseLit;

uint32_t ely_node_false_lit_sizeof(const ElyNodeFalseLit*);

ELY_EXPORT void ely_stx_node_create(ElyStxNode* __restrict__ stx,
                                    enum ElyStx kind,
                                    void* __restrict__ data,
                                    uint32_t data_len);

ELY_EXPORT void ely_stx_node_destroy(ElyStxNode* stx);

typedef struct ElyReader
{
    const char* filename;

    char* buffer;

    ElyPosition current_pos;
    uint32_t    current_byte;
} ElyReader;

ELY_EXPORT void ely_reader_create(ElyReader* reader, const char* filename);
ELY_EXPORT void ely_reader_destroy(ElyReader* reader);

// token_buffer expects sizes as multiple of sizeof(ElyToken) bytes
ELY_EXPORT ElyNode* ely_reader_read(ElyReader* __restrict__ reader,
                                    ElyLexer* __restrict__ lexer,
                                    ElyBuffer* __restrict__ token_buffer);

#ifdef __cplusplus
}
#endif
