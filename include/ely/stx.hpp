#pragma once

#include <memory>
#include <tuple>
#include <type_traits>

#include "ely/token.hpp"
#include "ely/variant.hpp"
#include "ely/vector.hpp"

namespace ely
{
namespace stx
{
class Sexpr;
class Syntax;
class List;
class Literal;
class Identifier;
class Eof;

class MissingRParen
{
public:
    using poison_tag = void;

public:
    static constexpr std::size_t size() noexcept
    {
        return 0;
    }
};

class MissingRBracket
{
public:
    using poison_tag = void;

public:
    static constexpr std::size_t size() noexcept
    {
        return 0;
    }
};

class MissingRBrace
{
public:
    using poison_tag = void;

public:
    static constexpr std::size_t size() noexcept
    {
        return 0;
    }
};

class List
{
private:
    ely::TokenVariant<ely::token::LParen>
        lparen_; // owns atmosphere before and after
    // this token can be Poison if no ending parenthesis is found
    ely::TokenVariant<ely::token::RParen, MissingRParen>
        rparen_; // owns atmosphere before and after

    ely::Vector<stx::Sexpr> values_{};
    // summed size of the inner part of the stx node (  |__|__|__)
    std::size_t values_size_{0};
    bool        value_poisoned_{false};

public:
    template<typename... LArgs, typename... RArgs>
    constexpr List(std::tuple<LArgs...>      lparen_args,
                   std::tuple<RArgs...>      rparen_args,
                   ely::Vector<stx::Sexpr>&& values,
                   std::size_t               values_size,
                   bool                      value_poisoned)
        : lparen_(std::apply(
              [](auto&&... args) {
                  return decltype(lparen_){
                      static_cast<decltype(args)&&>(args)...};
              },
              std::move(lparen_args))),
          rparen_(std::apply(
              [](auto&&... args) {
                  return decltype(rparen_){
                      static_cast<decltype(args)&&>(args)...};
              },
              std::move(rparen_args))),
          values_(std::move(values)), values_size_(values_size),
          value_poisoned_(value_poisoned)
    {}

    constexpr List(
        ely::TokenVariant<ely::token::LParen>&&                lparen,
        ely::TokenVariant<ely::token::RParen, MissingRParen>&& rparen,
        ely::Vector<stx::Sexpr>&&                              values,
        std::size_t                                            values_size,
        bool                                                   value_poisoned)
        : lparen_(std::move(lparen)), rparen_(std::move(rparen)),
          values_(std::move(values)), values_size_(values_size),
          value_poisoned_(value_poisoned)
    {}

    constexpr bool is_poison() const noexcept
    {
        return value_poisoned_ || lparen_.is_poison() || rparen_.is_poison();
    }

    constexpr std::size_t leading_size() const
    {
        return lparen_.leading_size();
    }

    constexpr std::size_t trailing_size() const
    {
        return rparen_.trailing_size();
    }

    constexpr std::size_t size() const
    {
        return lparen_.size() + rparen_.size() + values_size_;
    }

    // this is the span of the list including parentheses
    constexpr std::size_t inner_size() const
    {
        return lparen_.inner_size() + lparen_.trailing_size() +
               rparen_.inner_size() + rparen_.leading_size() + values_size_;
    }

    template<typename... Args>
    constexpr void emplace_back(Args&&... args);
};

class Literal : public ely::TokenVariant<token::IntLit,
                                         token::FloatLit,
                                         token::CharLit,
                                         token::StringLit,
                                         token::KeywordLit,
                                         token::BoolLit,
                                         token::UnterminatedStringLit>
{
private:
    using base_ = ely::TokenVariant<token::IntLit,
                                    token::FloatLit,
                                    token::CharLit,
                                    token::StringLit,
                                    token::KeywordLit,
                                    token::BoolLit,
                                    token::UnterminatedStringLit>;

public:
    using base_::base_;

    using base_::inner_size;
    using base_::is_poison;
    using base_::leading_size;
    using base_::size;
    using base_::trailing_size;
    using base_::visit;
    using base_::visit_all;
};

class Identifier
    : public ely::TokenVariant<token::Identifier, token::InvalidNumberSign>
{
private:
    using base_ =
        ely::TokenVariant<token::Identifier, token::InvalidNumberSign>;

public:
    template<typename T, typename... Args>
    constexpr Identifier(
        ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
        ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing,
        std::in_place_type_t<T>                             t,
        Args&&... args)
        : base_(std::move(leading),
                std::move(trailing),
                t,
                static_cast<Args&&>(args)...)
    {}

    using base_::inner_size;
    using base_::is_poison;
    using base_::leading_size;
    using base_::size;
    using base_::trailing_size;
    using base_::visit;
    using base_::visit_all;
};

class Var
{
private:
    // can be null
    std::unique_ptr<Sexpr> id_;
    // can be null
    std::unique_ptr<Sexpr>          ty_;
    ely::TokenVariant<token::Colon> colon_;

public:
    Var(ely::TokenVariant<token::Colon>&& colon,
        std::unique_ptr<Sexpr>&&          id,
        std::unique_ptr<Sexpr>&&          ty)
        : id_(std::move(id)), ty_(std::move(ty)), colon_(std::move(colon))
    {}

    bool is_poison() const noexcept
    {
        return !id_ || !ty_;
    }

