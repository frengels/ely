#include "ely/lex/lexer.h"
#include "ely/lex/token.h"

#include "common.h"

static inline ely_token scan(ely_lexer* lex)
{
    uint32_t marker_offset = lex->offset;

loop:
    /*!include:re2c "unicode_categories.re" */

    /*!re2c
    re2c:yyfill:enable = 0;
    re2c:eof = -1;
    re2c:api = custom;
    re2c:api:style = free-form;
    re2c:define:YYCTYPE = char;
    re2c:define:YYPEEK = "lex->src[lex->offset];";
    re2c:define:YYSKIP = "++lex->offset; ++lex->col;";
    re2c:define:YYBACKUP = "marker_offset = lex->offset;";
    re2c:define:YYRESTORE = "lex->offset = marker_offset;";

    ascii_space = " ";
    ascii_tab = "\t";
    ascii_vtab = "\v";
    unicode_space = [\u00a0\u1680\u180e\u2000-\u200b\u202f\u205f\u3000\ufeff];
    space = ascii_space | ascii_tab | ascii_vtab | unicode_space;
    many_space = space+;
    newline_lf = "\n";
    newline_cr = "\r";
    newline_crlf = "\r\n";
    newline = newline_lf | newline_cr | newline_crlf;

    id_start = L | Nl | [_-];
    id_continue = id_start | Mn | Mc | Nd | Pc | [\u200D\u05F3];
    identifier = id_start id_continue*;

    dig = [0-9];
    int = dig+;
    decimal = int? . int;

    lparen = "(";
    rparen = ")";
    lbracket = "[";
    rbracket = "]";
    lbrace = "{";
    rbrace = "}";

    many_space { goto loop; }
    newline {
        ++lex->line;
        lex->col = 1;
        goto loop;
    }

    identifier { return (ely_token){.type = ELY_TOKEN_IDENTIFIER}; }
    int { return (ely_token){.type = ELY_TOKEN_INT}; }
    decimal { return (ely_token){.type = ELY_TOKEN_DEC}; }
    lparen { return (ely_token){.type = ELY_TOKEN_LPAREN}; }
    rparen { return (ely_token){.type = ELY_TOKEN_RPAREN}; }
    lbracket { return (ely_token){.type = ELY_TOKEN_LBRACKET}; }
    rbracket { return (ely_token){.type = ELY_TOKEN_RBRACKET}; }
    lbrace { return (ely_token){.type = ELY_TOKEN_LBRACE}; }
    rbrace { return (ely_token){.type = ELY_TOKEN_RBRACE}; }
    * { return (ely_token){.type = ELY_TOKEN_UNKNOWN_CHAR}; }
    */
}

uint32_t ely_lexer_scan_tokens(ely_lexer*  lex,
                                      ely_token*  dst,
                                      uint32_t    dst_len)
{
    uint32_t i = 0;

    if (dst_len == 0)
    {
        return 0;
    }

    ely_token tok;
    do
    {
        tok    = scan(lex);
        dst[i] = tok;
        ++i;
    } while (lex->src[lex->offset] != 0);

    return i;
}