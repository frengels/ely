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

typedef struct ElyNodeKeywordLit
{
    uint32_t len;
    char     str[];
} ElyNodeKeywordLit;

typedef struct ElyNodeStringLit
{
    size_t len;
    char   str[];
} ElyNodeStringLit;

typedef struct ElyNodeIntLit
{
    uint32_t len;
    char     str[];
} ElyNodeIntLit;

typedef struct ElyNodeFloatLit
{
    uint32_t len;
    char     str[];
} ElyNodeFloatLit;

typedef struct ElyNodeCharLit
{
    uint32_t len;
    char     str[];
} ElyNodeCharLit;

typedef struct ElyNodeTrueLit
{
} ElyNodeTrueLit;

typedef struct ElyNodeFalseLit
{
} ElyNodeFalseLit;

typedef struct ElyNode
{
    ElyList        link;
    ElyNode*       parent;
    enum ElyStx    type;
    ElyStxLocation loc;
    union
    {
        ElyNodeParensList  parens_list;
        ElyNodeBracketList bracket_list;
        ElyNodeBraceList   brace_list;

        ElyNodeIdentifier id;

        ElyNodeKeywordLit kw_lit;
        ElyNodeStringLit  str_lit;
        ElyNodeIntLit     int_lit;
        ElyNodeFloatLit   float_lit;
        ElyNodeCharLit    char_lit;
        ElyNodeTrueLit    true_lit;
        ElyNodeFalseLit   false_lit;
    };
} ElyNode;

void            ely_node_create(ElyNode*       node,
                                ElyNode*       parent,
                                enum ElyStx    type,
                                ElyStxLocation loc);
ELY_EXPORT void ely_node_destroy(ElyNode* node);

ELY_EXPORT ElyString ely_node_to_string(const ElyNode* node);

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
