#pragma once

#include <numeric>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include "ely/scanner.hpp"
#include "ely/variant.hpp"

namespace ely
{
namespace atmosphere
{
class Whitespace
{
private:
    std::size_t len;

public:
    explicit constexpr Whitespace(std::size_t len) : len(len)
    {}

    template<typename I>
    constexpr Whitespace([[maybe_unused]] I first, std::size_t len)
        : Whitespace(len)
    {}

    constexpr std::size_t size() const
    {
        return len;
    }
};

class Tab
{
private:
    std::size_t len;

public:
    constexpr Tab(std::size_t len) : len(len)
    {}

    template<typename I>
    constexpr Tab([[maybe_unused]] I first, std::size_t len) : Tab(len)
    {}

    constexpr std::size_t size() const
    {
        return len;
    }
};

class NewlineCr
{
public:
    NewlineCr() = default;

    template<typename I>
    constexpr NewlineCr([[maybe_unused]] I           first,
                        [[maybe_unused]] std::size_t len)
        : NewlineCr()
    {}

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class NewlineLf
{
public:
    NewlineLf() = default;

    template<typename I>
    constexpr NewlineLf([[maybe_unused]] I           first,
                        [[maybe_unused]] std::size_t len)
        : NewlineLf()
    {}

    static constexpr std::size_t size()
    {
        return 1;
    }
};

class NewlineCrlf
{
public:
    NewlineCrlf() = default;

    template<typename I>
    constexpr NewlineCrlf([[maybe_unused]] I           first,
                          [[maybe_unused]] std::size_t len)
        : NewlineCrlf()
    {}

    static constexpr std::size_t size()
    {
        return 2;
    }
};

class Comment
{
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
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RParen
{
public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class LBracket
{
public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RBracket
{
public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class LBrace
{
public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class RBrace
{
public:
    static constexpr std::size_t size()
    {
        return 1;
    }
};

class Identifier
{
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
    Eof() = default;

    template<typename I>
    constexpr Eof(I, std::size_t) : Eof()
    {}

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
} // namespace detail

class Atmosphere
{
private:
    using VariantType = ely::Variant<atmosphere::Whitespace,
                                     atmosphere::Tab,
                                     atmosphere::NewlineCr,
                                     atmosphere::NewlineLf,
                                     atmosphere::NewlineCrlf,
                                     atmosphere::Comment>;

    VariantType variant_;

public:
    template<typename T, typename... Args>
    explicit constexpr Atmosphere(std::in_place_type_t<T>, Args&&... args)
        : variant_(std::in_place_type<T>, static_cast<Args&&>(args)...)
    {}

    template<typename I>
    explicit constexpr Atmosphere(Lexeme<I> lexeme)
        : variant_([&]() -> VariantType {
              ELY_ASSERT(ely::lexeme_is_atmosphere(lexeme.kind),
                         "Atmosphere must be made from atmosphere");

              switch (lexeme.kind)
              {
              case LexemeKind::Whitespace:
                  return atmosphere::Whitespace(lexeme.start, lexeme.size());
              default:
                  __builtin_unreachable();
              }
          }())
    {}

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return ely::visit(variant_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return ely::visit(variant_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return ely::visit(std::move(variant_), static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return ely::visit(std::move(variant_), static_cast<F&&>(fn));
    }

    constexpr std::size_t size() const
    {
        return visit([](const auto& x) -> std::size_t { return x.size(); });
    }
};

namespace detail
{}

/// unlike a Lexeme, a RawToken owns the data it holds
class RawToken
{
    using VariantType = ely::Variant<token::LParen,
                                     token::RParen,
                                     token::LBracket,
                                     token::RBracket,
                                     token::LBrace,
                                     token::RBrace,
                                     token::Identifier,
                                     token::IntLit,
                                     token::FloatLit,
                                     token::CharLit,
                                     token::StringLit,
                                     token::KeywordLit,
                                     token::BoolLit,
                                     token::Poison,
                                     token::Eof>;

private:
    VariantType variant_;

public:
    template<typename T, typename... Args>
    explicit constexpr RawToken(std::in_place_type_t<T>, Args&&... args)
        : variant_(std::in_place_type<T>, static_cast<Args&&>(args)...)
    {}

    template<typename I>
    constexpr RawToken(Lexeme<I> lexeme)
        : variant_([&]() -> VariantType {
              ELY_ASSERT(!ely::lexeme_is_atmosphere(lexeme.kind),
                         "RawToken cannot be made from atmosphere");

              switch (lexeme.kind)
              {
              case LexemeKind::LParen:
                  return VariantType(std::in_place_type<token::LParen>);
              case LexemeKind::RParen:
                  return VariantType(std::in_place_type<token::RParen>);
              case LexemeKind::LBracket:
                  return VariantType(std::in_place_type<token::LBracket>);
              case LexemeKind::RBracket:
                  return VariantType(std::in_place_type<token::RBracket>);
              case LexemeKind::LBrace:
                  return VariantType(std::in_place_type<token::LBrace>);
              case LexemeKind::RBrace:
                  return VariantType(std::in_place_type<token::RBrace>);

              case LexemeKind::Identifier:
                  return VariantType(std::in_place_type<token::Identifier>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::IntLit:
                  return VariantType(std::in_place_type<token::IntLit>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::FloatLit:
                  return VariantType(std::in_place_type<token::FloatLit>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::CharLit:
                  return VariantType(std::in_place_type<token::CharLit>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::StringLit:
                  return VariantType(std::in_place_type<token::StringLit>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::BoolLit:
                  return VariantType(std::in_place_type<token::BoolLit>,
                                     lexeme.start,
                                     lexeme.size());

              case LexemeKind::Poison:
                  return VariantType(std::in_place_type<token::Poison>,
                                     lexeme.start,
                                     lexeme.size());
              case LexemeKind::Eof:
                  return VariantType(std::in_place_type<token::Eof>,
                                     lexeme.start,
                                     lexeme.size());

              default:
                  __builtin_unreachable();
              }
          }())
    {}

    template<typename F>
    constexpr auto visit(F&& fn) const& -> decltype(auto)
    {
        return ely::visit(variant_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) & -> decltype(auto)
    {
        return ely::visit(variant_, static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) && -> decltype(auto)
    {
        return ely::visit(std::move(variant_), static_cast<F&&>(fn));
    }

    template<typename F>
    constexpr auto visit(F&& fn) const&& -> decltype(auto)
    {
        return ely::visit(std::move(variant_), static_cast<F&&>(fn));
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

            atmosphere_collector.emplace_back(lexeme);
        }

        RawToken raw_tok = RawToken(lexeme);

        ELY_UNIMPLEMENTED("collect trailing atmosphere and probably offload "
                          "collection to separate functions");
    }
};
} // namespace ely
