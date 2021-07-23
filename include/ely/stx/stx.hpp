#pragma once

#include "ely/lex/token.hpp"
#include "ely/variant.hpp"
#include "ely/vector.hpp"

namespace ely
{

namespace stx
{
template<typename TokenIt>
class Sexpr;

class MissingRParen
{
public:
    MissingRParen() = default;

    static constexpr std::size_t size() noexcept
    {
        return 0;
    }
};

class MissingRBracket
{
public:
    MissingRBracket() = default;

    static constexpr std::size_t size() noexcept
    {
        return 0;
    }
};

class MissingRBrace
{
public:
    MissingRBrace() = default;

    static constexpr std::size_t size() noexcept
    {
        return 0;
    }
};

template<typename TokenIt>
class List
{
private:
    ely::TokenVariantIterator<TokenIt, token::LParen> lparen_;
    ely::Variant<ely::TokenVariantIterator<TokenIt, token::RParen>,
                 MissingRParen>
        rparen_;

    ely::Vector<stx::Sexpr<TokenIt>> values_;

public:
    constexpr List(TokenIt                            lparen,
                   TokenIt                            rparen,
                   ely::Vector<stx::Sexpr<TokenIt>>&& values)
        : lparen_(lparen), rparen_(rparen), values_(std::move(values))
    {}

    constexpr List(TokenIt                            lparen,
                   MissingRParen                      rparen,
                   ely::Vector<stx::Sexpr<TokenIt>>&& values)
        : lparen_(lparen), rparen_(rparen), values_(std::move(values))
    {}
};

template<typename TokenIt>
class Literal : public TokenVariantIterator<TokenIt,
                                            token::IntLit,
                                            token::FloatLit,
                                            token::CharLit,
                                            token::StringLit,
                                            token::KeywordLit,
                                            token::BoolLit,
                                            token::UnterminatedStringLit>
{
    using base_ = TokenVariantIterator<TokenIt,
                                       token::IntLit,
                                       token::FloatLit,
                                       token::CharLit,
                                       token::StringLit,
                                       token::KeywordLit,
                                       token::BoolLit,
                                       token::UnterminatedStringLit>;

public:
    using base_::base_;
};

template<typename TokenIt>
class Identifier : public TokenVariantIterator<TokenIt,
                                               token::Identifier,
                                               token::InvalidNumberSign>
{
    using base_ = TokenVariantIterator<TokenIt,
                                       token::Identifier,
                                       token::InvalidNumberSign>;

public:
    explicit constexpr Identifier(TokenIt tok_it) : base_(tok_it)
    {}
};

template<typename TokenIt, template<typename> class Ptr>
class Var
{
private:
    ely::TokenVariantIterator<TokenIt, token::Colon> colon_;

    Ptr<Sexpr<TokenIt>> id_;
    Ptr<Sexpr<TokenIt>> ty_;

public:
    constexpr Var(TokenIt               colon_tok,
                  Ptr<Sexpr<TokenIt>>&& id,
                  Ptr<Sexpr<TokenIt>>&& ty)
        : colon_(colon_tok), id_(std::move(id)), ty_(std::move(ty))
    {}
};

template<typename TokenIt>
class Eof : public ely::TokenVariantIterator<TokenIt, token::Eof>
{
    using base_ = ely::TokenVariantIterator<TokenIt, token::Eof>;

public:
    using base_::base_;
};

template<typename TokenIt, template<typename> class Ptr>
class Sexpr : public ely::Variant<List<TokenIt>,
                                  Literal<TokenIt>,
                                  Identifier<TokenIt>,
                                  Var<TokenIt, Ptr>>
{
private:
    using base_ = ely::Variant<List<TokenIt>,
                               Literal<TokenIt>,
                               Identifier<TokenIt>,
                               Var<TokenIt>>;

public:
    using base_::base_;
};

class Stx : public ely::Variant<Sexpr, Eof>
{
    using base_ = ely::Variant<Sexpr, Eof>;

public:
    using base_::base_;
};

template<typename TokenIt>
class Module
{
    ely::Vector<stx::Sexpr>                             sexprs_;
    ely::TokenVariantIterator<TokenIt, ely::token::Eof> eof_;

public:
    Module() = default;

    constexpr Module(ely::Vector<stx::Sexpr>&& sexprs, TokenIt eof)
        : sexprs_(std::move(sexprs)), eof_(std::move(eof))
    {}

    void append_module(Module&& other)
    {
        sexprs_.
    }
};
} // namespace stx
} // namespace ely