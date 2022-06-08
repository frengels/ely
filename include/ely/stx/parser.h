#ifndef ELY_STX_PARSER_H
#define ELY_STX_PARSER_H

#include <ely/lex/token.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ely_lexer      ely_lexer;
typedef struct ely_stx_parser ely_stx_parser;

ely_stx_parser* ely_stx_parser_create(ely_lexer* lex);
void            ely_stx_parser_destroy(ely_stx_parser* parser);



#ifdef __cplusplus
}
#endif

#endif