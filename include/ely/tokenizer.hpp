#pragma once

#include <type_traits>
#include <utility>

#include "ely/scanner.hpp"

namespace ely
{
enum class TokenKind : uint8_t
{
#define CAST(x) static_cast<std::underlying_type_t<LexemeKind>>(x)
    Eof = CAST(LexemeKind::Eof),

    // doesn't include whitespace
    LParen   = CAST(LexemeKind::LParen),
    RParen   = CAST(LexemeKind::RParen),
    LBracket = CAST(LexemeKind::LBracket),
    RBracket = CAST(LexemeKind::RBracket),
    LBrace   = CAST(LexemeKind::LBrace),
    RBrace   = CAST(LexemeKind::RBrace),

    Identifier = CAST(LexemeKind::Identifier),

    IntLit     = CAST(LexemeKind::IntLit),
    FloatLit   = CAST(LexemeKind::FloatLit),
    CharLit    = CAST(LexemeKind::CharLit),
    StringLit  = CAST(LexemeKind::StringLit),
    KeywordLit = CAST(LexemeKind::KeywordLit),
    BoolLit    = CAST(LexemeKind::BoolLit),

#undef CAST
};

enum class AtmosphereKind : uint8_t
{
#define CAST(x) static_cast<std::underlying_type_t<LexemeKind>>(x)
    Whitespace  = CAST(LexemeKind::Whitespace),
    Tab         = CAST(LexemeKind::Tab),
    NewlineCr   = CAST(LexemeKind::NewlineCr),
    NewlineLf   = CAST(LexemeKind::NewlineLf),
    NewlineCrlf = CAST(LexemeKind::NewlineCrlf),
    Comment     = CAST(LexemeKind::Comment),
#undef CAST
};

namespace atmosphere
{
class Whitespace
{
public:
    static constexpr AtmosphereKind enum_value = AtmosphereKind::Whitespace;

private:
    std::size_t len;

public:
    constexpr Whitespace(std::size_t len) : len(len)
    {}

    constexpr std::size_t size() const
    {
        return len;
    }
};

class Tab
{
public:
    static constexpr AtmosphereKind enum_value = AtmosphereKind::Tab;

private:
    std::size_t len;

public:
    constexpr Tab(std::size_t len) : len(len)
    {}

    constexpr std::size_t size() const
    {
        return len;
    }
};

class NewlineCr
{
public:
    static constexpr AtmosphereKind enum_value = AtmosphereKind::NewlineCr;

public:
    constexpr std::size_t size() const
    {
        return 1;
    }
};

class NewlineLf
{
public:
    static constexpr AtmosphereKind enum_value = AtmosphereKind::NewlineLf;

public:
    constexpr std::size_t size() const
    {
        return 1;
    }
};

class NewlineCrlf
{
public:
    static constexpr AtmosphereKind enum_value = AtmosphereKind::NewlineCrlf;

public:
    constexpr std::size_t size() const
    {
        return 2;
    }
};

class Comment
{
public:
    static constexpr AtmosphereKind enum_value = AtmosphereKind::Comment;

private:
    std::string str;

public:
    Comment() = default;

    template<typename... Args>
    constexpr Comment(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    constexpr std::size_t size() const
    {
        return 1 + str.size();
    }
};
} // namespace atmosphere

namespace token
{
class Eof
{
public:
    static constexpr TokenKind enum_value = TokenKind::Eof;
};

class LParen
{
public:
    static constexpr TokenKind enum_value = TokenKind::LParen;
};

class RParen
{
public:
    static constexpr TokenKind enum_value = TokenKind::RParen;
};

class LBracket
{
public:
    static constexpr TokenKind enum_value = TokenKind::LBracket;
};

class RBracket
{
public:
    static constexpr TokenKind enum_value = TokenKind::RBracket;
};

class LBrace
{
public:
    static constexpr TokenKind enum_value = TokenKind::LBrace;
};

class RBrace
{
public:
    static constexpr TokenKind enum_value = TokenKind::RBrace;
};

class Identifier
{
public:
    static constexpr TokenKind enum_value = TokenKind::Identifier;

private:
    std::string name;

public:
    Identifier() = default;

    template<typename... Args>
    constexpr Identifier(std::in_place_t, Args&&... args)
        : name(static_cast<Args&&>(args)...)
    {}
};

class IntLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::IntLit;

private:
    std::string str;

public:
    IntLit() = default;

