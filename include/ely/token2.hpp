#pragma once

#include <array>
#include <string>

#include "ely/assert.h"
#include "ely/scanner.hpp"
#include "ely/variant.hpp"

namespace ely
{
namespace token2
{
using size_type = ely::lexeme::size_type;

// atmosphere tokens

class Whitespace
{
    size_type len_{1};

public:
    Whitespace() = default;

    ELY_ALWAYS_INLINE explicit constexpr Whitespace(size_type len) noexcept
        : len_(len)
    {
        ELY_ASSERT(len != 0, "whitespace must be at least 1 in length");
    }

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Whitespace(
        ely::lexeme::Whitespace,
        const ely::LexemeSpan<I>& span) noexcept
        : Whitespace{span.size()}
    {}

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return len_;
    }
};

class Tab
{
    size_type len_{1};

public:
    Tab() = default;

    ELY_ALWAYS_INLINE explicit constexpr Tab(size_type len) : len_(len)
    {
        ELY_ASSERT(len != 0, "tab must be at least 1 in length");
    }

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Tab(ely::lexeme::Tab,
                                    const ely::LexemeSpan<I>& span) noexcept
        : Tab{span.size()}
    {}

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return len_;
    }
};

using NewlineCr   = ely::lexeme::NewlineCr;
using NewlineLf   = ely::lexeme::NewlineLf;
using NewlineCrlf = ely::lexeme::NewlineCrlf;

class Comment
{
    std::string str_;

public:
    Comment() = default;

    ELY_ALWAYS_INLINE explicit constexpr Comment(std::string str)
        : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Comment(ely::lexeme::Comment,
                                        const ely::LexemeSpan<I>& span)
        : str_(span.begin(), span.end())
    {}

    ELY_ALWAYS_INLINE std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_ALWAYS_INLINE size_type size() const noexcept
    {
        return str().size();
    }
};

// real tokens

using LParen   = ely::lexeme::LParen;
using RParen   = ely::lexeme::RParen;
using LBracket = ely::lexeme::LBracket;
using RBracket = ely::lexeme::RBracket;
using LBrace   = ely::lexeme::LBrace;
using RBrace   = ely::lexeme::RBrace;

class Identifier
{
    std::string name_;

public:
    Identifier() = default;

    ELY_ALWAYS_INLINE explicit Identifier(std::string name)
        : name_(std::move(name))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Identifier(ely::lexeme::Identifier,
                                           const ely::LexemeSpan<I>& span)
        : name_(span.begin(), span.end())
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Identifier(I it, I end)
        : name_(std::move(it), std::move(end))
    {}

    ELY_ALWAYS_INLINE std::string_view name() const noexcept
    {
        return static_cast<std::string_view>(name_);
    }

    ELY_ALWAYS_INLINE std::string_view str() const noexcept
    {
        return name();
    }

    ELY_ALWAYS_INLINE size_type size() const noexcept
    {
        return name().size();
    }
};

class IntLit
{
    std::string str_;

public:
    IntLit() = default;

    ELY_ALWAYS_INLINE explicit IntLit(std::string str) : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr IntLit(ely::lexeme::IntLit,
                                       const ely::LexemeSpan<I>& span)
        : str_(span.begin(), span.end())
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr IntLit(I it, I end)
        : str_(std::move(it), std::move(end))
    {}

    ELY_ALWAYS_INLINE std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_ALWAYS_INLINE size_type size() const noexcept
    {
        return str().size();
    }
};

class FloatLit
{
    std::string str_;

public:
    FloatLit() = default;

    ELY_ALWAYS_INLINE explicit FloatLit(std::string str) : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr FloatLit(ely::lexeme::FloatLit,
                                         const ely::LexemeSpan<I>& span)
        : str_(span.begin(), span.end())
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr FloatLit(I it, I end)
        : str_(std::move(it), std::move(end))
    {}

    ELY_ALWAYS_INLINE std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_ALWAYS_INLINE size_type size() const noexcept
    {
        return str().size();
    }
};

class CharLit
{
    std::string str_;

public:
    CharLit() = default;

    ELY_ALWAYS_INLINE explicit CharLit(std::string str) : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr CharLit(ely::lexeme::CharLit,
                                        const ely::LexemeSpan<I>& span)
        : str_(span.begin(), span.end())
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr CharLit(I it, I end)
        : str_(std::move(it), std::move(end))
    {}

    ELY_ALWAYS_INLINE std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_ALWAYS_INLINE size_type size() const noexcept
    {
        return str().size();
    }
};

class StringLit
{
    std::string str_;

public:
    StringLit() = default;

    ELY_ALWAYS_INLINE explicit StringLit(std::string str) : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr StringLit(ely::lexeme::StringLit,
                                          const ely::LexemeSpan<I>& span)
        : str_(span.begin(), span.end())
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr StringLit(I it, I end)
        : str_(std::move(it), std::move(end))
    {}

