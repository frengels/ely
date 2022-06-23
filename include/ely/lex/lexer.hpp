#pragma once

#include <cassert>
#include <cstdint>
#include <iterator>

#include <llvm/Support/SMLoc.h>

#include "ely/export.h"
#include "ely/lex/token.hpp"

namespace ely
{
namespace detail
{
template<typename CharT>
constexpr bool is_digit(CharT ch)
{
    return '0' <= ch && ch <= '9';
}

template<typename CharT>
constexpr bool is_lower_alpha(CharT ch)
{
    return 'a' <= ch && ch <= 'z';
}

template<typename CharT>
constexpr bool is_upper_alpha(CharT ch)
{
    return 'A' <= ch && ch <= 'Z';
}

template<typename CharT>
constexpr bool is_alpha(CharT ch)
{
    return is_lower_alpha(ch) || is_upper_alpha(ch);
}

template<typename CharT>
constexpr bool is_identifier_start(CharT ch)
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

template<typename CharT>
constexpr bool is_identifier_continue(CharT ch)
{
    return is_identifier_start(ch) || is_digit(ch);
}
} // namespace detail
template<typename I, typename S>
class ELY_EXPORT lexer
{
    using char_type = std::iter_value_t<I>;

public:
    [[no_unique_address]] I it_;
    [[no_unique_address]] S end_;

public:
    explicit constexpr lexer(const char* src) : cursor(src)
    {}

    llvm::SMLoc location() const
    {
        return llvm::SMLoc::getFromPointer(std::addressof(*it_));
    }

    constexpr bool empty() const
    {
        return it_ == end_;
    }

    constexpr ely::token<I> next()
    {
        while (true)
        {
            char_type ch = peek_char();

            switch (ch)
            {
            case '\0':
                return {token_type::eof, it_, 0};
            case ';':
                scan_line_comment(ch);
                break;
            case '\n':
            case ' ':
            case '\t':
            case '\v':
                advance_char();
                break;
            case '"':
                return scan_string_lit(ch);
            case '(':
                return scan_single(token_type::lparen);
            case ')':
                return scan_single(token_type::rparen);
            }
        }
    }

private:
    constexpr char_type peek_char() const
    {
        return *it_;
    }

    constexpr void advance_char()
    {
        ++it_;
    }

    constexpr char_type consume_char()
    {
        char_type res = peek_char();
        advance_char();
        return res;
    }

    constexpr void scan_line_comment(char_type ch)
    {
        assert(ch == ';');
        advance_char();

        ch = consume_char();

        while (true)
        {
            switch (ch)
            {
            case '\r':
                ch = peek_char();
                if (ch == '\n')
                {
                    advance_char();
                }
                [[fallthrough]];
            case '\n':
                [[fallthrough]];
            case '\0':
                return;
            }

            ch = consume_char();
        }
    }

    constexpr token<I> scan_decimal(char_type ch, I token_start)
    {
        assert(ch == '.');
        advance_char();

        ch = peek_char();

        while (detail::is_digit(ch))
        {
            advance_char();
            ch = peek_char();
        }

        return {token_type::decimal_literal,
                token_start,
                std::distance(token_start, it_)};
    }

    constexpr token<I> scan_number_cont(char_type ch, I token_start)
    {
        assert(is_digit(ch));
        advance_char();

        ch = peek_char();
        while (is_digit(ch))
        {
            advance_char();
            ch = peek_char();
        }

        if (ch == '.')
        {
            return scan_decimal(ch, token_start);
        }

        return {token_type::int_literal,
                token_start,
                std::distance(token_start, it_)};
    }

    constexpr token<I> scan_number(char_type ch)
    {
        return scan_number_cont(ch, it_);
    }

    constexpr token<I> scan_identifier_cont(char_type ch, I token_start)
    {
        assert(detail::is_identifier_continue(ch));
        advance_char();

        ch = peek_char();
        while (detail::is_identifier_continue(ch))
        {
            advance_char();
            ch = peek_char();
        }

        return {token_type::identifier,
                token_start,
                std::distance(token_start, it_)};
    }

    constexpr token<I> scan_sign(char_type ch)
    {
        assert(ch == '+' || ch == '-');
        const auto token_start = it_;
        advance_char();

        ch = peek_char();
        if (detail::is_digit(ch))
        {
            return scan_number_cont(ch, token_start);
        }
        else if (detail::is_identifier_continue(ch))
        {
            return scan_identifier_cont(ch, token_start);
        }
        else
        {
            return {token_type::identifier,
                    token_start,
                    std::distance(token_start, it_)};
        }
    }

    constexpr token<I> scan_identifier(char_type ch)
    {
        assert(detail::is_identifier_start(ch));
        const auto token_start = it_;
        advance_char();

        ch = peek_char();

        if (detail::is_identifier_continue(ch))
        {
            return scan_identifier_cont(ch, token_start);
        }

        return {token_type::identifier,
                token_start,
                std::distance(token_start, it_)};
    }

    constexpr token<I> scan_string_lit(char_type ch)
    {
        assert(ch == '"');
        const auto token_start = it_;
        advance_char();

        bool escaping = false;
        ch            = peek_char();

        while (escaping || ch != '"')
        {
            if (ch == '\0')
            {
                return {token_type::unterminated_string_literal,
                        token_start,
                        std::distance(token_start, lex.it_)};
            }

            if (ch == '\\')
            {
                escaping = !escaping;
            }
            else
            {
                escaping = false;
            }

            advance_char();
            ch = peek_char();
        }

        advance_char();
        return {token_type::string_literal,
                token_start,
                std::distance(token_start, it_)};
    }

    constexpr token<I> scan_single(token_type kind)
    {
        auto start = it_;
        advance_char();
        return {ty, start, 1};
    }
};
} // namespace ely
