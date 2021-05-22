#pragma once

#include "ely/export.h"
#include "ely/reader.h"

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ElyExpr     ElyExpr;
typedef struct ElyCallExpr ElyCallExpr;
typedef struct ElyLetExpr  ElyLetExpr;
typedef struct ElyFnExpr   ElyFnExpr;
typedef struct ElyDef      ElyDef;

enum ElyExprKind
{
    ELY_EXPR_ID,
    ELY_EXPR_CALL,
    ELY_EXPR_LET,
    ELY_EXPR_IF,
    ELY_EXPR_FN,
    ELY_EXPR_LIT,
    ELY_EXPR_RET,
};

enum ElyLitKind
{
    ELY_LIT_STRING,
    ELY_LIT_INT,
    ELY_LIT_FLOAT,
    ELY_LIT_CHAR,
    ELY_LIT_BOOL,
};

enum ElyCharType
{
    ELY_CHAR8,
    // ELY_CHAR16,
    // ELY_CHAR32,
};

enum ElyIntType
{
    ELY_INT_S8,
    ELY_INT_U8,
    ELY_INT_S16,
    ELY_INT_U16,
    ELY_INT_S32,
    ELY_INT_U32,
    ELY_INT_S64,
    ELY_INT_U64,
};

enum ElyFloatType
{
    ELY_FLOAT_F32,
    ELY_FLOAT_F64,
};

typedef struct ElyIdExpr
{
    ElyNode* id;
} ElyIdExpr;

typedef struct ElyCallExpr
{

} ElyCallExpr;

typedef struct ElyLetExpr
{
    ElyNode* ids;
    ElyExpr* init;
    size_t   len;

    ElyExpr* expr;
} ElyLetExpr;

typedef struct ElyIfExpr
{
    ElyExpr* cond;
    ElyExpr* then;
    ElyExpr* else_;
} ElyIfExpr;

typedef struct ElyFnExpr
{
    ElyNode* ids;
    size_t   len;
} ElyFnExpr;

typedef struct ElyRetExpr
{
    ElyNode* ret_id;
    ElyExpr* ret_expr;
} ElyRetExpr;

typedef struct ElyIntLit
{
    enum ElyIntType ty;
    union
    {
        int8_t   i8;
        uint8_t  u8;
        int16_t  i16;
        uint16_t u16;
        int32_t  i32;
        uint32_t u32;
        int64_t  i64;
        uint64_t u64;
    };
} ElyIntLit;

typedef struct ElyFloatLit
{
    enum ElyFloatType ty;
    union
    {
        float  f32;
        double f64;
    };
} ElyFloatLit;

typedef struct ElyCharLit
{
    enum ElyCharType ty;
    union
    {
        char ch;
    };
} ElyCharLit;

typedef struct ElyStringLit
{
    const char* str;
    size_t      len;
} ElyStringLit;

typedef struct ElyBoolLit
{
    ElyNode* node;
    bool     val;
} ElyBoolLit;

typedef struct ElyLitExpr
{
    enum ElyLitKind kind;
    union
    {
        ElyIntLit    int_lit;
        ElyFloatLit  float_lit;
        ElyCharLit   char_lit;
        ElyStringLit str_lit;
        ElyBoolLit   bool_lit;
    };
} ElyLitExpr;

typedef struct ElyExpr
{
    enum ElyExprKind kind;
    union
    {
        ElyIdExpr   id_expr;
        ElyCallExpr call_expr;
        ElyLetExpr  let_expr;
        ElyIfExpr   if_expr;
        ElyFnExpr   fn_expr;
        ElyLitExpr  lit_expr;
        ElyRetExpr  ret_expr;
    };
} ElyExpr;

typedef struct ElyDef
{
    ElyNode* id;
    ElyExpr  expr;
} ElyDef;

ELY_EXPORT ElyExpr ely_ast_parse_expr(ElyNode* n) ELY_READONLY;
ELY_EXPORT ElyDef  ely_ast_parse_toplevel(ElyNode* n) ELY_READONLY;

#ifdef __cplusplus
}
#endif
