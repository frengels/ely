#pragma once

#include <array>
#include <string>

#include "ely/assert.h"
#include "ely/scanner.hpp"
#include "ely/utility.hpp"
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
        ely::type_identity<Whitespace>,
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
    ELY_ALWAYS_INLINE constexpr Tab(ely::type_identity<Tab>,
                                    const ely::LexemeSpan<I>& span) noexcept
        : Tab{span.size()}
    {}

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return len_;
    }
};

class NewlineCr
{
public:
    NewlineCr() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr NewlineCr(ely::type_identity<NewlineCr>,
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

class NewlineLf
{
public:
    NewlineLf() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr NewlineLf(ely::type_identity<NewlineLf>,
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

class NewlineCrlf
{
    NewlineCrlf() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr NewlineCrlf(ely::type_identity<NewlineCrlf>,
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

class Comment
{
    std::string str_;

public:
    Comment() = default;

    ELY_ALWAYS_INLINE explicit constexpr Comment(std::string str)
        : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Comment(ely::type_identity<Comment>,
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

class LParen
{
public:
    LParen() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr LParen(ely::type_identity<LParen>,
                                       const LexemeSpan<I>&) noexcept
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

class RParen
{
public:
    RParen() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr RParen(ely::type_identity<RParen>,
                                       const LexemeSpan<I>&) noexcept
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

class LBracket
{
public:
    LBracket() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr LBracket(ely::type_identity<LBracket>,
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

class RBracket
{
public:
    RBracket() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr RBracket(ely::type_identity<RBracket>,
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

class LBrace
{
public:
    LBrace() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr LBrace(ely::type_identity<LBrace>,
                                       const LexemeSpan<I>&) noexcept
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

class RBrace
{
public:
    RBrace() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr RBrace(ely::type_identity<RBrace>,
                                       const LexemeSpan<I>&) noexcept
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
class Identifier
{
    std::string name_;

public:
    Identifier() = default;

    ELY_ALWAYS_INLINE explicit Identifier(std::string name)
        : name_(std::move(name))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Identifier(ely::type_identity<Identifier>,
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
    ELY_ALWAYS_INLINE constexpr IntLit(ely::type_identity<IntLit>,
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
    ELY_ALWAYS_INLINE constexpr FloatLit(ely::type_identity<FloatLit>,
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
    ELY_ALWAYS_INLINE constexpr CharLit(ely::type_identity<CharLit>,
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
    ELY_ALWAYS_INLINE constexpr StringLit(ely::type_identity<StringLit>,
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
    ELY_ALWAYS_INLINE constexpr KeywordLit(ely::type_identity<KeywordLit>,
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
    ELY_ALWAYS_INLINE constexpr Comment(ely::type_identity<BoolLit>,
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

class Colon
{
public:
    Colon() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Colon(ely::type_identity<Colon>,
                                      const LexemeSpan<I>&) noexcept
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

class Quote
{
public:
    Quote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Quote(ely::type_identity<Quote>,
                                      const LexemeSpan<I>&) noexcept
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

class SyntaxQuote
{
public:
    SyntaxQuote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr SyntaxQuote(ely::type_identity<SyntaxQuote>,
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

class At
{
public:
    At() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr At(ely::type_identity<At>,
                                   const LexemeSpan<I>&) noexcept
        : At{}
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

class Unquote
{
public:
    Unquote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Unquote(ely::type_identity<Unquote>,
                                        const LexemeSpan<I>&) noexcept
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

class SyntaxUnquote
{
public:
    SyntaxUnquote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr SyntaxUnquote(ely::type_identity<SyntaxUnquote>,
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

class UnquoteSplicing
{
public:
    UnquoteSplicing() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr UnquoteSplicing(
        ely::type_identity<UnquoteSplicing>,
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

class SyntaxUnquoteSplicing
{
public:
    SyntaxUnquoteSplicing() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr SyntaxUnquoteSplicing(
        ely::type_identity<SyntaxUnquoteSplicing>,
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

class Exclamation
{
public:
    Exclamation() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Exclamation(ely::type_identity<Exclamation>,
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

class Question
{
public:
    Question() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Question(ely::type_identity<Question>,
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

class Asterisk
{
public:
    Asterisk() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Asterisk(ely::type_identity<Asterisk>,
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

class QuasiQuote
{
public:
    QuasiQuote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr QuasiQuote(ely::type_identity<QuasiQuote>,
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

class QuasiSyntax
{
public:
    QuasiSyntax() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr QuasiSyntax(ely::type_identity<QuasiSyntax>,
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
        ely::type_identity<UnterminatedStringLit>,
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
        ely::type_identity<InvalidNumberSign>,
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

class Eof
{
public:
    Eof() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Eof(ely::type_identity<Eof>,
                                    const LexemeSpan<I>&) noexcept
        : Eof{}
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

using token_types = ely::type_list<token2::Whitespace,
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
                                   token2::Eof>;

using variant_type = token_types::template apply_all<ely::Variant>;
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