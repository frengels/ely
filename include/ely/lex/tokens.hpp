#pragma once

#include "ely/assert.h"
#include "ely/defines.h"
#include "ely/lex/span.hpp"
#include "ely/utility.hpp"
#include "ely/variant.hpp"

namespace ely
{
namespace token
{
using size_type = std::size_t;

class Whitespace;
class Tab;
class Comment;
class NewlineCr;
class NewlineLf;
class NewlineCrlf;
class LParen;
class RParen;
class LBracket;
class RBracket;
class LBrace;
class RBrace;
class Identifier;
class IntLit;
class FloatLit;
class CharLit;
class StringLit;
class KeywordLit;
class BoolLit;
class Colon;
class Quote;
class SyntaxQuote;
class At;
class Unquote;
class SyntaxUnquote;
class UnquoteSplicing;
class SyntaxUnquoteSplicing;
class Exclamation;
class Question;
class Ampersand;
class QuasiQuote;
class QuasiSyntax;
class UnterminatedStringLit;
class InvalidNumberSign;
class Eof;

template<typename D>
struct TokenBase
{
private:
    static constexpr bool D_must_inherit_from_TokenBaseD() noexcept
    {
        return std::is_base_of_v<TokenBase<D>, D>;
    }

#define CRTP_CHECK static_assert(D_must_inherit_from_TokenBaseD())

public:
    static constexpr bool is_atmosphere() noexcept
    {
        CRTP_CHECK;
        return is_trailing_atmosphere();
    }

    static constexpr bool is_leading_atmosphere() noexcept
    {
        CRTP_CHECK;
        return ely::is_same_one_of_v<D, NewlineCr, NewlineLf, NewlineCrlf> ||
               is_trailing_atmosphere();
    }

    static constexpr bool is_trailing_atmosphere() noexcept
    {
        CRTP_CHECK;
        return ely::is_same_one_of_v<D, Whitespace, Tab, Comment>;
    }

    static constexpr bool is_identifier() noexcept
    {
        CRTP_CHECK;
        return std::is_same_v<D, Identifier>;
    }

    static constexpr bool is_literal() noexcept
    {
        CRTP_CHECK;
        return ely::is_same_one_of_v<D,
                                     IntLit,
                                     FloatLit,
                                     CharLit,
                                     StringLit,
                                     KeywordLit,
                                     BoolLit>;
    }

    static constexpr bool is_prefix_abbrev() noexcept
    {
        CRTP_CHECK;
        return ely::is_same_one_of_v<D,
                                     Quote,
                                     SyntaxQuote,
                                     At,
                                     Unquote,
                                     SyntaxUnquote,
                                     UnquoteSplicing,
                                     SyntaxUnquoteSplicing,
                                     Exclamation,
                                     Question,
                                     Ampersand,
                                     QuasiQuote,
                                     QuasiSyntax>;
    }

    static constexpr bool is_infix_abbrev() noexcept
    {
        CRTP_CHECK;
        return std::is_same_v<D, Colon>;
    }

    static constexpr bool is_abbrev() noexcept
    {
        CRTP_CHECK;
        return is_prefix_abbrev() || is_infix_abbrev();
    }

    static constexpr bool is_poison() noexcept
    {
        CRTP_CHECK;
        return ely::
            is_same_one_of_v<D, UnterminatedStringLit, InvalidNumberSign>;
    }

    static constexpr bool is_eof() noexcept
    {
        CRTP_CHECK;
        return std::is_same_v<D, Eof>;
    }

#undef CRTP_CHECK
};

// atmosphere tokens

class Whitespace : TokenBase<Whitespace>
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
        std::in_place_type_t<Whitespace>,
        const ely::LexemeSpan<I>& span) noexcept
        : Whitespace{span.size()}
    {}

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return len_;
    }
};

class Tab : TokenBase<Tab>
{
    size_type len_{1};

public:
    Tab() = default;