    template<typename... Args>
    constexpr IntLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}
};

class FloatLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::FloatLit;

private:
    std::string str;

public:
    FloatLit() = default;

    template<typename... Args>
    constexpr FloatLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}
};

class CharLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::CharLit;

private:
    std::string str;

public:
    CharLit() = default;

    template<typename... Args>
    constexpr CharLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}
};

class StringLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::StringLit;

private:
    std::string str;

public:
    StringLit() = default;

    template<typename... Args>
    constexpr StringLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}
};

class KeywordLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::KeywordLit;

private:
    std::string str;

public:
    KeywordLit() = default;

    template<typename... Args>
    constexpr KeywordLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}
};

class BoolLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::BoolLit;

private:
    bool b;

public:
    BoolLit() = default;

    constexpr BoolLit(bool b) : b(b)
    {}

    constexpr bool value() const
    {
        return b;
    }
};
} // namespace token

namespace detail
{
union AtmosphereUnion
{
#define DEFINE_CONSTRUCTOR(variant, member)                                    \
    template<typename... Args>                                                 \
    explicit constexpr AtmosphereUnion(std::in_place_type_t<variant>,          \
                                       Args&&... args)                         \
        : member(static_cast<Args&&>(args)...)                                 \
    {}

    DEFINE_CONSTRUCTOR(atmosphere::Whitespace, whitespace)
    DEFINE_CONSTRUCTOR(atmosphere::Tab, tab)
    DEFINE_CONSTRUCTOR(atmosphere::NewlineCr, newline_cr)
    DEFINE_CONSTRUCTOR(atmosphere::NewlineLf, newline_lf)
    DEFINE_CONSTRUCTOR(atmosphere::NewlineCrlf, newline_crlf)
    DEFINE_CONSTRUCTOR(atmosphere::Comment, comment)
#undef DEFINE_CONSTRUCTOR

    ~AtmosphereUnion()
    {}

    atmosphere::Whitespace  whitespace;
    atmosphere::Tab         tab;
    atmosphere::NewlineCr   newline_cr;
    atmosphere::NewlineLf   newline_lf;
    atmosphere::NewlineCrlf newline_crlf;
    atmosphere::Comment     comment;
};

union TokenUnion
{
#define DEFINE_CONSTRUCTOR(variant, member)                                    \
    template<typename... Args>                                                 \
    explicit constexpr TokenUnion(std::in_place_type_t<variant>,               \
                                  Args&&... args)                              \
        : member(static_cast<Args&&>(args)...)                                 \
    {}

    DEFINE_CONSTRUCTOR(token::Eof, eof)
    DEFINE_CONSTRUCTOR(token::LParen, lparen)
    DEFINE_CONSTRUCTOR(token::RParen, rparen)
    DEFINE_CONSTRUCTOR(token::LBracket, lbracket)
    DEFINE_CONSTRUCTOR(token::RBracket, rbracket)
    DEFINE_CONSTRUCTOR(token::LBrace, lbrace)
    DEFINE_CONSTRUCTOR(token::RBrace, rbrace)
    DEFINE_CONSTRUCTOR(token::Identifier, identifier)
    DEFINE_CONSTRUCTOR(token::IntLit, int_lit)
    DEFINE_CONSTRUCTOR(token::FloatLit, float_lit)
    DEFINE_CONSTRUCTOR(token::CharLit, char_lit)
    DEFINE_CONSTRUCTOR(token::StringLit, string_lit)
    DEFINE_CONSTRUCTOR(token::KeywordLit, keyword_lit)
    DEFINE_CONSTRUCTOR(token::BoolLit, bool_lit)
#undef DEFINE_CONSTRUCTOR

    ~TokenUnion()
    {}

    token::Eof      eof;
    token::LParen   lparen;
    token::RParen   rparen;
    token::LBracket lbracket;
    token::RBracket rbracket;
    token::LBrace   lbrace;
    token::RBrace   rbrace;

    token::Identifier identifier;

    token::IntLit     int_lit;
    token::FloatLit   float_lit;
    token::CharLit    char_lit;
    token::StringLit  string_lit;
    token::KeywordLit keyword_lit;
    token::BoolLit    bool_lit;
};
} // namespace detail

