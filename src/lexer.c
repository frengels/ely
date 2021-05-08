#include "ely/lexer.h"

#include <assert.h>
#include <stdbool.h>

#include "ely/defines.h"

static inline bool is_newline_start(char c)
{
    return c == '\r' || c == '\n';
}

static inline bool is_number(char c)
{
    return '0' <= c && c <= '9';
}

static inline bool is_atmosphere_start(char c)
{
    switch (c)
    {
    case ' ':
    case ';':
    case '\t':
        return true;
    default:
        ELY_MUSTTAIL return is_newline_start(c);
    }
}

static inline bool is_delimiter(char c)
{
    switch (c)
    {
    // enclosing
    case '(':
    case ')':
    case '{':
    case '}':
    case '[':
    case ']':
    case '"':
        return true;
    default:
        ELY_MUSTTAIL return is_atmosphere_start(c);
    }
}

static inline const char* ely_peek_data(ElyLexer* lex)
{
    return &lex->src[lex->pos];
}

static inline void ely_lex_advance(ElyLexer* lex)
{
    ++lex->pos;
}

static inline bool ely_lex_at_end(ElyLexer* lex)
{
    return lex->pos == lex->len;
}

static inline char ely_lex_peek_char(ElyLexer* lex)
{
    assert(!ely_lex_at_end(lex));
    return lex->src[lex->pos];
}

static inline const char* ely_lex_peek_data(ElyLexer* lex)
{
    assert(!ely_lex_at_end(lex));
    return &lex->src[lex->pos];
}

static inline uint32_t lex_advance_to_delimiter(ElyLexer* lexer)
{
    uint32_t len = 0;
    char     ch  = ely_lex_peek_char(lexer);
    while (!is_delimiter(ch))
    {
        ++len;
        ely_lex_advance(lexer);
        ch = ely_lex_peek_char(lexer);
    }

    return len;
}

static inline uint32_t parse_bad_stx();

static inline void lex_keyword(ElyLexer* lexer, ElyToken* dst)
{
    uint32_t kw_len = lex_advance_to_delimiter(lexer);
    dst->kind       = ELY_TOKEN_KEYWORD_LIT;
    dst->len        = 2 + kw_len;
}

// this will parse the fragment of the float
static inline void lex_float(ElyLexer* lexer, ElyToken* dst, uint32_t token_len)
{
    char c = ely_lex_peek_char(lexer);

    while (is_number(c))
    {
        ++token_len;
        ely_lex_advance(lexer);
        c = ely_lex_peek_char(lexer);
    }

    if (is_delimiter(c))
    {
        dst->kind = ELY_TOKEN_FLOAT_LIT;
        dst->len  = token_len;
        return;
    }
    else
    {
        ++token_len;
        ely_lex_advance(lexer);
        uint32_t advanced = lex_advance_to_delimiter(lexer);
        dst->kind         = ELY_TOKEN_ID;
        dst->len          = token_len + advanced;
        return;
    }
}

static inline void
parse_number_continue(ElyLexer* lexer, ElyToken* dst, uint32_t token_len)
{
    char c = ely_lex_peek_char(lexer);
    while (is_number(c))
    {
        ++token_len;
        ely_lex_advance(lexer);
        c = ely_lex_peek_char(lexer);
    }

    // hit a non number character;
    if (c == '.')
    {
        ++token_len;
        ely_lex_advance(lexer);
        // lexing a float now
        ELY_MUSTTAIL return lex_float(lexer, dst, token_len);
    }
    else if (is_delimiter(c))
    {
        dst->kind = ELY_TOKEN_INT_LIT;
        dst->len  = token_len;
        return;
    }
    else
    {
        ++token_len;
        ely_lex_advance(lexer);
        uint32_t advanced = lex_advance_to_delimiter(lexer);

        dst->kind = ELY_TOKEN_ID;
        dst->len  = token_len + advanced;
        return;
    }
}

static inline void
parse_number_start(ElyLexer* lexer, ElyToken* dst, uint32_t token_len)
{
    char c = ely_lex_peek_char(lexer);

    if (is_number(c))
    {
        ++token_len;
        ely_lex_advance(lexer);
        ELY_MUSTTAIL return parse_number_continue(lexer, dst, token_len);
    }
    else if (is_delimiter(c))
    {
        dst->kind = ELY_TOKEN_ID;
        dst->len  = token_len;
        return;
    }
    else
    {
        ++token_len;
        ely_lex_advance(lexer);
        uint32_t advanced = lex_advance_to_delimiter(lexer);
        dst->kind         = ELY_TOKEN_ID;
        dst->len          = token_len + advanced;
        return;
    }
}

static inline void lex_char(ElyLexer* lexer, ElyToken* dst)
{
    uint32_t advanced = lex_advance_to_delimiter(lexer);

    dst->kind = ELY_TOKEN_CHAR_LIT;
    dst->len  = advanced + 2;
}

static inline void parse_sign(ElyLexer* lex, ElyToken* dst, char sign)
{}

void ely_lex_create(ElyLexer* lex, const char* __restrict__ src, uint32_t len)
{
    lex->src = src;
    lex->len = len;
    lex->pos = 0;
}

