#include "elyc/lexer.h"

#include <assert.h>
#include <stdbool.h>

#include "elyc/defines.h"

static inline const char* elyc_peek_data(struct ElycLexer* lex)
{
    return &lex->src[lex->pos];
}

static inline void elyc_lex_advance(struct ElycLexer* lex)
{
    ++lex->pos;
}

static inline bool elyc_lex_at_end(struct ElycLexer* lex)
{
    return lex->pos == lex->len;
}

static inline char elyc_lex_peek_char(struct ElycLexer* lex)
{
    assert(!elyc_lex_at_end(lex));
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

uint32_t elyc_lex_src(struct ElycLexer* lex,
                      struct ElycToken* __restrict__ token_buf,
                      uint32_t buf_len)
{
    uint32_t buf_i = 0;
    for (; buf_i != buf_len; ++buf_i)
    {
        if (elyc_lex_at_end(lex))
        {
            token_buf[buf_i].kind = ELY_TOKEN_EOF;
            token_buf[buf_i].len  = 0;
            ++buf_i;
            return buf_i;
        }

        char ch = elyc_lex_peek_char(lex);
        elyc_lex_advance(lex);
        switch (ch)
        {
        case ' ': {
            uint32_t tok_len = 1;
            ch               = elyc_lex_peek_char(lex);
            while (ch == ' ')
            {

                elyc_lex_advance(lex);
                ++tok_len;
                if (!elyc_lex_at_end(lex))
                {
                    ch = elyc_lex_peek_char(lex);
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
            ch               = elyc_lex_peek_char(lex);
            while (ch == '\t')
            {
                elyc_lex_advance(lex);
                ++tok_len;
                if (!elyc_lex_at_end(lex))
                {
                    ch = elyc_lex_peek_char(lex);
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
            ch = elyc_lex_peek_char(lex);
            if (ch == '\n')
            {
                elyc_lex_advance(lex);
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
            token_buf[buf_i].kind = ELY_TOKEN_LPAREN;
            token_buf[buf_i].len  = 1;
            ++buf_i;
        case ';': {
            uint32_t tok_len = 1;
            ch               = elyc_lex_peek_char(lex);
            while (!elyc_lex_at_end(lex))
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
                    elyc_lex_advance(lex);
                    ch = elyc_lex_peek_char(lex);
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
            ch = elyc_lex_peek_char(lex);
            switch (ch)
            {
            case 't':
                elyc_lex_advance(lex);
                ch = elyc_lex_peek_char(lex);
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
                elyc_lex_advance(lex);
                ch = elyc_lex_peek_char(lex);
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