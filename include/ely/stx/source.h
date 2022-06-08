#ifndef ELY_STX_SOURCE_H
#define ELY_STX_SOURCE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_stx_datum ely_stx_datum;

typedef struct ely_stx_source
{
    ely_stx_datum* data;
    size_t         data_len;
} ely_stx_source;

#ifdef __cplusplus
}
#endif

#endif