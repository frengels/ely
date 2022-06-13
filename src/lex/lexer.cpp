#include "ely/lex/lexer.hpp"

#include <assert.h>
#include <stdlib.h>

#include "ely/lex/token.hpp"

using namespace ely;

namespace
{
extern "C" {
static inline char peek_char(ely::lexer& lex)
{
    return *lex.cursor;
}

static inline void advance_char(ely::lexer& lex)
{
    ++lex.cursor;
}

static inline char consume_char(ely::lexer& lex)
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

void scan_line_comment(ely::lexer& lex, char ch)
{
    assert(ch == ';');
    const char* token_start = lex.cursor;
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
                return;
            }
            __attribute__((fallthrough));
        case '\n':
            __attribute__((fallthrough));
        case '\0':
            return;
        }

        ch = consume_char(lex);
    }
}

ely::token scan_decimal(ely::lexer& lex, char ch, const char* token_start)
{
    assert(ch == '.');
    advance_char(lex);

    ch = peek_char(lex);

    while (is_digit(ch))
    {
        advance_char(lex);
        ch = peek_char(lex);
    }

    return {token_type::decimal_literal,
            token_start,
            static_cast<uint32_t>(lex.cursor - token_start)};
}

ely::token scan_number_cont(ely::lexer& lex, char ch, const char* token_start)
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

    return {token_type::int_literal,
            token_start,
            static_cast<uint32_t>(lex.cursor - token_start)};
}

ely::token scan_number(ely::lexer& lex, char ch)
{
    return scan_number_cont(lex, ch, lex.cursor);
}

ely::token
scan_identifier_cont(ely::lexer& lex, char ch, const char* token_start)
{
    assert(is_identifier_continue(ch));
    advance_char(lex);

    ch = peek_char(lex);
    while (is_identifier_continue(ch))
    {
        advance_char(lex);
        ch = peek_char(lex);
    }

    return {token_type::identifier,
            token_start,
            static_cast<uint32_t>(lex.cursor - token_start)};
}

ely::token scan_sign(ely::lexer& lex, char ch)
{
    assert(ch == '+' || ch == '-');
    const char* token_start = lex.cursor;
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

ely::token scan_identifier(ely::lexer& lex, char ch)
{
    assert(is_identifier_start(ch));
    const char* token_start = lex.cursor;
    advance_char(lex);

    ch = peek_char(lex);

    if (is_identifier_continue(ch))
    {
        return scan_identifier_cont(lex, ch, token_start);
    }

    return {token_type::identifier,
            token_start,
            static_cast<uint32_t>(lex.cursor - token_start)};
}

ely::token scan_string_lit(ely::lexer& lex, char ch)
{
    assert(ch == '"');
    const char* token_start = lex.cursor;
    advance_char(lex);

    bool escaping = false;
    ch            = peek_char(lex);

    while (escaping || ch != '"')
    {
        if (ch == '\0')
        {
            return {token_type::unterminated_string_literal,
                    token_start,
                    static_cast<uint32_t>(lex.cursor - token_start)};
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
    return {token_type::string_literal,
            token_start,
            static_cast<uint32_t>(lex.cursor - token_start)};
}

ely::token scan_single(ely::lexer& lex, token_type ty)
{
    const char* token_start = lex.cursor;
    advance_char(lex);
    return {ty, token_start, 1};
}
}
} // namespace

ely::token ely::lexer::next()
{
    while (true)
    {
        char ch = peek_char(*this);
        switch (ch)
        {
        case '\0':
            __builtin_unreachable();
        case ';':
            scan_line_comment(*this, ch);
        case '\n':
        case ' ':
        case '\t':
        case '\v':
            continue;
        case '"':
            return scan_string_lit(*this, ch);
        case '(':
            return scan_single(*this, token_type::lparen);
        case ')':
            return scan_single(*this, token_type::rparen);
        case '[':
            return scan_single(*this, token_type::lbracket);
        case ']':
            return scan_single(*this, token_type::rbracket);
        case '{':
            return scan_single(*this, token_type::lbrace);
        case '}':
            return scan_single(*this, token_type::rbrace);
        case '-':
        case '+':
            return scan_sign(*this, ch);
        default:
            if (is_digit(ch))
            {
                return scan_number(*this, ch);
            }
            else if (is_identifier_start(ch))
            {
                return scan_identifier(*this, ch);
            }
            break;
        }
    }

    return scan_single(*this, token_type::unknown_char);
}

uint32_t ely::lexer::scan_tokens(token* dst, uint32_t dst_len)
{
    uint32_t i = 0;

    if (dst_len == 0 || *cursor == '\0')
    {
        return 0;
    }

    do
    {
        auto tok = next();
        dst[i]   = tok;
        ++i;
    } while (*cursor != '\0');

    return i;
}