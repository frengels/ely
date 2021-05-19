#include "ely/lexer.h"

#include <assert.h>
#include <stdbool.h>

#include "ely/defines.h"
#include "ely/token.h"

ELY_NODISCARD static inline bool is_newline_start(char c)
{
    return c == '\r' || c == '\n';
}

ELY_NODISCARD static inline bool is_number(char c)
{
    return '0' <= c && c <= '9';
}

ELY_NODISCARD static inline bool is_atmosphere_start(char c)
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

ELY_NODISCARD static inline bool is_delimiter(char c)
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

ELY_NODISCARD static inline const char*
advance_to_delimiter(const char* it, const char* end)
{
    if (it != end)
    {
        char ch = *it;

        while (!is_delimiter(ch))
        {
            ++it;
            if (it == end)
            {
                break;
            }
            ch = *it;
        }
    }
    return it;
}

ELY_NODISCARD static inline const char*
parse_keyword(const char* it, const char* end, ElyToken* dst)
{
    const char* new_it = advance_to_delimiter(it, end);
    dst->kind          = ELY_TOKEN_KEYWORD_LIT;
    dst->len           = 2 + (new_it - it);
    it                 = new_it;
    return it;
}

// this will parse the fragment of the float
ELY_NODISCARD static inline const char*
parse_float(const char* it, const char* end, ElyToken* dst, uint32_t token_len)
{
    char c = *it;

    while (is_number(c))
    {
        ++token_len;
        ++it;
        c = *it;
    }

    if (is_delimiter(c))
    {
        dst->kind = ELY_TOKEN_FLOAT_LIT;
        dst->len  = token_len;
    }
    else
    {
        ++token_len;
        ++it;
        const char* new_it = advance_to_delimiter(it, end);
        dst->kind          = ELY_TOKEN_ID;
        dst->len           = token_len + (new_it - it);
        it                 = new_it;
    }

    return it;
}

ELY_NODISCARD static inline const char*
parse_number_continue(const char* it,
                      const char* end,
                      ElyToken*   dst,
                      uint32_t    token_len)
{
    char c = *it;
    while (is_number(c))
    {
        ++token_len;
        ++it;
        c = *it;
    }

    // hit a non number character;
    if (c == '.')
    {
        ++token_len;
        ++it;
        // lexing a float now
        ELY_MUSTTAIL return parse_float(it, end, dst, token_len);
    }
    else if (is_delimiter(c))
    {
        dst->kind = ELY_TOKEN_INT_LIT;
        dst->len  = token_len;
    }
    else
    {
        ++token_len;
        ++it;
        const char* new_it = advance_to_delimiter(it, end);

        dst->kind = ELY_TOKEN_ID;
        dst->len  = token_len + (new_it - it);
        it        = new_it;
    }

    return it;
}

ELY_NODISCARD static inline const char*
parse_char(const char* it, const char* end, ElyToken* dst)
{
    const char* new_it = advance_to_delimiter(it, end);

    dst->kind = ELY_TOKEN_CHAR_LIT;
    dst->len  = (new_it - it) + 2;
    it        = new_it;
    return it;
}

ELY_NODISCARD static inline const char*
parse_sign(const char* it, const char* end, ElyToken* dst, size_t token_len)
{
    char ch = *it;

    if (is_number(ch))
    {
        ++it;
        ++token_len;
        ELY_MUSTTAIL return parse_number_continue(it, end, dst, token_len);
    }
    else if (is_delimiter(ch))
    {
        dst->kind = ELY_TOKEN_ID;
        dst->len  = token_len;
    }
    else
    {
        ++token_len;
        ++it;
        const char* new_it = advance_to_delimiter(it, end);
        dst->kind          = ELY_TOKEN_ID;
        dst->len           = token_len + (new_it - it);
        it                 = new_it;
    }

    return it;
}

ELY_NODISCARD ElyLexResult ely_lex(const char* __restrict__ src,
                                            size_t src_len,
                                            ElyToken* __restrict__ token_buf,
                                            size_t buf_len)
{
    size_t      buf_i = 0;
    const char* end   = src + src_len;
    const char* it    = src;
    for (; buf_i != buf_len; ++buf_i)
    {
        // check if we've reached the end
        if (it == end)
        {
            token_buf[buf_i].kind = ELY_TOKEN_EOF;
            token_buf[buf_i].len  = 0;
            ++buf_i;
            ElyLexResult res = {.bytes_processed = (it - src),
                                .tokens_read     = buf_i};
            return res;
        }

        char ch = *it++;

        switch (ch)
        {
        case ' ': {
            uint32_t tok_len = 1;
            ch               = *it;
            while (ch == ' ')
            {
                ++it;
                ++tok_len;
                if (it != end)
                {
                    ch = *it;
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
            ch               = *it;
            while (ch == '\t')
            {
                ++it;
                ++tok_len;
                if (it != end)
                {
                    ch = *it;
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
            ch = *it;
            if (ch == '\n')
            {
                ++it;
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
            ch               = *it;
            while (it != end)
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
                    ++it;
                    ch = *it;
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
            it = parse_sign(it, end, &token_buf[buf_i], 1);
            break;
        case '#':
            ch = *it;
            switch (ch)
            {
            case 't':
                ++it;
                ch = *it;
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
                ++it;
                ch = *it;
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
                ++it;
                it = parse_keyword(it, end, &token_buf[buf_i]);
                break;
            case '\\':
                ++it;
                it = parse_char(it, end, &token_buf[buf_i]);
                break;
            case '\'':
            case '`':
            case ',':
            default:
                assert(false && "TODO: handle abbreviations starting with #");
                break;
            }
            break;
        default:
            if (is_number(ch))
            {
                it = parse_number_continue(it, end, &token_buf[buf_i], 1);
                break;
            }
            else
            {
                const char* new_it    = advance_to_delimiter(it, end);
                token_buf[buf_i].kind = ELY_TOKEN_ID;
                token_buf[buf_i].len  = (new_it - it) + 1;
                it                    = new_it;
                break;
            }
        }
    }
    ElyLexResult res = {.bytes_processed = (it - src), .tokens_read = buf_i};
    return res;
}
