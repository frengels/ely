#pragma once

#include <array>
#include <string>

#include "ely/assert.h"
#include "ely/variant.hpp"

namespace ely
{
namespace token2
{
using size_type = std::size_t;

// atmosphere tokens

class Whitespace
{
    size_type len_{1};

public:
    Whitespace() = default;

    explicit constexpr Whitespace(size_type len) : len_(len)
    {
        ELY_ASSERT(len != 0, "whitespace must be at least 1 in length");
    }

    constexpr size_type size() const noexcept
    {
        return len_;
    }
};

class Tab
{
    size_type len_{1};

public:
    Tab() = default;

    explicit constexpr Tab(size_type len) : len_(len)
    {
        ELY_ASSERT(len != 0, "tab must be at least 1 in length");
    }

    constexpr size_type size() const noexcept
    {
        return len_;
    }
};

class NewlineCr
{
public:
    NewlineCr() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'\r'};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class NewlineLf
{
public:
    NewlineLf() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'\n'};

        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class NewlineCrlf
{
public:
    NewlineCrlf() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 2>{'\r', '\n'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class Comment
{
    std::string str_;

public:
    Comment() = default;

    explicit constexpr Comment(std::string str) : str_(std::move(str))
    {}

    size_type size() const noexcept
    {
        return str_.size();
    }
};

// real tokens

class LParen
{
public:
    LParen() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'('};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class RParen
{
public:
    RParen() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{')'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class LBracket
{
public:
    LBracket() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'['};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class RBracket
{
public:
    RBracket() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{']'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class LBrace
{
public:
    LParen() = default;

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
    LParen() = default;

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

    explicit inline Identifier(std::string name) : name_(std::move(name))
    {}

    inline std::string_view name() const noexcept
    {
        return static_cast<std::string_view>(name_);
    }

    inline size_type size() const noexcept
    {
        return name().size();
    }
};

class IntLit
{
    std::string str_;

public:
    IntLit() = default;

    explicit inline IntLit(std::string str) : str_(std::move(str))
    {}

    inline std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    inline size_type size() const noexcept
    {
        return str().size();
    }
};

class FloatLit
{
    std::string str_;

public:
    FloatLit() = default;

    explicit inline FloatLit(std::string str) : str_(std::move(str))
    {}

    inline std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    inline size_type size() const noexcept
    {
        return str().size();
    }
};

class CharLit
{
    std::string str_;

public:
    CharLit() = default;

    explicit inline CharLit(std::string str) : str_(std::move(str))
    {}

    inline std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    inline size_type size() const noexcept
    {
        return str().size();
    }
};

class StringLit
{
    std::string str_;

public:
    StringLit() = default;

    explicit inline StringLit(std::string str) : str_(std::move(str))
    {}

    inline std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    inline size_type size() const noexcept
    {
        return str().size();
    }
};

class KeywordLit
{
    std::string str_;

public:
    KeywordLit() = default;

    explicit inline KeywordLit(std::string str) : str_(std::move(str))
    {}

    inline std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    inline size_type size() const noexcept
    {
        return str().size();
    }
};

class BoolLit
{
    bool b_{false};

public:
    BoolLit() = default;

    explicit constexpr BoolLit(bool b) : b_(b)
    {}

    explicit constexpr operator bool() const noexcept
    {
        return b_;
    }

    constexpr std::string_view str() const noexcept
    {
        static constexpr auto true_  = std::array<char, 2>{'#', 't'};
        static constexpr auto false_ = std::array<char, 2>{'#', 'f'};

        return b_ ? std::string_view{true_.data(), true_.size()} :
                    std::string_view{false_.data(), false_.size()};
    }

    constexpr size_type size() const noexcept
    {
        return str().size();
    }
};

class Colon
{
public:
    Colon() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{':'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class Quote
{
public:
    Quote() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'\''};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class SyntaxQuote
{
public:
    SyntaxQuote() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 2>{'#', '\''};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class At
{
public:
    At() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'@'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class Unquote
{
public:
    Unquote() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{','};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class SyntaxUnquote
{
public:
    SyntaxUnquote() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 2>{'#', ','};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class UnquoteSplicing
{
public:
    UnquoteSplicing() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 2>{',', '@'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class SyntaxUnquoteSplicing
{
public:
    SyntaxUnquoteSplicing() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 3>{'#', ',', '@'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class Exclamation
{
public:
    Exclamation() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'!'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class Question
{
public:
    Question() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'?'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class Asterisk
{
public:
    Asterisk() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'*'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class QuasiQuote
{
public:
    QuasiQuote() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 1>{'`'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class QuasiSyntax
{
public:
    QuasiSyntax() = default;

    static constexpr std::string_view str() noexcept
    {
        static constexpr auto str_ = std::array<char, 2>{'#', '`'};
        return std::string_view{str_.data(), str_.size()};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};

class UnterminatedStringLit
{
    std::string str_;

public:
    UnterminatedStringLit() = default;

    explicit inline UnterminatedStringLit(std::string str)
        : str_(std::move(str))
    {}

    inline std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    inline size_type size() const noexcept
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

    inline std::string_view str() const noexcept
    {
        return static_cast<std::string_view>(str_);
    }

    inline size_type size() const noexcept
    {
        return str().size();
    }
};

class Eof
{
public:
    Eof() = default;

    static constexpr std::string_view str() noexcept
    {
        return std::string_view{};
    }

    static constexpr size_type size() noexcept
    {
        return str().size();
    }
};
} // namespace token2
} // namespace ely