class ELY_EXPORT Atmosphere
{
    AtmosphereKind          kind;
    detail::AtmosphereUnion values;

public:
    template<typename T, typename... Args>
    explicit constexpr Atmosphere(std::in_place_type_t<T>, Args&&... args)
        : kind(T::enum_value),
          values(std::in_place_type<T>, static_cast<Args&&>(args)...)
    {}

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
#define CALL(member) static_cast<F&&>(fn)(member)
        switch (kind)
        {
        case AtmosphereKind::Whitespace:
            return CALL(values.whitespace);
        case AtmosphereKind::Tab:
            return CALL(values.tab);
        case AtmosphereKind::NewlineCr:
            return CALL(values.newline_cr);
        case AtmosphereKind::NewlineLf:
            return CALL(values.newline_lf);
        case AtmosphereKind::NewlineCrlf:
            return CALL(values.newline_crlf);
        case AtmosphereKind::Comment:
            return CALL(values.comment);
        default:
            __builtin_unreachable();
        }
#undef CALL
    }

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return static_cast<const Atmosphere&>(*this).visit(
            [&](const auto& x) -> decltype(auto) {
                using ty =
                    std::remove_cv_t<std::remove_reference_t<decltype(x)>>;
                return static_cast<F&&>(fn)(const_cast<ty&>(x));
            });
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return visit([&](auto& x) -> decltype(auto) {
            return static_cast<F&&>(fn)(std::move(x));
        });
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return static_cast<const Atmosphere&>(*this).visit(
            [&](const auto& x) -> decltype(auto) {
                return static_cast<F&&>(fn)(std::move(x));
            });
    }

    ~Atmosphere()
    {
        visit([](auto& x) {
            using ty = std::remove_cv_t<std::remove_reference_t<decltype(x)>>;
            if constexpr (!std::is_trivially_destructible_v<ty>)
            {
                x.~ty();
            }
        });
    }
};

/// unlike a Lexeme, a Token owns the data it holds. Additionally it holds
/// preceding atmosphere and any trailing atmosphere until the next newline.
class ELY_EXPORT Token
{
private:
    TokenKind          kind;
    detail::TokenUnion values;

public:
    constexpr Token()
        : kind(TokenKind::Eof), values(std::in_place_type<token::Eof>)
    {}

    template<typename T, typename... Args>
    explicit constexpr Token(std::in_place_type_t<T>, Args&&... args)
        : kind(T::enum_value),
          values(std::in_place_type<T>, static_cast<Args&&>(args)...)
    {}

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
#define CALL(member) static_cast<F&&>(fn)(member)
        switch (kind)
        {
        case TokenKind::Eof:
            return CALL(values.eof);
        case TokenKind::LParen:
            return CALL(values.lparen);
        case TokenKind::RParen:
            return CALL(values.rparen);
        case TokenKind::LBracket:
            return CALL(values.lbracket);
        case TokenKind::RBracket:
            return CALL(values.rbracket);
        case TokenKind::LBrace:
            return CALL(values.lbrace);
        case TokenKind::RBrace:
            return CALL(values.rbrace);
        case TokenKind::Identifier:
            return CALL(values.identifier);
        case TokenKind::IntLit:
            return CALL(values.int_lit);
        case TokenKind::FloatLit:
            return CALL(values.float_lit);
        case TokenKind::CharLit:
            return CALL(values.char_lit);
        case TokenKind::StringLit:
            return CALL(values.string_lit);
        case TokenKind::KeywordLit:
            return CALL(values.keyword_lit);
        case TokenKind::BoolLit:
            return CALL(values.bool_lit);
        default:
            __builtin_unreachable();
        }
#undef CALL
    }

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return static_cast<const Token&>(*this).visit(
            [&](const auto& x) -> decltype(auto) {
                using ty =
                    std::remove_cv_t<std::remove_reference_t<decltype(x)>>;
                return static_cast<F&&>(fn)(const_cast<ty&>(x));
            });
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return visit([&](auto& x) -> decltype(auto) {
            return static_cast<F&&>(fn)(std::move(x));
        });
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return static_cast<const Token&>(*this).visit(
            [&](const auto& x) -> decltype(auto) {
                return static_cast<F&&>(fn)(std::move(x));
            });
    }

    ~Token()
    {
        visit([](auto& x) -> void {
            using ty = std::remove_reference_t<decltype(x)>;
            if constexpr (!std::is_trivially_destructible_v<ty>)
            {
                x.~ty();
            }
        });
    }
};
} // namespace ely
