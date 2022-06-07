#include "ely/lex/token.h"

const char* ely_token_type_to_string(ely_token_type ty)
{
    switch (ty)
    {
    case ELY_TOKEN_EOF:
        return "eof";
    case ELY_TOKEN_LPAREN:
        return "lparen";
    case ELY_TOKEN_RPAREN:
        return "rparen";
    case ELY_TOKEN_LBRACKET:
        return "lbracket";
    case ELY_TOKEN_RBRACKET:
        return "rbracket";
    case ELY_TOKEN_LBRACE:
        return "lbrace";
    case ELY_TOKEN_RBRACE:
        return "rbrace";
    case ELY_TOKEN_IDENTIFIER:
        return "identifier";
    case ELY_TOKEN_STRING:
        return "string";
    case ELY_TOKEN_INT:
        return "int";
    case ELY_TOKEN_DEC:
        return "dec";
    case ELY_TOKEN_CHAR:
        return "char";
    case ELY_TOKEN_BOOL:
        return "bool";
    case ELY_TOKEN_UNKNOWN_CHAR:
        return "unknown char";
    case ELY_TOKEN_UNTERMINATED_STRING:
        return "unterminated string";
    default:
        return "<unknown>";
    }
}