    ELY_ALWAYS_INLINE std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_ALWAYS_INLINE size_type size() const noexcept
    {
        return str().size();
    }
};

class KeywordLit
{
    std::string str_;

public:
    KeywordLit() = default;

    ELY_ALWAYS_INLINE explicit KeywordLit(std::string str)
        : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr KeywordLit(ely::lexeme::KeywordLit,
                                           const ely::LexemeSpan<I>& span)
        : str_(span.begin(), span.end())
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr KeywordLit(I it, I end)
        : str_(std::move(it), std::move(end))
    {}

    ELY_ALWAYS_INLINE std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_ALWAYS_INLINE size_type size() const noexcept
    {
        return str().size();
    }
};

class BoolLit
{
    bool b_{false};

public:
    BoolLit() = default;

    ELY_ALWAYS_INLINE explicit constexpr BoolLit(bool b) : b_(b)
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Comment(ely::lexeme::BoolLit,
                                        const ely::LexemeSpan<I>& span)
        : b_{*++span.begin() == 't': true: false}
    {}

    ELY_ALWAYS_INLINE explicit constexpr operator bool() const noexcept
    {
        return b_;
    }

    ELY_ALWAYS_INLINE constexpr std::string_view str() const noexcept
    {
        static constexpr auto true_  = std::array<char, 2>{'#', 't'};
        static constexpr auto false_ = std::array<char, 2>{'#', 'f'};

        return b_ ? std::string_view{true_.data(), true_.size()} :
                    std::string_view{false_.data(), false_.size()};
    }

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return str().size();
    }
};

using Colon                 = ely::lexeme::Colon;
using Quote                 = ely::lexeme::Quote;
using SyntaxQuote           = ely::lexeme::SyntaxQuote;
using At                    = ely::lexeme::At;
using Unquote               = ely::lexeme::Unquote;
using SyntaxUnquote         = ely::lexeme::SyntaxUnquote;
using UnquoteSplicing       = ely::lexeme::UnquoteSplicing;
using SyntaxUnquoteSplicing = ely::lexeme::SyntaxUnquoteSplicing;
using Exclamation           = ely::lexeme::Exclamation;
using Question              = ely::lexeme::Question;
using Asterisk              = ely::lexeme::Asterisk;
using QuasiQuote            = ely::lexeme::QuasiQuote;
using QuasiSyntax           = ely::lexeme::QuasiSyntax;

class UnterminatedStringLit
{
    std::string str_;

public:
    UnterminatedStringLit() = default;

    explicit ELY_ALWAYS_INLINE UnterminatedStringLit(std::string str)
        : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr UnterminatedStringLit(
        ely::lexeme::UnterminatedStringLit,
        const ely::LexemeSpan<I>& span)
        : str_(span.begin(), span.end())
    {}

    ELY_ALWAYS_INLINE std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_ALWAYS_INLINE size_type size() const noexcept
    {
        return str().size();
    }
};

class InvalidNumberSign
{
    std::string str_;

public:
    InvalidNumberSign() = default;

    explicit inline InvalidNumberSign(std::string str) : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr InvalidNumberSign(
        ely::lexeme::InvalidNumberSign,
        const ely::LexemeSpan<I>& span)
        : str_(span.begin(), span.end())
    {}

    ELY_ALWAYS_INLINE std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    ELY_ALWAYS_INLINE size_type size() const noexcept
    {
        return str().size();
    }
};

using Eof = ely::lexeme::Eof;

using variant_type = ely::Variant<token2::Whitespace,
                                  token2::Tab,
                                  token2::NewlineCr,
                                  token2::NewlineLf,
                                  token2::NewlineCrlf,
                                  token2::Comment,
                                  token2::LParen,
                                  token2::RParen,
                                  token2::LBracket,
                                  token2::RBracket,
                                  token2::LBrace,
                                  token2::RBrace,
                                  token2::Identifier,
                                  token2::IntLit,
                                  token2::FloatLit,
                                  token2::CharLit,
                                  token2::StringLit,
                                  token2::KeywordLit,
                                  token2::BoolLit,
                                  token2::Colon,
                                  token2::Quote,
                                  token2::SyntaxQuote,
                                  token2::At,
                                  token2::Unquote,
                                  token2::SyntaxUnquote,
                                  token2::UnquoteSplicing,
                                  token2::SyntaxUnquoteSplicing,
                                  token2::Exclamation,
                                  token2::Question,
                                  token2::Asterisk,
                                  token2::QuasiQuote,
                                  token2::QuasiSyntax,
                                  token2::UnterminatedStringLit,
                                  token2::InvalidNumberSign,
                                  token2::Eof>
} // namespace token2

class Token2 : public token2::variant_type
{
    using base_ = token2::variant_type;

public:
    using base_::base_;

    template<typename I>
    Token2(ely::)
};
} // namespace ely