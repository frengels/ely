#pragma once

#include <cassert>
#include <cstdint>
#include <iterator>
#include <ranges>

#include "ely/export.h"
#include "ely/lex/token.hpp"
#include "ely/lex/traits.hpp"

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
template<typename V>
class lexer
{
    using base_iter = std::ranges::iterator_t<V>;
    using char_type = std::iter_value_t<base_iter>;

public:
    class iterator;
    using sentinel = std::default_sentinel_t;

public:
    [[no_unique_address]] V base_;

public:
    lexer() = default;

    constexpr lexer(V base) : base_(std::move(base))
    {}

    constexpr V base() const&
    {
        return base_;
    }

    constexpr V base() &&
    {
        return std::move(base_);
    }

    constexpr bool empty() const
    {
        return base_.empty();
    }

    constexpr iterator begin() const
    {
        return iterator{base_.begin(), base_.end()};
    }

    constexpr sentinel end() const
    {
        return std::default_sentinel;
    }
};

template<std::ranges::range R>
lexer(R&&) -> lexer<std::views::all_t<R>>;

template<typename V>
class lexer<V>::iterator
{
    using base_iter = std::ranges::iterator_t<V>;
    using base_sent = std::ranges::sentinel_t<V>;

    using char_type = std::iter_value_t<base_iter>;

public:
    using value_type        = ely::token<V>;
    using reference         = value_type;
    using pointer           = void;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::input_iterator_tag;

private:
    [[no_unique_address]] base_iter it_;
    [[no_unique_address]] base_sent end_;

public:
    iterator() = default;

    constexpr iterator(base_iter it, base_sent end) : it_(it), end_(end)
    {}

    friend bool operator==(const iterator& lhs, const iterator& rhs)  = default;
    friend auto operator<=>(const iterator& lhs, const iterator& rhs) = default;

    friend constexpr bool operator==(const iterator& it,
                                     const std::default_sentinel_t&)
    {
        return it.it_ == it.end_;
    }

    friend constexpr bool operator==(const std::default_sentinel_t& end,
                                     const iterator&                it)
    {
        return it == end;
    }

    constexpr ely::token<V> operator*()
    {
        while (true)
        {
            char_type ch = peek_char();

            switch (ch)
            {
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
            case '[':
                return scan_single(token_type::lbracket);
            case ']':
                return scan_single(token_type::rbracket);
            case '{':
                return scan_single(token_type::lbrace);
            case '}':
                return scan_single(token_type::rbrace);
            case '+':
            case '-':
                return scan_sign(ch);
            default:
                if (detail::is_identifier_start(ch))
                {
                    return scan_identifier(ch);
                }
                else if (detail::is_digit(ch))
                {
                    return scan_number(ch);
                }
                return scan_single(token_type::unknown_char);
            }
        }
    }

    constexpr iterator& operator++()
    {
        return *this;
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

    constexpr token<V> scan_decimal(char_type ch, base_iter token_start)
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
                static_cast<std::size_t>(std::distance(token_start, it_))};
    }

    constexpr token<V> scan_number_cont(char_type ch, base_iter token_start)
    {
        assert(detail::is_digit(ch));
        advance_char();

        ch = peek_char();
        while (detail::is_digit(ch))
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
                static_cast<std::size_t>(std::distance(token_start, it_))};
    }

    constexpr token<V> scan_number(char_type ch)
    {
        return scan_number_cont(ch, it_);
    }

    constexpr token<V> scan_identifier_cont(char_type ch, base_iter token_start)
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
                static_cast<std::size_t>(std::distance(token_start, it_))};
    }

    constexpr token<V> scan_sign(char_type ch)
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
                    static_cast<std::size_t>(std::distance(token_start, it_))};
        }
    }

    constexpr token<V> scan_identifier(char_type ch)
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
                static_cast<std::size_t>(std::distance(token_start, it_))};
    }

    constexpr token<V> scan_string_lit(char_type ch)
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
                return {
                    token_type::unterminated_string_literal,
                    token_start,
                    static_cast<std::size_t>(std::distance(token_start, it_))};
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
                static_cast<std::size_t>(std::distance(token_start, it_))};
    }

    constexpr token<V> scan_single(token_type kind)
    {
        auto start = it_;
        advance_char();
        return {kind, start, 1};
    }
};

template<typename I, typename S, typename OutIt>
constexpr OutIt copy(I it, S end, OutIt dst)
{
    for (; it != end; (void) ++it, ++dst)
    {
        *dst = *it;
    }

    return dst;
}
} // namespace ely
