#include "ely/lex/lexer.h"

#include <assert.h>
#include <stdlib.h>

#include "ely/lex/token.h"

typedef struct ely_lexer
{
    const char* cursor;
    uint32_t    offset;
    uint32_t    line;
    uint32_t    col;
} ely_lexer;

ely_lexer* ely_lexer_create(const char* src)
{
    ely_lexer* lex = malloc(sizeof(*lex));
    lex->cursor    = src;
    lex->offset    = 0;
    lex->line      = 1;
    lex->col       = 1;

    return lex;
}

void ely_lexer_destroy(ely_lexer* lex)
{
    free(lex);
}

ely_position ely_lexer_position(const ely_lexer* lex)
{
    return (ely_position){
        .offset = lex->offset, .line = lex->line, .col = lex->col};
}

bool ely_lexer_empty(const ely_lexer* lex)
{
    return *lex->cursor == '\0';
}

const char* ely_lexer_cursor(const ely_lexer* lex)
{
    return lex->cursor;
}

static inline char peek_char(ely_lexer* lex)
{
    return *lex->cursor;
}

static inline void advance_char(ely_lexer* lex)
{
    ++lex->offset;
    ++lex->col;
    ++lex->cursor;
}

static inline char consume_char(ely_lexer* lex)
{
    char res = peek_char(lex);
    advance_char(lex);
    return res;
}

static inline bool is_digit(char ch)
{
    return '0' <= ch && ch <= '9';
}

static inline bool is_lower_alpha(char ch)
{
    return 'a' <= ch && ch <= 'z';
}

static inline bool is_upper_alpha(char ch)
{
    return 'A' <= ch && ch <= 'Z';
}

static inline bool is_alpha(char ch)
{
    return is_lower_alpha(ch) || is_upper_alpha(ch);
}

static inline bool is_identifier_start(char ch)
{
    switch (ch)
    {
    case '+':
    case '-':
    case '_':
        return true;
    default:
        return is_alpha(ch);
    }
}

static inline bool is_newline_start(char ch)
{
    switch (ch)
    {
    case '\n':
    case '\r':
        return true;
    default:
        return false;
    }
}

static inline bool is_identifier_continue(char ch)
{
    return is_identifier_start(ch) || is_digit(ch);
}

static inline void scan_line_comment(ely_lexer* lex, char ch)
{
    assert(ch == ';');
    const char* token_start = lex->cursor;
    advance_char(lex);

    ch = consume_char(lex);

    while (1)
    {
        switch (ch)
        {
        case '\r':
            ch = peek_char(lex);
            if (ch == '\n')
            {
                advance_char(lex);
                ++lex->line;
                lex->col = 1;
                return;
            }
            __attribute__((fallthrough));
        case '\n':
            ++lex->line;
            lex->col = 1;
            __attribute__((fallthrough));
        case '\0':
            return;
        }

        ch = consume_char(lex);
    }
}

static inline ely_token
scan_decimal(ely_lexer* lex, char ch, const char* token_start)
{
    assert(ch == '.');
    advance_char(lex);

    ch = peek_char(lex);

    while (is_digit(ch))
    {
        advance_char(lex);
        ch = peek_char(lex);
    }

    return (ely_token){.type  = ELY_TOKEN_DEC,
                       .len   = lex->cursor - token_start,
                       .start = token_start};
}

static inline ely_token
scan_number_cont(ely_lexer* lex, char ch, const char* token_start)
{
    assert(is_digit(ch));
    advance_char(lex);

    ch = peek_char(lex);
    while (is_digit(ch))
    {
        advance_char(lex);
        ch = peek_char(lex);
    }

    if (ch == '.')
    {
        return scan_decimal(lex, ch, token_start);
    }

    return (ely_token){.type  = ELY_TOKEN_INT,
                       .len   = lex->cursor - token_start,
                       .start = token_start};
}

static inline ely_token scan_number(ely_lexer* lex, char ch)
{
    return scan_number_cont(lex, ch, lex->cursor);
}

