#pragma once

#include <numeric>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include "ely/scanner.hpp"

namespace ely
{
enum class TokenKind : uint8_t
{
#define CAST(x) static_cast<std::underlying_type_t<LexemeKind>>(x)
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

    Poison = CAST(LexemeKind::Poison),
    Eof    = CAST(LexemeKind::Eof),
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
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class NewlineLf
{
public:
    static constexpr AtmosphereKind enum_value = AtmosphereKind::NewlineLf;

public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class NewlineCrlf
{
public:
    static constexpr AtmosphereKind enum_value = AtmosphereKind::NewlineCrlf;

public:
    static constexpr std::size_t size()
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

    template<typename I>
    constexpr Comment(I first, std::size_t len)
        : str(std::next(first), std::next(first, len))
    {}

    template<typename... Args>
    constexpr Comment(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return 1 + str.size();
    }
};
} // namespace atmosphere

namespace token
{
class LParen
{
public:
    static constexpr TokenKind enum_value = TokenKind::LParen;

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RParen
{
public:
    static constexpr TokenKind enum_value = TokenKind::RParen;

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class LBracket
{
public:
    static constexpr TokenKind enum_value = TokenKind::LBracket;

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RBracket
{
public:
    static constexpr TokenKind enum_value = TokenKind::RBracket;

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class LBrace
{
public:
    static constexpr TokenKind enum_value = TokenKind::LBrace;

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RBrace
{
public:
    static constexpr TokenKind enum_value = TokenKind::RBrace;

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class Identifier
{
public:
    static constexpr TokenKind enum_value = TokenKind::Identifier;

private:
    std::string name;

public:
    Identifier() = default;

    template<typename I>
    constexpr Identifier(I first, std::size_t len)
        : name(first, std::next(first, len))
    {}

    template<typename... Args>
    constexpr Identifier(std::in_place_t, Args&&... args)
        : name(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return name.size();
    }
};

class IntLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::IntLit;

private:
    std::string str;

public:
    IntLit() = default;

    template<typename I>
    constexpr IntLit(I first, std::size_t len)
        : str(first, std::next(first, len))
    {}

    template<typename... Args>
    constexpr IntLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str.size();
    }
};

class FloatLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::FloatLit;

private:
    std::string str;

public:
    FloatLit() = default;

    template<typename I>
    constexpr FloatLit(I first, std::size_t len)
        : str(first, std::next(first, len))
    {}

    template<typename... Args>
    constexpr FloatLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str.size();
    }
};

class CharLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::CharLit;

private:
    std::string str;

public:
    CharLit() = default;

    /// automatically cuts off the first 2 values from the iterator
    template<typename I>
    constexpr CharLit(I first, std::size_t len)
        : str(std::next(first, 2), std::next(first, len))
    {}

    template<typename... Args>
    constexpr CharLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return 2 + str.size();
    }
};

class StringLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::StringLit;

private:
    std::string str;

public:
    StringLit() = default;

    template<typename I>
    constexpr StringLit(I first, std::size_t len)
        : str(std::next(first), std::next(first, len - 1))
    {}

    template<typename... Args>
    constexpr StringLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return 2 + str.size();
    }
};

class KeywordLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::KeywordLit;

private:
    std::string str;

public:
    KeywordLit() = default;

    template<typename I>
    constexpr KeywordLit(I first, std::size_t len)
        : str(std::next(first, 2), std::next(first, len))
    {}

    template<typename... Args>
    constexpr KeywordLit(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return 2 + str.size();
    }
};

class BoolLit
{
public:
    static constexpr TokenKind enum_value = TokenKind::BoolLit;

private:
    bool b;

public:
    BoolLit() = default;

    template<typename I>
    constexpr BoolLit(I first, [[maybe_unused]] std::size_t len)
        : b(*std::next(first) == 't')
    {}

    constexpr BoolLit(bool b) : b(b)
    {}

    constexpr bool value() const
    {
        return b;
    }

    static constexpr std::size_t size()
    {
        return 2;
    }
};

class Poison
{
public:
    static constexpr TokenKind enum_value = TokenKind::Poison;

private:
    std::string str;

public:
    Poison() = default;

    template<typename I>
    constexpr Poison(I first, std::size_t len)
        : str(first, std::next(first, len))
    {}

    template<typename... Args>
    explicit constexpr Poison(std::in_place_t, Args&&... args)
        : str(static_cast<Args&&>(args)...)
    {}

    ELY_CONSTEXPR_STRING std::size_t size() const
    {
        return str.size();
    }
};

class Eof
{
public:
    static constexpr TokenKind enum_value = TokenKind::Eof;

public:
    Eof() = default;

