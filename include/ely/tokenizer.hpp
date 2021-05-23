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

namespace token
{
class Eof
{};

class LParen
{};

class RParen
{};

class LBracket
{};

class RBracket
{};

class LBrace
{};

class RBrace
{};

class Identifier
{
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
union TokenUnion
{
    TokenUnion()
    {}

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

/// unlike a Lexeme, a Token owns the data it holds. Additionally it holds
/// preceding atmosphere and any trailing atmosphere until the next newline.
class ELY_EXPORT Token
{
    TokenKind          kind;
    detail::TokenUnion values;

    Token() = default;

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
            [&](const auto& x) { return static_cast<F&&>(fn)(std::move(x)); });
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