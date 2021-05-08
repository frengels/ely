#include "ely/lexer.h"

#include <assert.h>
#include <stdbool.h>

#include "ely/defines.h"

static inline const char* ely_peek_data(struct ElyLexer* lex)
{
    return &lex->src[lex->pos];
}

static inline void ely_lex_advance(struct ElyLexer* lex)
{
    ++lex->pos;
}

static inline bool ely_lex_at_end(struct ElyLexer* lex)
{
    return lex->pos == lex->len;
}

static inline char ely_lex_peek_char(struct ElyLexer* lex)
{
    assert(!ely_lex_at_end(lex));
    return lex->src[lex->pos];
}

static inline bool is_newline_start(char c)
{
    return c == '\r' || c == '\n';
}

static inline bool is_delimiter(char c)
{
    switch (c)
    {
    // atmosphere
    case ' ':
    case '\t':
    case '\r':
    case '\n':
    case ';':
    // enclosing
    case '(':
    case ')':
    case '{':
    case '}':
    case '[':
    case ']':
    case '"':
    // abbrevs
    case '\'':
    case '#':
    case '$':
    case '!':
    case ':':
    case '|':
    case '@':
    case '&':
        return true;
    default:
        return false;
    }
}

static inline uint32_t parse_bad_stx();

void ely_lex_create(struct ElyLexer* lex,
                    const char* __restrict__ src,
                    uint32_t len)
{
    lex->src = src;
    lex->len = len;
    lex->pos = 0;
}

uint32_t ely_lex_src(struct ElyLexer* lex,
                     struct ElyToken* __restrict__ token_buf,
                     uint32_t buf_len)
{
    uint32_t buf_i = 0;
    for (; buf_i != buf_len; ++buf_i)
    {
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
            ++buf_i;
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
            ++buf_i;
        }
        break;
        case '\r':
            ch = ely_lex_peek_char(lex);
            if (ch == '\n')
            {
                ely_lex_advance(lex);
                token_buf[buf_i].kind = ELY_TOKEN_NEWLINE_CRLF;
                token_buf[buf_i].len  = 2;
                ++buf_i;
            }
            else
            {
                token_buf[buf_i].kind = ELY_TOKEN_NEWLINE_CR;
                token_buf[buf_i].len  = 1;
                ++buf_i;
            }
            break;
        case '\n':
            token_buf[buf_i].kind = ELY_TOKEN_NEWLINE_LF;
            token_buf[buf_i].len  = 1;
            ++buf_i;
        case ';': {
            uint32_t tok_len = 1;
            ch               = ely_lex_peek_char(lex);
            while (!ely_lex_at_end(lex))
            {
                if (is_newline_start(ch))
                {
                    token_buf[buf_i].kind = ELY_TOKEN_COMMENT;
                    token_buf[buf_i].len  = tok_len;
                    ++buf_i;
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
            ++buf_i;
            break;
        case ')':
            token_buf[buf_i].kind = ELY_TOKEN_RPAREN;
            token_buf[buf_i].len  = 1;
            ++buf_i;
            break;
        case '[':
            token_buf[buf_i].kind = ELY_TOKEN_LBRACKET;
            token_buf[buf_i].len  = 1;
            ++buf_i;
            break;
        case ']':
            token_buf[buf_i].kind = ELY_TOKEN_RBRACKET;
            token_buf[buf_i].len  = 1;
            ++buf_i;
            break;
        case '{':
            token_buf[buf_i].kind = ELY_TOKEN_LBRACE;
            token_buf[buf_i].len  = 1;
            ++buf_i;
            break;
        case '}':
            token_buf[buf_i].kind = ELY_TOKEN_RBRACE;
            token_buf[buf_i].len  = 1;
            ++buf_i;
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
                    ++buf_i;
                    break;
                }
                else
                {}

            case 'f':
                ely_lex_advance(lex);
                ch = ely_lex_peek_char(lex);
                if (is_delimiter(ch))
                {
                    token_buf[buf_i].kind = ELY_TOKEN_FALSE_LIT;
                    token_buf[buf_i].len  = 2;
                    ++buf_i;
                    break;
                }
                else
                {}
            }
        case '\0':
            token_buf[buf_i].kind = ELY_TOKEN_EOF;
            token_buf[buf_i].len  = 0;
            ++buf_i;
            return buf_i;
        }
    }

    return buf_i;
}