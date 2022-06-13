#ifndef ELY_STX_PARSER_H
#define ELY_STX_PARSER_H

#include "ely/ilist.h"
#include "ely/lex/token.hpp"
#include "ely/stx/source.h"

#include "ely/export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_lexer      ely_lexer;
typedef struct ely_stx_parser ely_stx_parser;

ELY_EXPORT ely_stx_parser* ely_stx_parser_create(ely_lexer* lex);
ELY_EXPORT void            ely_stx_parser_destroy(ely_stx_parser* parser);
ELY_EXPORT void            ely_stx_parser_parse(ely_ilist*      res_list,
                                                ely_stx_parser* parser);

#ifdef __cplusplus
}
#endif

#endif