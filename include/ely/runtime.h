#ifndef ELY_RUNTIME_H
#define ELY_RUNTIME_H

#include <cstdio>

#include "ely/config.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ely_expr;

struct ely_runtime;
struct ely_value;

enum ely_value_kind
{
    ELY_VALUE_UINT64,
    ELY_VALUE_UINT32,
    ELY_VALUE_UINT16,
    ELY_VALUE_UINT8,
    ELY_VALUE_SINT64,
    ELY_VALUE_SINT32,
    ELY_VALUE_SINT16,
    ELY_VALUE_SINT8,
    ELY_VALUE_FLOAT,
    ELY_VALUE_DOUBLE,
    ELY_VALUE_STRING_LIT,
    ELY_VALUE_INT_LIT,
    ELY_VALUE_DEC_LIT,
};

ELY_EXPORT struct ely_runtime* ely_runtime_create();
ELY_EXPORT void                ely_runtime_destroy(struct ely_runtime* rt);

ELY_EXPORT struct ely_value* ely_runtime_eval(struct ely_runtime* rt,
                                              struct ely_expr*    e);
ELY_EXPORT void              ely_runtime_push_def(struct ely_runtime* rt,
                                                  const char*         name,
                                                  struct ely_value*   val);

ELY_EXPORT void   ely_value_print(const struct ely_value* v, FILE* f);
ELY_EXPORT size_t ely_value_to_chars(const struct ely_value* v,
                                     char*                   buf,
                                     size_t                  buf_len);
ELY_EXPORT enum ely_value_kind ely_value_get_kind(const struct ely_value* v);

#ifdef __cplusplus
}
#endif

#endif