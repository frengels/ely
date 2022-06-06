#ifndef ELY_STX_IDENTIFIER_H
#define ELY_STX_IDENTIFIER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    char* str;
    bool  is_literal;
} ely_stx_identifier;

void ely_stx_identifier_destroy(ely_stx_identifier* ident);

#ifdef __cplusplus
}
#endif

#endif