#pragma once

#include <cstdint>
#include <iterator>
#include <utility>

#include "ely/assert.h"
#include "ely/defines.h"
#include "ely/token.h"

namespace ely
{
enum class LexemeKind : uint8_t
{
    Eof,

    Whitespace,
    Tab,
    NewlineCr,
    NewlineLf,
    NewlineCrlf,
    Comment,

    LParen,
    RParen,
    LBracket,
    RBracket,
    LBrace,
    RBrace,

    Identifier,

    IntLit,
    FloatLit,
    CharLit,
    StringLit,
    KeywordLit,
    BoolLit,

    Poison,
};

constexpr ElyTokenKind lexeme_kind_to_ctoken_kind(LexemeKind lex)
{
    switch (lex)
    {
    case LexemeKind::Eof:
        return ELY_TOKEN_EOF;
    case LexemeKind::Whitespace:
        return ELY_TOKEN_WHITESPACE;
    case LexemeKind::Tab:
        return ELY_TOKEN_TAB;
    case LexemeKind::NewlineCr:
        return ELY_TOKEN_NEWLINE_CR;
    case LexemeKind::NewlineLf:
        return ELY_TOKEN_NEWLINE_LF;
    case LexemeKind::NewlineCrlf:
        return ELY_TOKEN_NEWLINE_CRLF;
    case LexemeKind::Comment:
        return ELY_TOKEN_COMMENT;
    case LexemeKind::LParen:
        return ELY_TOKEN_LPAREN;
    case LexemeKind::RParen:
        return ELY_TOKEN_RPAREN;
    case LexemeKind::LBracket:
        return ELY_TOKEN_LBRACKET;
    case LexemeKind::RBracket:
        return ELY_TOKEN_RBRACKET;
    case LexemeKind::LBrace:
        return ELY_TOKEN_LBRACE;
    case LexemeKind::RBrace:
        return ELY_TOKEN_RBRACE;
    case LexemeKind::Identifier:
        return ELY_TOKEN_ID;
    case LexemeKind::IntLit:
        return ELY_TOKEN_INT_LIT;
    case LexemeKind::FloatLit:
        return ELY_TOKEN_FLOAT_LIT;
    case LexemeKind::CharLit:
        return ELY_TOKEN_CHAR_LIT;
    case LexemeKind::StringLit:
        return ELY_TOKEN_STRING_LIT;
    case LexemeKind::KeywordLit:
        return ELY_TOKEN_KEYWORD_LIT;
    case LexemeKind::BoolLit:
        return ELY_TOKEN_TRUE_LIT;
    case LexemeKind::Poison:
        ELY_UNIMPLEMENTED("there's no equivalent in the C code");
    default:
        __builtin_unreachable();
    }
}

constexpr bool lexeme_is_atmosphere(LexemeKind kind)
{
    switch (kind)
    {
    case LexemeKind::Whitespace:
    case LexemeKind::Tab:
    case LexemeKind::NewlineCr:
    case LexemeKind::NewlineLf:
    case LexemeKind::NewlineCrlf:
    case LexemeKind::Comment:
        return true;
    default:
        return false;
    }
}

constexpr bool lexeme_is_literal(LexemeKind kind)
{
    switch (kind)
    {
    case LexemeKind::IntLit:
    case LexemeKind::FloatLit:
    case LexemeKind::CharLit:
    case LexemeKind::StringLit:
    case LexemeKind::KeywordLit:
    case LexemeKind::BoolLit:
        return true;
    default:
        return false;
    }
}

constexpr bool lexeme_is_identifier(LexemeKind kind)
{
    return kind == LexemeKind::Identifier;
}

constexpr bool lexeme_is_eof(LexemeKind kind)
{
    return kind == LexemeKind::Eof;
}

template<typename I>
struct Lexeme
{
    I          start;
    uint32_t   len;
    LexemeKind kind;

    constexpr std::size_t size() const
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
constexpr ScanResult<I> scan_poisoned(I it, S end)
{
    I next = advance_to_delimiter(it, end);
    return {next, LexemeKind::Poison};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_identifier_continue(I it, S end)
{
    I next = advance_to_delimiter(it, end);
    return {next, LexemeKind::Identifier};
}

template<typename I, typename S>
constexpr ScanResult<I> scan_keyword(I it, S end)
{
    I new_it = advance_to_delimiter(it, end);

    return {new_it, LexemeKind::KeywordLit};
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
        return {it, LexemeKind::FloatLit};
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
        return {it, LexemeKind::IntLit};
    }
    else
    {
        ++it;
        ELY_MUSTTAIL return scan_identifier_continue(it, end);
    }
}

template<typename I, typename S>
constexpr ScanResult<I> scan_char(I it, S end)
{
    I new_it = advance_to_delimiter(it, end);
    return {new_it, LexemeKind::CharLit};
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
        return {it, LexemeKind::Identifier};
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

    return {it, LexemeKind::Whitespace};
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

    return {it, LexemeKind::Tab};
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
            return {it, LexemeKind::NewlineCrlf};
        }
    }