    std::size_t leading_size() const noexcept;
    std::size_t trailing_size() const noexcept;
    std::size_t size() const noexcept;
    std::size_t inner_size() const noexcept;
};

class Eof final : public ely::TokenVariant<token::Eof>
{
private:
    using base_ = ely::TokenVariant<token::Eof>;

public:
    using base_::base_;

    constexpr bool is_poison() const noexcept
    {
        return false;
    }
};

// sexpr is the final syntax node without eof
class Sexpr final : public ely::Variant<List, Literal, Identifier, Var>
{
private:
    using base_ = ely::Variant<List, Literal, Identifier, Var>;

public:
    using base_::base_;

    constexpr bool is_list() const
    {
        return ely::holds_alternative<List>(*this);
    }

    constexpr bool is_literal() const
    {
        return ely::holds_alternative<Literal>(*this);
    }

    constexpr bool is_identifier() const
    {
        return ely::holds_alternative<Identifier>(*this);
    }

    constexpr bool is_poison() const
    {
        return ely::visit([](const auto& sexp) { return sexp.is_poison(); },
                          *this);
    }

    constexpr std::size_t size() const noexcept
    {
        return ely::visit([](const auto& stx) { return stx.size(); }, *this);
    }

    constexpr std::size_t leading_size() const noexcept
    {
        return ely::visit([](const auto& stx) { return stx.leading_size(); },
                          *this);
    }

    constexpr std::size_t trailing_size() const noexcept
    {
        return ely::visit([](const auto& stx) { return stx.trailing_size(); },
                          *this);
    }

    constexpr std::size_t inner_size() const noexcept
    {
        return ely::visit([](const auto& stx) { return stx.inner_size(); },
                          *this);
    }
};

class Syntax final : public ely::Variant<Sexpr, Eof>
{
private:
    using base_ = ely::Variant<Sexpr, Eof>;

public:
    using base_::base_;

    constexpr bool is_list() const
    {
        return ely::visit(
            [](const auto& x) {
                using ty = ely::remove_cvref_t<decltype(x)>;

                if constexpr (std::is_same_v<ty, Eof>)
                {
                    return false;
                }
                else
                {
                    return x.is_list();
                }
            },
            *this);
    }

    constexpr bool is_literal() const
    {
        return ely::visit(
            [](const auto& x) {
                using ty = ely::remove_cvref_t<decltype(x)>;

                if constexpr (std::is_same_v<ty, Eof>)
                {
                    return false;
                }
                else
                {
                    return x.is_literal();
                }
            },
            *this);
    }

    constexpr bool is_identifier() const
    {
        return ely::visit(
            [](const auto& x) {
                using ty = ely::remove_cvref_t<decltype(x)>;

                if constexpr (std::is_same_v<ty, Eof>)
                {
                    return false;
                }
                else
                {
                    return x.is_identifier();
                }
            },
            *this);
    }

    constexpr bool is_eof() const
    {
        return holds_alternative<Eof>(*this);
    }

    explicit constexpr operator bool() const noexcept
    {
        return !is_eof();
    }

    using base_::visit;

    constexpr bool is_poison() const noexcept
    {
        return ely::visit(
            [](const auto& x) {
                using ty = ely::remove_cvref_t<decltype(x)>;

                if constexpr (std::is_same_v<ty, Eof>)
                {
                    return false;
                }
                else
                {
                    return x.is_poison();
                }
            },
            *this);
    }

    constexpr std::size_t size() const noexcept
    {
        return ely::visit([](const auto& stx) { return stx.size(); }, *this);
    }

    constexpr std::size_t leading_size() const noexcept
    {
        return ely::visit([](const auto& stx) { return stx.leading_size(); },
                          *this);
    }

    constexpr std::size_t trailing_size() const noexcept
    {
        return ely::visit([](const auto& stx) { return stx.trailing_size(); },
                          *this);
    }

    constexpr std::size_t inner_size() const noexcept
    {
        return ely::visit([](const auto& stx) { return stx.inner_size(); },
                          *this);
    }
};

template<typename... Args>
constexpr void List::emplace_back(Args&&... args)
{
    stx::Sexpr& sexp = values_.emplace_back(static_cast<Args&&>(args)...);
    values_size_ += sexp.size();
}

std::size_t Var::leading_size() const noexcept
{
    if (id_)
    {
        return id_->leading_size();
    }

    return colon_.leading_size();
}

std::size_t Var::trailing_size() const noexcept
{
    if (ty_)
    {
        return ty_->trailing_size();
    }

    return colon_.trailing_size();
}

std::size_t Var::size() const noexcept
{
    std::size_t sz{};

    if (id_)
    {
        sz += id_->size();
    }

    sz += colon_.size();

    if (ty_)
    {
        sz += ty_->size();
    }

    return sz;
}

std::size_t Var::inner_size() const noexcept
{
    std::size_t sz{};

    if (id_)
    {
        sz += id_->inner_size();
        sz += id_->trailing_size();

        sz += colon_.leading_size();
    }

    sz += colon_.inner_size();

    if (ty_)
    {
        sz += ty_->leading_size();
        sz += ty_->inner_size();

        sz += colon_.trailing_size();
    }

    return sz;
}
} // namespace stx
} // namespace ely