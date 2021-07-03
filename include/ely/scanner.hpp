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
template<typename T, typename = void>
struct is_lexeme : std::false_type
{};

template<typename T>
struct is_lexeme<T, std::void_t<typename T::lexeme_tag>> : std::true_type
{};

template<typename T>
inline constexpr bool is_lexeme_v = is_lexeme<T>::value;

template<typename L, typename = void>
struct is_newline : std::false_type
{};

template<typename L>
struct is_newline<L, std::void_t<typename L::newline_tag>> : is_lexeme<L>
{};

template<typename L>
inline constexpr bool is_newline_v = is_newline<L>::value;

template<typename L, typename = void>
struct is_atmosphere : std::false_type
{};

template<typename L>
struct is_atmosphere<L, std::void_t<typename L::atmosphere_tag>> : is_lexeme<L>
{};

template<typename L>
inline constexpr bool is_atmosphere_v = is_atmosphere<L>::value;

template<typename L, typename = void>
struct is_trailing_atmosphere : std::false_type
{};

template<typename L>
struct is_trailing_atmosphere<L,
                              std::void_t<typename L::trailing_atmosphere_tag>>
    : std::true_type
{};

template<typename L>
inline constexpr bool is_trailing_atmosphere_v =
    is_trailing_atmosphere<L>::value;

template<typename L, typename = void>
struct is_literal : std::false_type
{};

template<typename L>
struct is_literal<L, std::void_t<typename L::literal_tag>> : is_lexeme<L>
{};

template<typename L>
inline constexpr bool is_literal_v = is_literal<L>::value;

template<typename L, typename = void>
struct is_identifier : std::false_type
{};

template<typename L>
struct is_identifier<L, std::void_t<typename L::identifier_tag>> : is_lexeme<L>
{};

template<typename L>
inline constexpr bool is_identifier_v = is_identifier<L>::value;

template<typename L, typename = void>
struct is_eof : std::false_type
{};

template<typename L>
struct is_eof<L, std::void_t<typename L::eof_tag>> : is_lexeme<L>
{};

template<typename L>
inline constexpr bool is_eof_v = is_eof<L>::value;

struct Whitespace
{
    using lexeme_tag              = void;
    using atmosphere_tag          = void;
    using trailing_atmosphere_tag = void;
};
struct Tab
{
    using lexeme_tag              = void;
    using atmosphere_tag          = void;
    using trailing_atmosphere_tag = void;
};
struct NewlineCr
{
    using lexeme_tag     = void;
    using newline_tag    = void;
    using atmosphere_tag = void;
};
struct NewlineLf
{
    using lexeme_tag     = void;
    using newline_tag    = void;
    using atmosphere_tag = void;
};
struct NewlineCrlf
{
    using lexeme_tag     = void;
    using newline_tag    = void;
    using atmosphere_tag = void;
};
struct Comment
{
    using lexeme_tag              = void;
    using atmosphere_tag          = void;
    using trailing_atmosphere_tag = void;
};
struct LParen
{
    using lexeme_tag = void;
};
struct RParen
{
    using lexeme_tag = void;
};
struct LBracket
{
    using lexeme_tag = void;
};
struct RBracket
{
    using lexeme_tag = void;
};
struct LBrace
{
    using lexeme_tag = void;
};
struct RBrace
{
    using lexeme_tag = void;
};
struct Identifier
{
    using lexeme_tag     = void;
    using identifier_tag = void;
};
struct IntLit
{
    using lexeme_tag  = void;
    using literal_tag = void;
};
struct FloatLit
{
    using lexeme_tag  = void;
    using literal_tag = void;
};
struct CharLit
{
    using lexeme_tag  = void;
    using literal_tag = void;
};
struct StringLit
{
    using lexeme_tag  = void;
    using literal_tag = void;
};
struct KeywordLit
{
    using lexeme_tag  = void;
    using literal_tag = void;
};
struct BoolLit
{
    using lexeme_tag  = void;
    using literal_tag = void;
};
struct UnterminatedStringLit
{
    using lexeme_tag  = void;
    using literal_tag = void;
};
struct InvalidNumberSign
{
    using lexeme_tag = void;
};
struct Eof
{
    using lexeme_tag = void;
    using eof_tag    = void;
};
} // namespace lexeme