    return {it, LexemeKind::NewlineCr};
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

    return {it, LexemeKind::Comment};
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
                    ELY_MUSTTAIL return scan_poisoned(it, end);
                }
            }
            return {it, LexemeKind::BoolLit};
        case ':':
            ++it;
            ELY_MUSTTAIL return scan_keyword(it, end);
        case '\\':
            ++it;
            ELY_MUSTTAIL return scan_char(it, end);
        case '\'':
        case '`':
        case ',':
        default:
            ++it;
            ELY_MUSTTAIL return scan_poisoned(it, end);
        }
    }

    return {it, LexemeKind::Poison};
}
} // namespace detail

template<typename I, typename S>
constexpr detail::ScanResult<I> scan_lexeme(I it, S end) noexcept
{
    if (it == end)
    {
        return {it, LexemeKind::Eof};
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
        return {it, LexemeKind::NewlineLf};
    case ';':
        ELY_MUSTTAIL return detail::scan_line_comment(it, end);
    case '(':
        return {it, LexemeKind::LParen};
    case ')':
        return {it, LexemeKind::RParen};
    case '[':
        return {it, LexemeKind::LBracket};
    case ']':
        return {it, LexemeKind::RBracket};
    case '{':
        return {it, LexemeKind::LBrace};
    case '}':
        return {it, LexemeKind::RBrace};
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

template<typename I, typename S>
class ScannerIterator
{
public:
    using value_type        = Lexeme<I>;
    using reference         = value_type;
    using pointer           = void;
    using size_type         = std::size_t;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

private:
    [[no_unique_address]] I it_;
    [[no_unique_address]] S end_;
    // TODO: erase this for contiguous_iterator
    // need to keep one for continuous use of dereference operator
    detail::ScanResult<I> cached_lexeme_;

public:
    ScannerIterator() = default;

    constexpr ScannerIterator(I it, S end)
        : it_(std::move(it)), end_(std::move(end))
    {
        cache_next();
    }

    constexpr I base() const&
    {
        return it_;
    }

    constexpr I base() &&
    {
        return std::move(it_);
    }

    friend constexpr bool operator==(const ScannerIterator& lhs,
                                     const ScannerIterator& rhs) noexcept
    {
        return lhs.it_ == rhs.it_;
    }

    friend constexpr bool operator!=(const ScannerIterator& lhs,
                                     const ScannerIterator& rhs) noexcept
    {
        return lhs.it_ != rhs.it_;
    }

    constexpr ScannerIterator& operator++() noexcept
    {
        it_ = cached_lexeme_.end;
        cache_next();
        return *this;
    }

    constexpr ScannerIterator operator++(int) noexcept
    {
        ScannerIterator self{*this};
        ++*this;
        return self;
    }

    constexpr reference operator*() const noexcept
    {
        return {it_,
                static_cast<uint32_t>(cached_lexeme_.end - it_),
                cached_lexeme_.kind};
    }

    constexpr bool _at_end() const
    {
        return cached_lexeme_.kind == LexemeKind::Eof;
    }

private:
    constexpr void cache_next() noexcept
    {
        cached_lexeme_ = scan_lexeme(it_, end_);
    }
};

template<typename I, typename S>
class ScannerSentinel
{
public:
    ScannerSentinel() = default;

    friend constexpr bool operator==(const ScannerIterator<I, S>& it,
                                     const ScannerSentinel<I, S>&) noexcept
    {
        return it._at_end();
    }

    friend constexpr bool operator==(const ScannerSentinel<I, S>&,
                                     const ScannerIterator<I, S>& it) noexcept
    {
        return it._at_end();
    }
};

template<typename I, typename S = I>
class Scanner
{
public:
    using iterator = ScannerIterator<I, S>;
    using sentinel = ScannerSentinel<I, S>;

    using value_type = typename iterator::value_type;

private:
    [[no_unique_address]] I it_;
    [[no_unique_address]] S end_;

public:
    Scanner() = default;

    constexpr Scanner(I it, S end) : it_(std::move(it)), end_(std::move(end))
    {}

    constexpr iterator begin() const
    {
        return iterator{it_, end_};
    }

    constexpr sentinel end() const
    {
        return sentinel{};
    }
};
} // namespace ely