    ELY_ALWAYS_INLINE explicit constexpr Tab(size_type len) : len_(len)
    {
        ELY_ASSERT(len != 0, "tab must be at least 1 in length");
    }

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Tab(std::in_place_type_t<Tab>,
                                    const ely::LexemeSpan<I>& span) noexcept
        : Tab{span.size()}
    {}

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return len_;
    }
};

class Comment : TokenBase<Comment>
{
    std::string str_;

public:
    Comment() = default;

    ELY_ALWAYS_INLINE explicit Comment(std::string str) : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Comment(std::in_place_type_t<Comment>,
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

class NewlineCr : TokenBase<NewlineCr>
{
    static constexpr auto str_ = std::array<char, 1>{'\r'};

public:
    NewlineCr() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr NewlineCr(std::in_place_type_t<NewlineCr>,
                                          const LexemeSpan<I>&) noexcept
        : NewlineCr{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class NewlineLf : TokenBase<NewlineLf>
{
    static constexpr auto str_ = std::array<char, 1>{'\n'};

public:
    NewlineLf() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr NewlineLf(std::in_place_type_t<NewlineLf>,
                                          const LexemeSpan<I>&) noexcept
        : NewlineLf{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {

        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class NewlineCrlf : TokenBase<NewlineCrlf>
{
    static constexpr auto str_ = std::array<char, 2>{'\r', '\n'};

public:
    NewlineCrlf() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr NewlineCrlf(std::in_place_type_t<NewlineCrlf>,
                                            const LexemeSpan<I>&) noexcept
        : NewlineCrlf{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

// real tokens

class LParen : TokenBase<LParen>
{
    static constexpr auto str_ = std::array<char, 1>{'('};

public:
    LParen() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr LParen(std::in_place_type_t<LParen>,
                                       const LexemeSpan<I>&) noexcept
        : LParen{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class RParen : TokenBase<RParen>
{
    static constexpr auto str_ = std::array<char, 1>{')'};

public:
    RParen() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr RParen(std::in_place_type_t<RParen>,
                                       const LexemeSpan<I>&) noexcept
        : RParen{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class LBracket : TokenBase<LBracket>
{
    static constexpr auto str_ = std::array<char, 1>{'['};

public:
    LBracket() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr LBracket(std::in_place_type_t<LBracket>,
                                         const LexemeSpan<I>&) noexcept
        : LBracket{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class RBracket : TokenBase<RBracket>
{
    static constexpr auto str_ = std::array<char, 1>{']'};

public:
    RBracket() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr RBracket(std::in_place_type_t<RBracket>,
                                         const LexemeSpan<I>&) noexcept
        : RBracket{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class LBrace : TokenBase<LBrace>
{
    static constexpr auto str_ = std::array<char, 1>{'{'};

public:
    LBrace() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr LBrace(std::in_place_type_t<LBrace>,
                                       const LexemeSpan<I>&) noexcept
        : LBrace{}
    {}

    static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class RBrace : TokenBase<RBrace>
{
    static constexpr auto str_ = std::array<char, 1>{'}'};

public:
    RBrace() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr RBrace(std::in_place_type_t<RBrace>,
                                       const LexemeSpan<I>&) noexcept
        : RBrace{}
    {}

    static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};
class Identifier : TokenBase<Identifier>
{
    std::string name_;

public:
    Identifier() = default;

    ELY_ALWAYS_INLINE explicit Identifier(std::string name)
        : name_(std::move(name))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Identifier(std::in_place_type_t<Identifier>,
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

class IntLit : TokenBase<IntLit>
{
    std::string str_;

public:
    IntLit() = default;

    ELY_ALWAYS_INLINE explicit IntLit(std::string str) : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr IntLit(std::in_place_type_t<IntLit>,
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

class FloatLit : TokenBase<FloatLit>
{
    std::string str_;

public:
    FloatLit() = default;

    ELY_ALWAYS_INLINE explicit FloatLit(std::string str) : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr FloatLit(std::in_place_type_t<FloatLit>,
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

class CharLit : TokenBase<CharLit>
{
    std::string str_;

public:
    CharLit() = default;

    ELY_ALWAYS_INLINE explicit CharLit(std::string str) : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr CharLit(std::in_place_type_t<CharLit>,
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

class StringLit : TokenBase<StringLit>
{
    std::string str_;

public:
    StringLit() = default;

    ELY_ALWAYS_INLINE explicit StringLit(std::string str) : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr StringLit(std::in_place_type_t<StringLit>,
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

class KeywordLit : TokenBase<KeywordLit>
{
    std::string str_;

public:
    KeywordLit() = default;

    ELY_ALWAYS_INLINE explicit KeywordLit(std::string str)
        : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr KeywordLit(std::in_place_type_t<KeywordLit>,
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

class BoolLit : TokenBase<BoolLit>
{
    static constexpr auto true_  = std::array<char, 2>{'#', 't'};
    static constexpr auto false_ = std::array<char, 2>{'#', 'f'};

    bool b_{false};

public:
    BoolLit() = default;

    ELY_ALWAYS_INLINE explicit constexpr BoolLit(bool b) : b_(b)
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr BoolLit(std::in_place_type_t<BoolLit>,
                                        const ely::LexemeSpan<I>& span)
        : b_{*std::next(span.begin()) == 't'}
    {}

    ELY_ALWAYS_INLINE explicit constexpr operator bool() const noexcept
    {
        return b_;
    }

    ELY_ALWAYS_INLINE constexpr std::string_view str() const noexcept
    {
        return b_ ? std::string_view{true_.data(), true_.size()} :
                    std::string_view{false_.data(), false_.size()};
    }

    ELY_ALWAYS_INLINE constexpr size_type size() const noexcept
    {
        return str().size();
    }
};

class Colon : TokenBase<Colon>
{
    static constexpr auto str_ = std::array<char, 1>{':'};

public:
    Colon() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Colon(std::in_place_type_t<Colon>,
                                      const LexemeSpan<I>&) noexcept
        : Colon{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class Quote : TokenBase<Quote>
{
    static constexpr auto str_ = std::array<char, 1>{'\''};

public:
    Quote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Quote(std::in_place_type_t<Quote>,
                                      const LexemeSpan<I>&) noexcept
        : Quote{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class SyntaxQuote : TokenBase<SyntaxQuote>
{
    static constexpr auto str_ = std::array<char, 2>{'#', '\''};

public:
    SyntaxQuote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr SyntaxQuote(std::in_place_type_t<SyntaxQuote>,
                                            const LexemeSpan<I>&) noexcept
        : SyntaxQuote{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class At : TokenBase<At>
{
    static constexpr auto str_ = std::array<char, 1>{'@'};

public:
    At() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr At(std::in_place_type_t<At>,
                                   const LexemeSpan<I>&) noexcept
        : At{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class Unquote : TokenBase<Unquote>
{
    static constexpr auto str_ = std::array<char, 1>{','};

public:
    Unquote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Unquote(std::in_place_type_t<Unquote>,
                                        const LexemeSpan<I>&) noexcept
        : Unquote{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class SyntaxUnquote : TokenBase<SyntaxUnquote>
{
    static constexpr auto str_ = std::array<char, 2>{'#', ','};

public:
    SyntaxUnquote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr SyntaxUnquote(
        std::in_place_type_t<SyntaxUnquote>,
        const LexemeSpan<I>&) noexcept
        : SyntaxUnquote{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class UnquoteSplicing : TokenBase<UnquoteSplicing>
{
    static constexpr auto str_ = std::array<char, 2>{',', '@'};

public:
    UnquoteSplicing() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr UnquoteSplicing(
        std::in_place_type_t<UnquoteSplicing>,
        const LexemeSpan<I>&) noexcept
        : UnquoteSplicing{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class SyntaxUnquoteSplicing : TokenBase<SyntaxUnquoteSplicing>
{
    static constexpr auto str_ = std::array<char, 3>{'#', ',', '@'};

public:
    SyntaxUnquoteSplicing() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr SyntaxUnquoteSplicing(
        std::in_place_type_t<SyntaxUnquoteSplicing>,
        const LexemeSpan<I>&) noexcept
        : SyntaxUnquoteSplicing{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class Exclamation : TokenBase<Exclamation>
{
    static constexpr auto str_ = std::array<char, 1>{'!'};

public:
    Exclamation() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Exclamation(std::in_place_type_t<Exclamation>,
                                            const LexemeSpan<I>&) noexcept
        : Exclamation{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class Question : TokenBase<Question>
{
    static constexpr auto str_ = std::array<char, 1>{'?'};

public:
    Question() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Question(std::in_place_type_t<Question>,
                                         const LexemeSpan<I>&) noexcept
        : Question{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class Ampersand : TokenBase<Ampersand>
{
    static constexpr auto str_ = std::array<char, 1>{'&'};

public:
    Ampersand() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Ampersand(std::in_place_type_t<Ampersand>,
                                          const LexemeSpan<I>&) noexcept
        : Ampersand{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class QuasiQuote : TokenBase<QuasiQuote>
{
    static constexpr auto str_ = std::array<char, 1>{'`'};

public:
    QuasiQuote() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr QuasiQuote(std::in_place_type_t<QuasiQuote>,
                                           const LexemeSpan<I>&) noexcept
        : QuasiQuote{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class QuasiSyntax : TokenBase<QuasiSyntax>
{
    static constexpr auto str_ = std::array<char, 2>{'#', '`'};

public:
    QuasiSyntax() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr QuasiSyntax(std::in_place_type_t<QuasiSyntax>,
                                            const LexemeSpan<I>&) noexcept
        : QuasiSyntax{}
    {}

    ELY_ALWAYS_INLINE static constexpr std::string_view str() noexcept
    {
        return std::string_view{str_.data(), str_.size()};
    }

    ELY_ALWAYS_INLINE static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class UnterminatedStringLit : TokenBase<UnterminatedStringLit>
{
    std::string str_;

public:
    UnterminatedStringLit() = default;

    explicit ELY_ALWAYS_INLINE UnterminatedStringLit(std::string str)
        : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr UnterminatedStringLit(
        std::in_place_type_t<UnterminatedStringLit>,
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

class InvalidNumberSign : TokenBase<InvalidNumberSign>
{
    std::string str_;

public:
    InvalidNumberSign() = default;

    explicit inline InvalidNumberSign(std::string str) : str_(std::move(str))
    {}

    template<typename I>
    ELY_ALWAYS_INLINE constexpr InvalidNumberSign(
        std::in_place_type_t<InvalidNumberSign>,
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

class Eof : TokenBase<Eof>
{
public:
    Eof() = default;

    template<typename I>
    ELY_ALWAYS_INLINE constexpr Eof(std::in_place_type_t<Eof>,
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

using token_types = ely::type_list<token::Whitespace,
                                   token::Tab,
                                   token::NewlineCr,
                                   token::NewlineLf,
                                   token::NewlineCrlf,
                                   token::Comment,
                                   token::LParen,
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
                                   token::Colon,
                                   token::Quote,
                                   token::SyntaxQuote,
                                   token::At,
                                   token::Unquote,
                                   token::SyntaxUnquote,
                                   token::UnquoteSplicing,
                                   token::SyntaxUnquoteSplicing,
                                   token::Exclamation,
                                   token::Question,
                                   token::Ampersand,
                                   token::QuasiQuote,
                                   token::QuasiSyntax,
                                   token::UnterminatedStringLit,
                                   token::InvalidNumberSign,
                                   token::Eof>;

using variant_type = token_types::template apply_all<ely::Variant>;
} // namespace token
} // namespace ely