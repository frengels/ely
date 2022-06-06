#ifndef SRC_LEX_COMMON_H
#define SRC_LEX_COMMON_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_lexer
{
    const char* src;
    uint32_t    offset;
    uint32_t    line;
    uint32_t    col;
} ely_lexer;

#ifdef __cplusplus
}
#endif

#endif