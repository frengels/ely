#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "ely/lexer.h"
#include "ely/export.h"

#ifdef __cplusplus
extern "C" {
#endif

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

struct ElyPosition
{
    uint32_t row;
    uint32_t col;
};

struct ElyStxLocation
{
    const char*         filename;
    uint32_t            start_byte;
    uint32_t            end_byte;
    struct ElyPosition start_pos;
    struct ElyPosition end_pos;
};

struct ElyStxNode
{

    void*    data;
    uint32_t len;
    // both of these get packed into the 4 bytes behind len
    enum ElyStx kind;
    bool         is_stx;

    struct ElyStxLocation location;
};

ELY_EXPORT void ely_stx_node_create(struct ElyStxNode* __restrict__ stx,
                          enum ElyStx kind,
                          void* __restrict__ data,
                          uint32_t len);

ELY_EXPORT void ely_stx_node_destroy(struct ElyStxNode* stx);

struct ElyReader
{
    struct ElyLexer lexer;
};

#ifdef __cplusplus
}
#endif