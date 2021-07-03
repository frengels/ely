#pragma once

#include <cstdint>
#include <iterator>
#include <utility>

#include "ely/assert.h"
#include "ely/defines.h"
#include "ely/variant.hpp"

namespace ely
{
namespace lexeme
{
struct Whitespace
{};
struct Tab
{};
struct NewlineCr
{};
struct NewlineLf
{};
struct NewlineCrlf
{};
struct Comment
{};
struct LParen
{};
struct RParen
{};
struct LBracket
{};
struct RBracket
{};
struct LBrace
{};
struct RBrace
{};
struct Identifier
{};
struct IntLit
{};
struct FloatLit
{};
struct CharLit
{};
struct StringLit
{};
struct KeywordLit
{};
struct BoolLit
{};
struct UnterminatedStringLit
{};
struct InvalidNumberSign
{};
struct Eof
{};
} // namespace lexeme

using LexemeKind = ely::Variant<lexeme::Whitespace,
                                lexeme::Tab,
                                lexeme::NewlineCr,
                                lexeme::NewlineLf,
                                lexeme::NewlineCrlf,
                                lexeme::Comment,
                                lexeme::LParen,
                                lexeme::RParen,
                                lexeme::LBracket,
                                lexeme::RBracket,
                                lexeme::LBrace,
                                lexeme::RBrace,
                                lexeme::Identifier,
                                lexeme::IntLit,
                                lexeme::FloatLit,
                                lexeme::CharLit,
                                lexeme::StringLit,
                                lexeme::KeywordLit,
                                lexeme::BoolLit,
                                lexeme::UnterminatedStringLit,
                                lexeme::InvalidNumberSign,
                                lexeme::Eof>;

static_assert(std::is_trivially_destructible_v<LexemeKind>);
static_assert(std::is_trivially_copy_constructible_v<LexemeKind>);
static_assert(
    std::is_default_constructible_v<LexemeKind>); // not trivial since index
                                                  // needs to be initialized too
static_assert(sizeof(LexemeKind) == 1);

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<!std::is_same_v<Lex, LexemeKind>,
                                             bool>
lexeme_is_newline(Lex lex)
{
    return std::is_same_v<Lex, lexeme::NewlineCr> ||
           std::is_same_v<Lex, lexeme::NewlineLf> ||
           std::is_same_v<Lex, lexeme::NewlineCrlf>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_newline(LexemeKind kind)
{
    using ely::visit;
    return visit([](auto lex) { return lexeme_is_newline(lex); }, kind);
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<!std::is_same_v<Lex, LexemeKind>,
                                             bool>
lexeme_is_trailing_atmosphere(Lex lex)
{
    return std::is_same_v<Lex, lexeme::Whitespace> ||
           std::is_same_v<Lex, lexeme::Tab> ||
           std::is_same_v<Lex, lexeme::Comment>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_trailing_atmosphere(LexemeKind kind)
{
    using ely::visit;
    return visit([](auto lex) { return lexeme_is_trailing_atmosphere(lex); },
                 kind);
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<!std::is_same_v<Lex, LexemeKind>,
                                             bool>
lexeme_is_atmosphere(Lex lex)
{
    return lexeme_is_trailing_atmosphere(lex) || lexeme_is_newline(lex);
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_atmosphere(LexemeKind kind)
{
    using ely::visit;
    return visit([](auto lex) { return lexeme_is_atmosphere(lex); }, kind);
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<!std::is_same_v<Lex, LexemeKind>,
                                             bool>
lexeme_is_leading_atmosphere(Lex lex)
{
    ELY_MUSTTAIL return lexeme_is_atmosphere(lex);
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_leading_atmosphere(LexemeKind kind)
{
    using ely::visit;
    return visit([](auto lex) { return lexeme_is_leading_atmosphere(lex); },
                 kind);
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<!std::is_same_v<Lex, LexemeKind>,
                                             bool>
lexeme_is_literal(Lex lex)
{
    return std::is_same_v<Lex, lexeme::IntLit> ||
           std::is_same_v<Lex, lexeme::FloatLit> ||
           std::is_same_v<Lex, lexeme::CharLit> ||
           std::is_same_v<Lex, lexeme::StringLit> ||
           std::is_same_v<Lex, lexeme::KeywordLit> ||
           std::is_same_v<Lex, lexeme::BoolLit>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_literal(LexemeKind kind)
{
    using ely::visit;
    return visit([](auto lex) { return lexeme_is_literal(lex); }, kind);
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<!std::is_same_v<Lex, LexemeKind>,
                                             bool>
lexeme_is_eof(Lex lex)
{
    return std::is_same_v<Lex, lexeme::Eof>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_eof(LexemeKind kind)
{
    using ely::visit;
    return visit([](auto lex) { return lexeme_is_eof(lex); }, kind);
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<!std::is_same_v<Lex, LexemeKind>,
                                             bool>
lexeme_is_identifier(Lex lex)
{
    return std::is_same_v<Lex, lexeme::Identifier>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_identifier(LexemeKind kind)
{
    using ely::visit;
    return visit([](auto lex) { return lexeme_is_identifier(lex); }, kind);
}

template<typename I>
struct Lexeme
{
public:
    using iterator  = I;
    using size_type = uint32_t;

public:
    I          start{};
    uint32_t   len{};
    LexemeKind kind{lexeme::Eof{}};

    explicit constexpr operator bool() const noexcept
    {
        return !lexeme_is_eof(kind);
    }

    constexpr iterator begin() const
    {
        return start;
    }

    constexpr iterator end() const
    {
        return std::next(
            start,
            static_cast<typename std::iterator_traits<I>::difference_type>(
                len));
    }

    constexpr std::size_t size() const noexcept
    {
        return static_cast<std::size_t>(len);
    }
};

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
    return {next, lexeme::Identifier{}};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_keyword(I it, S end)
{
    I next = advance_to_delimiter(it, end);
    return {next, lexeme::KeywordLit{}};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_invalid_number_sign(I it, S end)
{
    I next = advance_to_delimiter(it, end);
    return {next, lexeme::InvalidNumberSign{}};
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
        return {it, lexeme::FloatLit{}};
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
        return {it, lexeme::IntLit{}};
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
            return {it, lexeme::StringLit{}};
        }
    }

    return {it, lexeme::UnterminatedStringLit{}};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_char(I it, S end)
{
    I new_it = advance_to_delimiter(it, end);
    return {new_it, lexeme::CharLit{}};
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
        return {it, lexeme::Identifier{}};
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

    return {it, lexeme::Whitespace{}};
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

    return {it, lexeme::Tab{}};
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
            return {it, lexeme::NewlineCrlf{}};
        }
    }

    return {it, lexeme::NewlineCr{}};
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

    return {it, lexeme::Comment{}};
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
            return {it, lexeme::BoolLit{}};
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

    return {it, lexeme::InvalidNumberSign{}};
}

template<typename I, typename S>
constexpr detail::ScanResult<I> scan_lexeme(I it, S end) noexcept
{
    if (it == end)
    {
        return {it, lexeme::Eof{}};
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
        return {it, lexeme::NewlineLf{}};
    case ';':
        ELY_MUSTTAIL return detail::scan_line_comment(it, end);
    case '(':
        return {it, lexeme::LParen{}};
    case ')':
        return {it, lexeme::RParen{}};
    case '[':
        return {it, lexeme::LBracket{}};
    case ']':
        return {it, lexeme::RBracket{}};
    case '{':
        return {it, lexeme::LBrace{}};
    case '}':
        return {it, lexeme::RBrace{}};
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
    [[no_unique_address]] I it_;
    [[no_unique_address]] S end_;

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
            it_, static_cast<uint32_t>(scan_res.end - it_), scan_res.kind};
        it_ = scan_res.end;
        return res;
    }
};
} // namespace ely