class LexemeKind : public ely::Variant<lexeme::Whitespace,
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
                                       lexeme::Eof>
{
    using base_ = ely::Variant<lexeme::Whitespace,
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

public:
    using base_::base_;

    ELY_ALWAYS_INLINE constexpr bool is_newline() const noexcept
    {
        return ely::visit(
            [](auto lex) { return lexeme::is_newline_v<decltype(lex)>; },
            *this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_atmosphere() const noexcept
    {
        return ely::visit(
            [](auto lex) { return lexeme::is_atmosphere_v<decltype(lex)>; },
            *this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_leading_atmosphere() const noexcept
    {
        return is_atmosphere();
    }

    ELY_ALWAYS_INLINE constexpr bool is_trailing_atmosphere() const noexcept
    {
        return ely::visit(
            [](auto lex) {
                return lexeme::is_trailing_atmosphere_v<decltype(lex)>;
            },
            *this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_literal() const noexcept
    {
        return ely::visit(
            [](auto lex) { return lexeme::is_literal_v<decltype(lex)>; },
            *this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_eof() const noexcept
    {
        return ely::visit(
            [](auto lex) { return lexeme::is_eof_v<decltype(lex)>; }, *this);
    }

    ELY_ALWAYS_INLINE constexpr bool is_identifier() const noexcept
    {
        return ely::visit(
            [](auto lex) { return lexeme::is_identifier_v<decltype(lex)>; },
            *this);
    }
};

static_assert(std::is_trivially_destructible_v<LexemeKind>);
static_assert(std::is_trivially_copy_constructible_v<LexemeKind>);
static_assert(
    std::is_default_constructible_v<LexemeKind>); // not trivial since index
                                                  // needs to be initialized too
static_assert(sizeof(LexemeKind) == 1);

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<lexeme::is_lexeme_v<Lex>, bool>
lexeme_is_newline(Lex lex)
{
    return lexeme::is_newline_v<Lex>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_newline(LexemeKind kind)
{
    return kind.is_newline();
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<lexeme::is_lexeme_v<Lex>, bool>
lexeme_is_trailing_atmosphere(Lex lex)
{
    return lexeme::is_trailing_atmosphere_v<Lex>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_trailing_atmosphere(LexemeKind kind)
{
    return kind.is_trailing_atmosphere();
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<lexeme::is_lexeme_v<Lex>, bool>
lexeme_is_atmosphere(Lex lex)
{
    return lexeme::is_atmosphere_v<Lex>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_atmosphere(LexemeKind kind)
{
    return kind.is_atmosphere();
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<!std::is_same_v<Lex, LexemeKind>,
                                             bool>
lexeme_is_leading_atmosphere(Lex lex)
{
    return lexeme::is_atmosphere_v<Lex>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_leading_atmosphere(LexemeKind kind)
{
    return kind.is_leading_atmosphere();
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<lexeme::is_lexeme_v<Lex>, bool>
lexeme_is_literal(Lex lex)
{
    return lexeme::is_literal_v<Lex>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_literal(LexemeKind kind)
{
    return kind.is_literal();
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<lexeme::is_lexeme_v<Lex>, bool>
lexeme_is_eof(Lex lex)
{
    return lexeme::is_eof_v<Lex>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_eof(LexemeKind kind)
{
    return kind.is_eof();
}

template<typename Lex>
ELY_ALWAYS_INLINE constexpr std::enable_if_t<!std::is_same_v<Lex, LexemeKind>,
                                             bool>
lexeme_is_identifier(Lex lex)
{
    return lexeme::is_identifier_v<Lex>;
}

ELY_ALWAYS_INLINE constexpr bool lexeme_is_identifier(LexemeKind kind)
{
    return kind.is_identifier();
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
        return !kind.is_eof();
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
