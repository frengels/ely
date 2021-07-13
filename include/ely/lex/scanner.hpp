#pragma once

#include "ely/defines.h"
#include "ely/lex/lexeme.hpp"
#include "ely/lex/lexemes.hpp"

namespace ely
{
namespace detail
{
constexpr bool is_newline_start(char c)
{
    switch (c)
    {
    case '\r':
    case '\n':
        return true;
    default:
        return false;
    }
}

constexpr bool is_number(char c)
{
    return '0' <= c && c <= '9';
}

constexpr bool is_atmosphere_start(char c)
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

constexpr bool is_delimiter(char c)
{
    switch (c)
    {
    case '(':
    case ')':
    case '{':
    case '}':
    case '[':
    case ']':
    case '"':
    case ':':
        return true;
    default:
        ELY_MUSTTAIL return is_atmosphere_start(c);
    }
}

template<typename I, typename S>
constexpr I advance_to_delimiter(I it, S end)
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

template<typename I>
struct ScanResult
{
    I          end;
    LexemeKind kind;
};

template<typename I, typename S>
constexpr ScanResult<I> scan_identifier_continue(I it, S end)
{
    I next = advance_to_delimiter(it, end);
    return {next, std::in_place_type<token2::Identifier>};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_keyword(I it, S end)
{
    I next = advance_to_delimiter(it, end);
    return {next, std::in_place_type<token2::KeywordLit>};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_invalid_number_sign(I it, S end)
{
    I next = advance_to_delimiter(it, end);
    return {next, std::in_place_type<token2::InvalidNumberSign>};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_float(I it, S end)
{
    char c = *it;

    while (is_number(c))
    {
        ++it;
        c = *it;
    }

    if (is_delimiter(c))
    {
        return {it, std::in_place_type<token2::FloatLit>};
    }
    else
    {
        ++it;
        ELY_MUSTTAIL return scan_identifier_continue(it, end);
    }
}

template<typename I, typename S>
constexpr ScanResult<I> scan_number_continue(I it, S end)
{
    char c = *it;

    while (is_number(c))
    {
        ++it;
        c = *it;
    }

    // hit a non number character;
    if (c == '.')
    {
        ++it;
        // lexing a float now
        ELY_MUSTTAIL return scan_float(it, end);
    }
    else if (is_delimiter(c))
    {
        return {it, std::in_place_type<token2::IntLit>};
    }
    else
    {
        ++it;
        ELY_MUSTTAIL return scan_identifier_continue(it, end);
    }
}

template<typename I, typename S>
constexpr ScanResult<I> scan_string(I it, S end)
{
    bool escaping = false;

    while (it != end)
    {
        char ch = *it++;

        if (ch == '\\')
        {
            escaping = !escaping;
        }
        else if (ch == '"' && !escaping)
        {
            return {it, std::in_place_type<token2::StringLit>};
        }
    }

    return {it, std::in_place_type<token2::UnterminatedStringLit>};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_char(I it, S end)
{
    I new_it = advance_to_delimiter(it, end);
    return {new_it, std::in_place_type<token2::CharLit>};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_sign(I it, S end)
{
    char ch = *it;

    if (is_number(ch))
    {
        ++it;
        ELY_MUSTTAIL return scan_number_continue(it, end);
    }
    else if (is_delimiter(ch))
    {
        return {it, std::in_place_type<token2::Identifier>};
    }
    else
    {
        ++it;
        ELY_MUSTTAIL return scan_identifier_continue(it, end);
    }
}

template<typename I, typename S>
constexpr ScanResult<I> scan_whitespace(I it, S end)
{
    while (it != end)
    {
        char ch = *it;
        if (ch == ' ')
        {
            ++it;
        }
        else
        {
            break;
        }
    }

    return {it, std::in_place_type<token2::Whitespace>};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_tab(I it, S end)
{
    while (it != end)
    {
        char ch = *it;
        if (ch == '\t')
        {
            ++it;
        }
        else
        {
            break;
        }
    }

    return {it, std::in_place_type<token2::Tab>};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_cr(I it, S end)
{
    if (it != end)
    {
        char ch = *it;
        if (ch == '\n')
        {
            ++it;
            return {it, std::in_place_type<token2::NewlineCrlf>};
        }
    }

    return {it, std::in_place_type<token2::NewlineCr>};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_line_comment(I it, S end)
{
    for (; it != end; ++it)
    {
        char ch = *it;
        if (is_newline_start(ch))
        {
            break;
        }
    }

    return {it, std::in_place_type<token2::Comment>};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_number_sign(I it, S end)
{
    if (it != end)
    {
        char ch = *it;

        switch (ch)
        {
        case 'f':
        case 't':
            ++it;
            if (it != end)
            {
                ch = *it;
                if (!is_delimiter(ch))
                {
                    ++it;
                    ELY_MUSTTAIL return scan_invalid_number_sign(it, end);
                }
            }
            return {it, std::in_place_type<token2::BoolLit>};
        case ':':
            ++it;
            ELY_MUSTTAIL return scan_keyword(it, end);
        case '\\':
            ++it;
            ELY_MUSTTAIL return scan_char(it, end);
        default:
            ++it;
            ELY_MUSTTAIL return scan_invalid_number_sign(it, end);
        }
    }

    return {it, std::in_place_type<token2::InvalidNumberSign>};
}

template<typename I, typename S>
constexpr detail::ScanResult<I> scan_lexeme(I it, S end) noexcept
{
    if (it == end)
    {
        return {it, std::in_place_type<token2::Eof>};
    }

    char ch = *it++;

    switch (ch)
    {
    case ' ':
        ELY_MUSTTAIL return detail::scan_whitespace(it, end);
    case '\t':
        ELY_MUSTTAIL return detail::scan_tab(it, end);
    case '\r':
        ELY_MUSTTAIL return detail::scan_cr(it, end);
    case '\n':
        return {it, std::in_place_type<token2::NewlineLf>};
    case ';':
        ELY_MUSTTAIL return detail::scan_line_comment(it, end);
    case '(':
        return {it, std::in_place_type<token2::LParen>};
    case ')':
        return {it, std::in_place_type<token2::RParen>};
    case '[':
        return {it, std::in_place_type<token2::LBracket>};
    case ']':
        return {it, std::in_place_type<token2::RBracket>};
    case '{':
        return {it, std::in_place_type<token2::LBrace>};
    case '}':
        return {it, std::in_place_type<token2::RBrace>};
    case ':':
        return {it, std::in_place_type<token2::Colon>};
    case '"':
        ELY_MUSTTAIL return detail::scan_string(it, end);
    case '+':
    case '-':
        ELY_MUSTTAIL return detail::scan_sign(it, end);
    case '#':
        ELY_MUSTTAIL return detail::scan_number_sign(it, end);
    default:
        if (detail::is_number(ch))
        {
            ELY_MUSTTAIL return detail::scan_number_continue(it, end);
        }
        else
        {
            ELY_MUSTTAIL return detail::scan_identifier_continue(it, end);
        }
    }
}
} // namespace detail

template<typename I, typename S>
class ScannerStream
{
public:
    using value_type = Lexeme<I>;
    using reference  = value_type;

private:
    I it_;
    S end_;

public:
    ScannerStream() = default;

    constexpr ScannerStream(I it, S end)
        : it_(std::move(it)), end_(std::move(end))
    {}

    constexpr I base_iterator() const
    {
        return it_;
    }

    constexpr S base_sentinel() const
    {
        return end_;
    }

    constexpr reference next() noexcept
    {
        detail::ScanResult<I> scan_res = detail::scan_lexeme(it_, end_);
        value_type            res{
            {it_, scan_res.end, static_cast<uint32_t>(scan_res.end - it_)},
            scan_res.kind};
        it_ = scan_res.end;
        return res;
    }
};

} // namespace ely