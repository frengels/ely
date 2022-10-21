#pragma once

#ifndef ELY_TYPE_H
#define ELY_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

enum ely_type_kind
{
    ELY_TYPE_POISON,

    ELY_TYPE_U32,
    ELY_TYPE_U64,
    ELY_TYPE_S32,
    ELY_TYPE_S64,

    ELY_TYPE_F32,
    ELY_TYPE_F64,

    ELY_TYPE_STRING_LIT,
    ELY_TYPE_INT_LIT,
    ELY_TYPE_DEC_LIT,
};

struct ely_type;

#ifdef __cplusplus
}
#endif

#endif