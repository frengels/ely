#pragma once

#include <cstdint>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

#include "ely/assert.h"
#include "ely/defines.h"
#include "ely/variant.hpp"

namespace ely
{
template<typename I>
class LexemeSpan;

namespace lexeme
{
using size_type = std::size_t;

struct Whitespace
{};

struct Tab
{};

struct NewlineCr
{
    NewlineCr() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr NewlineCr(NewlineCr,
                                          const LexemeSpan<I>&) noexcept
        : NewlineCr{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'\r'};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct NewlineLf
{
    NewlineLf() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr NewlineLf(NewlineLf,
                                          const LexemeSpan<I>&) noexcept
        : NewlineLf{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'\n'};

        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct NewlineCrlf
{
    NewlineCrlf() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr NewlineCrlf(NewlineCrlf,
                                            const LexemeSpan<I>&) noexcept
        : NewlineCrlf{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 2>{'\r', '\n'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct Comment
{};

struct LParen
{
    LParen() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr LParen(LParen, const LexemeSpan<I>&) noexcept
        : LParen{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'('};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct RParen
{
    RParen() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr RParen(RParen, const LexemeSpan<I>&) noexcept
        : RParen{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{')'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct LBracket
{
    LBracket() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr LBracket(LBracket,
                                         const LexemeSpan<I>&) noexcept
        : LBracket{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'['};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct RBracket
{
    RBracket() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr RBracket(LBracket,
                                         const LexemeSpan<I>&) noexcept
        : RBracket{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{']'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct LBrace
{
    LBrace() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr LBrace(LBrace, const LexemeSpan<I>&) noexcept
        : LBrace{}
    {}

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'{'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct RBrace
{
    RBrace() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr RBrace(RBrace, const LexemeSpan<I>&) noexcept
        : RBrace{}
    {}

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'}'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

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

struct Colon
{
    Colon() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Colon(Colon, const LexemeSpan<I>&) noexcept
        : Colon{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{':'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct Quote
{
    Quote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Quote(Quote, const LexemeSpan<I>&) noexcept
        : Quote{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'\''};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
}

struct SyntaxQuote
{
    SyntaxQuote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr SyntaxQuote(SyntaxQuote,
                                            const LexemeSpan<I>&) noexcept
        : SyntaxQuote{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 2>{'#', '\''};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct At
{
    At() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr At(At, const LexemeSpan<I>&) noexcept : At{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'@'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct Unquote
{
    Unquote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Unquote(Unquote, const LexemeSpan<I>&) noexcept
        : Unquote{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{','};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct SyntaxUnquote
{
    SyntaxUnquote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr SyntaxUnquote(SyntaxUnquote,
                                              const LexemeSpan<I>&) noexcept
        : SyntaxUnquote{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 2>{'#', ','};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct UnquoteSplicing
{
    UnquoteSplicing() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr UnquoteSplicing(UnquoteSplicing,
                                                const LexemeSpan<I>&) noexcept
        : UnquoteSplicing{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 2>{',', '@'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct SyntaxUnquoteSplicing
{
    SyntaxUnquoteSplicing() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr SyntaxUnquoteSplicing(
        SyntaxUnquoteSplicing,
        const LexemeSpan<I>&) noexcept
        : SyntaxUnquoteSplicing{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 3>{'#', ',', '@'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct Exclamation
{
    Exclamation() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Exclamation(Exclamation,
                                            const LexemeSpan<I>&) noexcept
        : Exclamation{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'!'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct Question
{
    Question() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Question(Question,
                                         const LexemeSpan<I>&) noexcept
        : Question{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'?'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct Asterisk
{
    Asterisk() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Asterisk(Asterisk,
                                         const LexemeSpan<I>&) noexcept
        : Asterisk{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'*'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct QuasiQuote
{
    QuasiQuote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr QuasiQuote(QuasiQuote,
                                           const LexemeSpan<I>&) noexcept
        : QuasiQuote{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'`'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct QuasiSyntax
{
    QuasiSyntax() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr QuasiSyntax(QuasiSyntax,
                                            const LexemeSpan<I>&) noexcept
        : QuasiSyntax{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 2>{'#', '`'};
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

struct UnterminatedStringLit
{};

struct InvalidNumberSign
{};

struct Eof
{
    Eof() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Eof(Eof, const LexemeSpan<I>&) noexcept : Eof{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

using variant_type = ely::Variant<lexeme::Whitespace,
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
                                  lexeme::Colon,
                                  lexeme::Quote,
                                  lexeme::SyntaxQuote,
                                  lexeme::At,
                                  lexeme::Unquote,
                                  lexeme::SyntaxUnquote,
                                  lexeme::UnquoteSplicing,
                                  lexeme::SyntaxUnquoteSplicing,
                                  lexeme::Exclamation,
                                  lexeme::Question,
                                  lexeme::Asterisk,
                                  lexeme::QuasiQuote,
                                  lexeme::QuasiSyntax,
                                  lexeme::UnterminatedStringLit,
                                  lexeme::InvalidNumberSign,
                                  lexeme::Eof>;
} // namespace lexeme

class LexemeKind : public lexeme::variant_type
{
    using base_ = lexeme::variant_type;

public:
    using base_::base_;
};

static_assert(std::is_trivially_destructible_v<LexemeKind>);
static_assert(std::is_trivially_copy_constructible_v<LexemeKind>);
static_assert(
    std::is_default_constructible_v<LexemeKind>); // not trivial since index
                                                  // needs to be initialized too
static_assert(sizeof(LexemeKind) == 1);

namespace detail
{
template<typename T, typename... Ts>
inline constexpr bool is_same_as_one_of_v = (std::is_same_v<T, Ts> || ...);
}

/// provides the span for the lexeme, in the case of contiguous iterators we can
/// elide the length since it's just end - start. With non contiguous iterators
/// we don't do this since the minus operation could be expensive.
template<typename I,
         bool ElideLen = detail::is_same_as_one_of_v<
                             I,
                             typename std::string_view::iterator,
                             typename std::string::iterator,
                             typename std::vector<char>::iterator> ||
                         std::is_pointer_v<I>>
class LexemeSpanBase;

template<typename I>
class LexemeSpanBase<I, true>
{
public:
    using iterator  = I;
    using size_type = std::make_unsigned_t<
        typename std::iterator_traits<iterator>::difference_type>;

private:
    iterator start_;
    iterator end_;

public:
    LexemeSpanBase() = default;

    ELY_ALWAYS_INLINE constexpr LexemeSpanBase(iterator  start,
                                               iterator  end,
                                               size_type len) noexcept
        : start_(std::move(start)), end_(std::move(end))
    {
        ELY_ASSERT(std::distance(start_, end_) == len,
                   "The distance between the iterators does not equal the "
                   "given length");
    }

    ELY_ALWAYS_INLINE constexpr iterator begin() const noexcept
    {
        return start_;
    }

    ELY_ALWAYS_INLINE constexpr iterator end() const noexcept
    {
        return end_;
    }

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return static_cast<size_type>(std::distance(begin(), end()));
    }
};

template<typename I>
class LexemeSpanBase<I, false>
{
public:
    using iterator  = I;
    using size_type = std::make_unsigned_t<
        typename std::iterator_traits<iterator>::difference_type>;

private:
    iterator  start_;
    iterator  end_;
    size_type size_;

public:
    LexemeSpanBase() = default;

    ELY_ALWAYS_INLINE constexpr LexemeSpanBase(iterator  start,
                                               iterator  end,
                                               size_type sz) noexcept
        : start_(std::move(start)), end_(std::move(end)), size_(sz)
    {}

    ELY_ALWAYS_INLINE constexpr iterator begin() const noexcept
    {
        return start_;
    }

    ELY_ALWAYS_INLINE constexpr iterator end() const noexcept
    {
        return end_;
    }

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return size_;
    }
};

template<typename I>
class LexemeSpan : public LexemeSpanBase<I>
{
private:
    using base_ = LexemeSpanBase<I>;

public:
    using base_::base_;

    using base_::begin;
    using base_::end;
    using base_::size;
};

template<typename I>
struct Lexeme
{
public:
    using iterator  = I;
    using size_type = uint32_t;

public:
    LexemeSpan span;
    LexemeKind kind{lexeme::Eof{}};

    explicit constexpr operator bool() const noexcept
    {
        return !kind.is_eof();
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
    case ':':
        return {it, lexeme::Colon{}};
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
            it_, static_cast<uint32_t>(scan_res.end - it_), scan_res.kind};
        it_ = scan_res.end;
        return res;
    }
};
} // namespace ely