    static constexpr std::size_t size()
    {
        return 0;
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
    DEFINE_CONSTRUCTOR(token::Poison, poison)
#undef DEFINE_CONSTRUCTOR

    ~TokenUnion()
    {}

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

    token::Poison poison;
};
} // namespace detail

class Atmosphere
{
    AtmosphereKind          kind;
    detail::AtmosphereUnion values;

private:
    constexpr Atmosphere()
        : kind(AtmosphereKind::NewlineLf),
          values(std::in_place_type<atmosphere::NewlineLf>)
    {}

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

    constexpr Atmosphere(const Atmosphere& other) : Atmosphere()
    {
        kind = other.kind;
        other.visit([&](auto& x) {
            using ty = std::remove_cvref_t<decltype(x)>;
            std::construct_at(
                std::addressof(values), std::in_place_type<ty>, x);
        });
    }

    constexpr Atmosphere(Atmosphere&& other) : Atmosphere()
    {
        kind = other.kind;
        std::move(other).visit([&](auto&& x) {
            using ty = std::remove_cvref_t<decltype(x)>;
            std::construct_at(
                std::addressof(values), std::in_place_type<ty>, std::move(x));
        });
    }

    ~Atmosphere()
    {
        visit([](auto& x) { std::destroy_at(std::addressof(x)); });
    }

    constexpr std::size_t size() const
    {
        return visit([](const auto& x) -> std::size_t { return x.size(); });
    }
};

/// unlike a Lexeme, a RawToken owns the data it holds
class RawToken
{
private:
    TokenKind          kind;
    detail::TokenUnion values;

private:
    // puts the raw token's state into a dummy value to appease constexpr
    // requirements
    constexpr RawToken()
        : kind(TokenKind::LParen), values(std::in_place_type<token::LParen>)
    {}

public:
    template<typename T, typename... Args>
    explicit constexpr RawToken(std::in_place_type_t<T>, Args&&... args)
        : kind(T::enum_value),
          values(std::in_place_type<T>, static_cast<Args&&>(args)...)
    {}

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
#define CALL(member) static_cast<F&&>(fn)(member)
        switch (kind)
        {
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
        case TokenKind::Poison:
            return CALL(values.poison);
        default:
            __builtin_unreachable();
        }
#undef CALL
    }

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return static_cast<const RawToken&>(*this).visit(
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
        return static_cast<const RawToken&>(*this).visit(
            [&](const auto& x) -> decltype(auto) {
                return static_cast<F&&>(fn)(std::move(x));
            });
    }

    constexpr RawToken(const RawToken& other) : RawToken()
    {
        kind = other.kind;
        other.visit([&](auto& x) {
            using ty = std::remove_cvref_t<decltype(x)>;
            std::construct_at(
                std::addressof(values), std::in_place_type<ty>, x);
        });
    }

    constexpr RawToken(RawToken&& other) noexcept : RawToken()
    {
        kind = other.kind;
        std::move(other).visit([&](auto&& x) {
            using ty = std::remove_cvref_t<decltype(x)>;
            std::construct_at(
                std::addressof(values), std::in_place_type<ty>, std::move(x));
        });
    }

    ~RawToken()
    {
        visit([](auto& x) -> void { std::destroy_at(std::addressof(x)); });
    }

    constexpr std::size_t size() const
    {
        return visit([](const auto& x) -> std::size_t { return x.size(); });
    }
};

class Token
{
    std::vector<Atmosphere> surrounding_atmosphere;
    std::size_t             trailing_at;
    RawToken                raw;

public:
    ELY_CONSTEXPR_VECTOR Token(std::vector<Atmosphere> surrounding_atmosphere,
                               std::size_t             trailing_at,
                               RawToken                tok)
        : surrounding_atmosphere(std::move(surrounding_atmosphere)),
          trailing_at(trailing_at), raw(std::move(tok))
    {}

    constexpr RawToken& raw_token() &
    {
        return raw;
    }

    constexpr const RawToken& raw_token() const&
    {
        return raw;
    }

    constexpr RawToken&& raw_token() &&
    {
        return std::move(raw);
    }

    constexpr const RawToken&& raw_token() const&&
    {
        return std::move(raw);
    }

    ELY_CONSTEXPR_VECTOR std::span<const Atmosphere> leading_atmosphere() const&
    {
        return std::span<const Atmosphere>{surrounding_atmosphere.begin(),
                                           trailing_at};
    }