static inline ely_token
scan_identifier_cont(ely_lexer* lex, char ch, const char* token_start)
{
    assert(is_identifier_continue(ch));
    advance_char(lex);

    ch = peek_char(lex);
    while (is_identifier_continue(ch))
    {
        advance_char(lex);
        ch = peek_char(lex);
    }

    return (ely_token){.type  = ELY_TOKEN_IDENTIFIER,
                       .len   = lex->cursor - token_start,
                       .start = token_start};
}

static inline ely_token scan_sign(ely_lexer* lex, char ch)
{
    assert(ch == '+' || ch == '-');
    const char* token_start = lex->cursor;
    advance_char(lex);

    ch = peek_char(lex);
    if (is_digit(ch))
    {
        return scan_number_cont(lex, ch, token_start);
    }
    else
    {
        return scan_identifier_cont(lex, ch, token_start);
    }
}

static inline ely_token scan_identifier(ely_lexer* lex, char ch)
{
    assert(is_identifier_start(ch));
    const char* token_start = lex->cursor;
    advance_char(lex);

    ch = peek_char(lex);

    if (is_identifier_continue(ch))
    {
        return scan_identifier_cont(lex, ch, token_start);
    }

    return (ely_token){.type  = ELY_TOKEN_IDENTIFIER,
                       .len   = lex->cursor - token_start,
                       .start = token_start};
}

static inline ely_token scan_string_lit(ely_lexer* lex, char ch)
{
    assert(ch == '"');
    const char* token_start = lex->cursor;
    advance_char(lex);

    bool escaping = false;
    ch            = peek_char(lex);

    while (escaping || ch != '"')
    {
        if (ch == '\0')
        {
            return (ely_token){.type  = ELY_TOKEN_UNTERMINATED_STRING,
                               .len   = lex->cursor - token_start,
                               .start = token_start};
        }
        else if (ch == '\\')
        {
            escaping = !escaping;
            advance_char(lex);
            ch = peek_char(lex);
            continue;
        }

        escaping = false;
        advance_char(lex);
        ch = peek_char(lex);
    }

    advance_char(lex);
    return (ely_token){.type  = ELY_TOKEN_STRING,
                       .len   = lex->cursor - token_start,
                       .start = token_start};
}

static inline ely_token scan_single(ely_lexer* lex, ely_token_type ty)
{
    const char*  token_start = lex->cursor;
    ely_position pos         = ely_lexer_position(lex);
    advance_char(lex);
    return (ely_token){.type = ty, .len = 1, .start = token_start, .pos = pos};
}

static inline ely_token scan_token(ely_lexer* lex)
{
loop:
    char ch = peek_char(lex);
    switch (ch)
    {
    case '\0':
        __builtin_unreachable();
    case ';':
        scan_line_comment(lex, ch);
    case '\n':
    case ' ':
    case '\t':
    case '\v':
        goto loop;
    case '"':
        return scan_string_lit(lex, ch);
    case '(':
        return scan_single(lex, ELY_TOKEN_LPAREN);
    case ')':
        return scan_single(lex, ELY_TOKEN_RPAREN);
    case '[':
        return scan_single(lex, ELY_TOKEN_LBRACKET);
    case ']':
        return scan_single(lex, ELY_TOKEN_RBRACKET);
    case '{':
        return scan_single(lex, ELY_TOKEN_LBRACE);
    case '}':
        return scan_single(lex, ELY_TOKEN_RBRACE);
    case '-':
    case '+':
        return scan_sign(lex, ch);
    default:
        if (is_digit(ch))
        {
            return scan_number(lex, ch);
        }
        else if (is_identifier_start(ch))
        {
            return scan_identifier(lex, ch);
        }
    }

    return scan_single(lex, ELY_TOKEN_UNKNOWN_CHAR);
}

uint32_t ely_lexer_scan_tokens(ely_lexer* lex, ely_token* dst, uint32_t dst_len)
{
    uint32_t i = 0;

    if (dst_len == 0 || *lex->cursor == '\0')
    {
        return 0;
    }

    ely_token tok;
    do
    {
        tok    = scan_token(lex);
        dst[i] = tok;
        ++i;
    } while (*lex->cursor != '\0');

    return i;
}