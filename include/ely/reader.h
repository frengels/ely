#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "ely/export.h"
#include "ely/lexer.h"

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

typedef struct ElyStxNode
{

    void*    data;
    uint32_t len;
    // both of these get packed into the 4 bytes behind len
    enum ElyStx kind;
    bool        is_stx;

    ElyStxLocation location;
} ElyStxNode;

ELY_EXPORT void ely_stx_node_create(ElyStxNode* __restrict__ stx,
                                    enum ElyStx kind,
                                    void* __restrict__ data,
                                    uint32_t data_len);

ELY_EXPORT void ely_stx_node_destroy(ElyStxNode* stx);

typedef struct ElyReader
{
    const char* filename;

    ElyPosition current_pos;
    uint32_t    current_byte;
} ElyReader;

ELY_EXPORT void ely_reader_create(ElyReader* reader, const char* filename);

ELY_EXPORT ElyStxNode*
           ely_reader_read(ElyReader* reader, ElyToken* tokens, uint32_t len);

#ifdef __cplusplus
}
#endif