uint32_t
ely_lex_src(ElyLexer* lex, ElyToken* __restrict__ token_buf, uint32_t buf_len)
{
    uint32_t buf_i = 0;
    for (; buf_i != buf_len; ++buf_i)
    {
        // check if we've reached the end
        if (ely_lex_at_end(lex))
        {
            token_buf[buf_i].kind = ELY_TOKEN_EOF;
            token_buf[buf_i].len  = 0;
            ++buf_i;
            return buf_i;
        }

        char ch = ely_lex_peek_char(lex);
        ely_lex_advance(lex);
        switch (ch)
        {
        case ' ': {
            uint32_t tok_len = 1;
            ch               = ely_lex_peek_char(lex);
            while (ch == ' ')
            {

                ely_lex_advance(lex);
                ++tok_len;
                if (!ely_lex_at_end(lex))
                {
                    ch = ely_lex_peek_char(lex);
                }
                else
                {
                    break;
                }
            }

            token_buf[buf_i].kind = ELY_TOKEN_WHITESPACE;
            token_buf[buf_i].len  = tok_len;
        }
        break;
        case '\t': {
            uint32_t tok_len = 1;
            ch               = ely_lex_peek_char(lex);
            while (ch == '\t')
            {
                ely_lex_advance(lex);
                ++tok_len;
                if (!ely_lex_at_end(lex))
                {
                    ch = ely_lex_peek_char(lex);
                }
                else
                {
                    break;
                }
            }

            token_buf[buf_i].kind = ELY_TOKEN_TAB;
            token_buf[buf_i].len  = tok_len;
        }
        break;
        case '\r':
            ch = ely_lex_peek_char(lex);
            if (ch == '\n')
            {
                ely_lex_advance(lex);
                token_buf[buf_i].kind = ELY_TOKEN_NEWLINE_CRLF;
                token_buf[buf_i].len  = 2;
            }
            else
            {
                token_buf[buf_i].kind = ELY_TOKEN_NEWLINE_CR;
                token_buf[buf_i].len  = 1;
            }
            break;
        case '\n':
            token_buf[buf_i].kind = ELY_TOKEN_NEWLINE_LF;
            token_buf[buf_i].len  = 1;
            break;
        case ';': {
            uint32_t tok_len = 1;
            ch               = ely_lex_peek_char(lex);
            while (!ely_lex_at_end(lex))
            {
                if (is_newline_start(ch))
                {
                    token_buf[buf_i].kind = ELY_TOKEN_COMMENT;
                    token_buf[buf_i].len  = tok_len;
                    break;
                }
                else
                {
                    ++tok_len;
                    ely_lex_advance(lex);
                    ch = ely_lex_peek_char(lex);
                }
            }
        }
        break;
        case '(':
            token_buf[buf_i].kind = ELY_TOKEN_LPAREN;
            token_buf[buf_i].len  = 1;
            break;
        case ')':
            token_buf[buf_i].kind = ELY_TOKEN_RPAREN;
            token_buf[buf_i].len  = 1;
            break;
        case '[':
            token_buf[buf_i].kind = ELY_TOKEN_LBRACKET;
            token_buf[buf_i].len  = 1;
            break;
        case ']':
            token_buf[buf_i].kind = ELY_TOKEN_RBRACKET;
            token_buf[buf_i].len  = 1;
            break;
        case '{':
            token_buf[buf_i].kind = ELY_TOKEN_LBRACE;
            token_buf[buf_i].len  = 1;
            break;
        case '}':
            token_buf[buf_i].kind = ELY_TOKEN_RBRACE;
            token_buf[buf_i].len  = 1;
            break;
        case '+':
        case '-':
            parse_sign(lex, &token_buf[buf_i], ch);
            break;
        case '#':
            ch = ely_lex_peek_char(lex);
            switch (ch)
            {
            case 't':
                ely_lex_advance(lex);
                ch = ely_lex_peek_char(lex);
                if (is_delimiter(ch))
                {
                    token_buf[buf_i].kind = ELY_TOKEN_TRUE_LIT;
                    token_buf[buf_i].len  = 2;
                    break;
                }
                else
                {}
                break;
            case 'f':
                ely_lex_advance(lex);
                ch = ely_lex_peek_char(lex);
                if (is_delimiter(ch))
                {
                    token_buf[buf_i].kind = ELY_TOKEN_FALSE_LIT;
                    token_buf[buf_i].len  = 2;
                    break;
                }
                else
                {}
                break;
            case ':':
                ely_lex_advance(lex);
                lex_keyword(lex, &token_buf[buf_i]);
                break;
            case '\\':
                ely_lex_advance(lex);
                lex_char(lex, &token_buf[buf_i]);
                break;
            case '\t':
            case '`':
            case ',':
            default:
                assert(false && "TODO: handle failure cases and abbreviations");
                break;
            }
        default:
            if (is_number(ch))
            {
                parse_number_continue(lex, &token_buf[buf_i], 1);
                break;
            }
            else
            {
                uint32_t advanced     = lex_advance_to_delimiter(lex);
                token_buf[buf_i].kind = ELY_TOKEN_ID;
                token_buf[buf_i].len  = advanced + 1;
                break;
            }
        }
    }
    return buf_i;
}