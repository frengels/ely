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
typedef struct ElyNode   ElyNode;

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
    ELY_STX_CHAR_LIT,
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
} ElyStxLocation;

typedef struct ElyNode
{
    ElyList        link;
    ElyNode*       parent;
    enum ElyStx    type;
    ElyStxLocation loc;
    // this doesn't need an explicit alignment parameter as it should follow
    // loc which has void* alignment.
    char data[];
} ElyNode;

void            ely_node_create(ElyNode*       node,
                                ElyNode*       parent,
                                enum ElyStx    type,
                                ElyStxLocation loc);
ELY_EXPORT void ely_node_destroy(ElyNode* node);

ELY_EXPORT ElyString ely_node_to_string(const ElyNode* node);

uint32_t ely_node_sizeof(const ElyNode* node);

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

typedef struct ElyNodeCharLit
{
    uint32_t len;
    char     str[];
} ElyNodeCharLit;

uint32_t ely_node_char_lit_sizeof(const ElyNodeCharLit* node);

typedef struct ElyNodeTrueLit
{
} ElyNodeTrueLit;

uint32_t ely_node_true_lit_sizeof(const ElyNodeTrueLit*);

typedef struct ElyNodeFalseLit
{
} ElyNodeFalseLit;

uint32_t ely_node_false_lit_sizeof(const ElyNodeFalseLit*);

typedef struct ElyReader
{
    const char* filename;
    uint32_t    current_byte;
    ElyNode*    unfinished_node;
} ElyReader;

ELY_EXPORT void ely_reader_create(ElyReader* reader, const char* filename);

typedef struct ElyReadResult
{
    ElyNode* node;
    uint32_t tokens_consumed;
} ElyReadResult;

// token_buffer expects sizes as multiple of sizeof(ElyToken) bytes
ELY_EXPORT ElyReadResult ely_reader_read(ElyReader* reader,
                                         const char* __restrict__ src,
                                         const ElyToken* tokens,
                                         uint32_t        len);

ELY_EXPORT void ely_reader_read_all(ElyReader* reader,
                                    const char* __restrict__ src,
                                    const ElyToken* tokens,
                                    uint32_t        len,
                                    ElyList*        list);

#ifdef __cplusplus
}
#endif
