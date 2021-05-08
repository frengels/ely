#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "elyc/lexer.h"

enum ElycStx : uint8_t
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

struct ElycPosition
{
    uint32_t row;
    uint32_t col;
};

struct ElycStxLocation
{
    const char*         filename;
    uint32_t            start_byte;
    uint32_t            end_byte;
    struct ElycPosition start_pos;
    struct ElycPosition end_pos;
};

struct ElycStxNode
{

    void*    data;
    uint32_t len;
    // both of these get packed into the 4 bytes behind len
    enum ElycStx kind;
    bool         is_stx;

    struct ElycStxLocation location;
};

void elyc_stx_node_create(struct ElycStxNode* __restrict__ stx,
                          enum ElycStx kind,
                          void* __restrict__ data,
                          uint32_t len);

void elyc_stx_node_destroy(struct ElycStxNode* stx);

struct ElycReader
{
    struct ElycLexer lexer;
};