    ELY_CONSTEXPR_VECTOR std::span<const Atmosphere>
                         trailing_atmosphere() const&
    {
        return std::span<const Atmosphere>{surrounding_atmosphere.begin() +
                                               trailing_at,
                                           surrounding_atmosphere.end()};
    }

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return static_cast<RawToken&>(raw).visit(static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return static_cast<const RawToken&>(raw).visit(static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return static_cast<RawToken&&>(raw).visit(static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return static_cast<const RawToken&&>(raw).visit(static_cast<F&&>(fn));
    }

    ELY_CONSTEXPR_VECTOR std::size_t size() const
    {
        std::size_t atmosphere_size = std::accumulate(
            surrounding_atmosphere.begin(),
            surrounding_atmosphere.end(),
            std::size_t{0},
            [](std::size_t sz, const auto& atmo) -> std::size_t {
                return sz + atmo.size();
            });
        return atmosphere_size +
               visit([](const auto& x) -> std::size_t { return x.size(); });
    }
};

template<typename I, typename S>
class TokenStream
{
public:
    using value_type = Token;
    using reference  = Token;

private:
    ely::ScannerStream<I, S> scanner_;

public:
    TokenStream() = default;

    constexpr TokenStream(I it, S end) : scanner_(std::move(it), std::move(end))
    {}

    constexpr reference next()
    {
        std::vector<Atmosphere> atmosphere_collector{};
        std::size_t             trailing_start = 0;

        auto lexeme = scanner_.next();

        while (lexeme && ely::lexeme_is_atmosphere(lexeme.kind))
        {
            ++trailing_start;

            switch (lexeme.kind)
            {
            case LexemeKind::Whitespace:
                atmosphere_collector.emplace_back(
                    std::in_place_type<atmosphere::Whitespace>, lexeme.size());
                break;
            case LexemeKind::Tab:
                atmosphere_collector.emplace_back(
                    std::in_place_type<atmosphere::Tab>, lexeme.size());
                break;
            case LexemeKind::NewlineCr:
                atmosphere_collector.emplace_back(
                    std::in_place_type<atmosphere::NewlineCr>);
                break;
            case LexemeKind::NewlineLf:
                atmosphere_collector.emplace_back(
                    std::in_place_type<atmosphere::NewlineLf>);
                break;
            case LexemeKind::NewlineCrlf:
                atmosphere_collector.emplace_back(
                    std::in_place_type<atmosphere::NewlineCrlf>);
                break;
            case LexemeKind::Comment:
                atmosphere_collector.emplace_back(
                    std::in_place_type<atmosphere::Comment>,
                    lexeme.start,
                    lexeme.size());
                break;
            default:
#ifndef NDEBUG
                std::fprintf(stderr,
                             "unexpected atmosphere %d\n",
                             static_cast<int>(lexeme.kind));
                std::terminate();
#endif
                __builtin_unreachable();
            }
        }

        RawToken raw_tok = [&] {
            switch (lexeme.kind)
            {
            case LexemeKind::LParen:
                return RawToken(std::in_place_type<token::LParen>);
            case LexemeKind::RParen:
                return RawToken(std::in_place_type<token::RParen>);
            case LexemeKind::LBracket:
                return RawToken(std::in_place_type<token::LBracket>);
            case LexemeKind::RBracket:
                return RawToken(std::in_place_type<token::RBracket>);
            case LexemeKind::LBrace:
                return RawToken(std::in_place_type<token::LBrace>);
            case LexemeKind::RBrace:
                return RawToken(std::in_place_type<token::RBrace>);

            case LexemeKind::Identifier:
                return RawToken(std::in_place_type<token::Identifier>,
                                lexeme.start,
                                lexeme.size());
            case LexemeKind::IntLit:
                return RawToken(std::in_place_type<token::IntLit>,
                                lexeme.start,
                                lexeme.size());
            case LexemeKind::FloatLit:
                return RawToken(std::in_place_type<token::FloatLit>,
                                lexeme.start,
                                lexeme.size());
            case LexemeKind::CharLit:
                return RawToken(std::in_place_type<token::CharLit>,
                                lexeme.start,
                                lexeme.size());
            case LexemeKind::StringLit:
                return RawToken(std::in_place_type<token::StringLit>,
                                lexeme.start,
                                lexeme.size());
            case LexemeKind::BoolLit:
                return RawToken(std::in_place_type<token::BoolLit>,
                                lexeme.start,
                                lexeme.size());

            case LexemeKind::Poison:
                return RawToken(std::in_place_type<token::Poison>,
                                lexeme.start,
                                lexeme.size());
            case LexemeKind::Eof:

            default:
#ifndef NDEBUG
                std::fprintf(stderr,
                             "unexpected lexeme received %d\n",
                             static_cast<int>(lexeme.kind));
                std::terminate();
#endif
                __builtin_unreachable();
            }
        }();

        ELY_UNIMPLEMENTED("collect trailing atmosphere and probably offload "
                          "collection to separate functions");
    }
};
} // namespace ely
