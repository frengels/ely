#pragma once

#include "ely/stx.hpp"
#include "ely/token.hpp"
#include "ely/tokenstream.hpp"
#include "ely/utility.hpp"

namespace ely
{
namespace detail
{
template<typename T, typename... Us>
inline constexpr bool is_same_as_one_of_v = (std::is_same_v<T, Us> || ...);
}

template<typename I, typename S>
class Reader
{
public:
    using value_type = ely::stx::Syntax;
    using reference  = value_type;
    using token_type = typename TokenStream<I, S>::value_type;

private:
    ely::TokenStream<I, S>    tok_stream_;
    std::optional<token_type> token_lookahead_{};

public:
    constexpr Reader(I it, S end) : tok_stream_(std::move(it), std::move(end))
    {}

    explicit constexpr Reader(ely::TokenStream<I, S> stream)
        : tok_stream_(std::move(stream))
    {}

    reference next()
    {
        token_type tok = next_token();

        return next_impl(std::move(tok));
    }

private:
    // get the next token, looking in the lookahead first
    constexpr token_type next_token()
    {
        if (token_lookahead_)
        {
            token_type res = *std::move(token_lookahead_);
            token_lookahead_.reset();
            return res;
        }

        return tok_stream_.next();
    }

    reference next_impl(token_type&& tok)
    {
        return std::move(tok).visit_all(
            [&](auto&& tok, auto&& leading, auto&& trailing) -> reference {
                return read_stx(
                    std::move(tok), std::move(leading), std::move(trailing));
            });
    }

    template<typename Tok>
    reference
    next_unwrapped(std::optional<ely::stx::Syntax>&&                   opt_stx,
                   Tok&&                                               t,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        if (opt_stx)
        {
            return read(*std::move(opt_stx),
                        static_cast<Tok&&>(t),
                        std::move(leading),
                        std::move(trailing));
        }
        else
        {
            return read(
                static_cast<Tok&&>(t), std::move(leading), std::move(trailing));
        }
    }

    stx::Var
    read_var(std::unique_ptr<stx::Sexpr>&&                       sexp,
             ely::token::Colon&&                                 c,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        token_type tok = next_token();

        auto ty_sexp = std::move(tok).visit_all(
            [&](auto&& t,
                auto&& leading,
                auto&& trailing) -> std::unique_ptr<stx::Sexpr> {
                using tok_ty = ely::remove_cvref_t<decltype(t)>;

                if constexpr (std::is_same_v<tok_ty, ely::token::Eof>)
                {
                    ELY_UNIMPLEMENTED("Expected sexpr, got EOF");
                    token_lookahead_ = std::move(tok);
                    // TODO: make a proper error rather than just returning an
                    // empty ptr
                    return std::unique_ptr<stx::Sexpr>{};
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::RParen>)
                {
                    ELY_UNIMPLEMENTED("Expected sexpr, got `)`");
                    token_lookahead_ = std::move(tok);
                    return std::unique_ptr<stx::Sexpr>{};
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::RBrace>)
                {
                    ELY_UNIMPLEMENTED("Expected sexpr, got `}`");
                    token_lookahead_ = std::move(tok);
                    return std::unique_ptr<stx::Sexpr>{};
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::RBracket>)
                {
                    ELY_UNIMPLEMENTED("Expected sexpr, got `]`");
                    token_lookahead_ = std::move(tok);
                    return std::unique_ptr<stx::Sexpr>{};
                }
                else
                {
                    return std::make_unique<stx::Sexpr>(read_sexpr(
                        static_cast<decltype(t)&&>(t),
                        static_cast<decltype(leading)&&>(leading),
                        static_cast<decltype(trailing)&&>(trailing)));
                }
            });

        return stx::Var{{std::move(leading), std::move(trailing), std::move(c)},
                        std::move(sexp),
                        std::move(ty_sexp)};
    }

    stx::Var
    read_var(stx::Sexpr&&                                        sexp,
             ely::token::Colon&&                                 c,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return read_var(std::make_unique<stx::Sexpr>(std::move(sexp)),
                        std::move(c),
                        std::move(leading),
                        std::move(trailing));
    }

    stx::Var read_incomplete_var(
        ely::token::Colon&&                                 c,
        ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
        ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return read_var(std::unique_ptr<stx::Sexpr>{},
                        std::move(c),
                        std::move(leading),
                        std::move(trailing));
    }

    stx::Literal
    read_lit(ely::token::IntLit&&                                ilit,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Literal(std::move(leading),
                            std::move(trailing),
                            std::in_place_type<ely::token::IntLit>,
                            std::move(ilit));
    }

    stx::Literal
    read_lit(ely::token::FloatLit&&                              flit,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Literal(std::move(leading),
                            std::move(trailing),
                            std::in_place_type<ely::token::FloatLit>,
                            std::move(flit));
    }

    stx::Literal
    read_lit(ely::token::CharLit&&                               clit,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Literal(std::move(leading),
                            std::move(trailing),
                            std::in_place_type<ely::token::CharLit>,
                            std::move(clit));
    }

    stx::Literal
    read_lit(ely::token::StringLit&&                             str_lit,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Literal(std::move(leading),
                            std::move(trailing),
                            std::in_place_type<ely::token::StringLit>,
                            std::move(str_lit));
    }

    stx::Literal
    read_lit(ely::token::KeywordLit&&                            kw_lit,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Literal(std::move(leading),
                            std::move(trailing),
                            std::in_place_type<ely::token::KeywordLit>,
                            std::move(kw_lit));
    }

    stx::Literal
    read_lit(ely::token::BoolLit&&                               blit,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Literal(std::move(leading),
                            std::move(trailing),
                            std::in_place_type<ely::token::BoolLit>,
                            std::move(blit));
    }

    stx::Literal
    read_lit(ely::token::UnterminatedStringLit&&                 ustr_lit,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Literal(
            std::move(leading),
            std::move(trailing),
            std::in_place_type<ely::token::UnterminatedStringLit>,
            std::move(ustr_lit));
    }

    stx::List
    read_list(ely::token::LParen&&                                lp,
              ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
              ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        ely::Vector<stx::Sexpr> values{};
        std::size_t             values_size{};
        bool                    value_poisoned{false};

        std::optional<stx::List> res{};
        do
        {
            res = tok_stream_.next().visit_all([&](auto&& tok,
                                                   auto&& tok_leading,
                                                   auto&& tok_trailing)
                                                   -> std::optional<stx::List> {
                using tok_ty = ely::remove_cvref_t<decltype(tok)>;

                if constexpr (std::is_same_v<tok_ty, ely::token::RParen>)
                {
                    return std::optional<stx::List>{stx::List{
                        {std::move(leading), std::move(trailing)},
                        {std::move(tok_leading), std::move(tok_trailing)},
                        std::move(values),
                        values_size,
                        value_poisoned}};
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::RBracket>)
                {
                    ELY_UNIMPLEMENTED("expected `)`, got `]`");
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::RBrace>)
                {
                    ELY_UNIMPLEMENTED("expected `)`, got `}`");
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::Eof>)
                {
                    ELY_UNIMPLEMENTED("expected `)`, got EOF");
                }
                else
                {
                    values.emplace_back(read_sexpr(std::move(tok),
                                                   std::move(tok_leading),
                                                   std::move(tok_trailing)));
                }

                return std::nullopt;
            });
        } while (!res);

        return *std::move(res);
    }

    template<typename T>
    std::enable_if_t<detail::is_same_as_one_of_v<ely::remove_cvref_t<T>,
                                                 token::Identifier,
                                                 token::InvalidNumberSign>,
                     stx::Identifier>
    read_identifier(
        T&&                                                 id,
        ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
        ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Identifier{std::move(leading),
                               std::move(trailing),
                               std::in_place_type<ely::remove_cvref_t<T>>,
                               static_cast<T&&>(id)};
    }

    template<typename Tok>
    stx::Sexpr
    read_sexpr(Tok&&                                               t,
               ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
               ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        using tok_ty = ely::remove_cvref_t<Tok>;

        auto sexp = [&]() -> stx::Sexpr {
            if constexpr (std::is_same_v<tok_ty, ely::token::LParen>)
            {
                return read_list(static_cast<Tok&&>(t),
                                 std::move(leading),
                                 std::move(trailing));
            }
            else if constexpr (std::is_same_v<tok_ty, ely::token::RParen>)
            {
                ELY_UNIMPLEMENTED("expected sexpr , got `)`");
            }
            else if constexpr (std::is_same_v<tok_ty, ely::token::LBrace>)
            {
                ELY_UNIMPLEMENTED("read attribute list");
                // return read_attribute_list(static_cast<Tok&&>(t),
                //                            std::move(leading),
                //                            std::move(trailing));
            }
            else if constexpr (std::is_same_v<tok_ty, ely::token::RBrace>)
            {
                ELY_UNIMPLEMENTED("expected sexpr, got `}`");
            }
            else if constexpr (std::is_same_v<tok_ty, ely::token::LBracket>)
            {
                ELY_UNIMPLEMENTED("read tuple");
                // return read_tuple(static_cast<Tok&&>(t),
                //                   std::move(leading),
                //                   std::move(trailing));
            }
            else if constexpr (std::is_same_v<tok_ty, ely::token::RBracket>)
            {
                ELY_UNIMPLEMENTED("expected sexpr, got `]`");
            }
            else if constexpr (detail::is_same_as_one_of_v<
                                   tok_ty,
                                   ely::token::Identifier,
                                   ely::token::InvalidNumberSign>)
            {
                return read_identifier(static_cast<Tok&&>(t),
                                       std::move(leading),
                                       std::move(trailing));
            }
            else if constexpr (detail::is_same_as_one_of_v<
                                   tok_ty,
                                   ely::token::IntLit,
                                   ely::token::FloatLit,
                                   ely::token::CharLit,
                                   ely::token::StringLit,
                                   ely::token::KeywordLit,
                                   ely::token::BoolLit,
                                   ely::token::UnterminatedStringLit>)
            {
                return read_lit(static_cast<Tok&&>(t),
                                std::move(leading),
                                std::move(trailing));
            }
            else if constexpr (std::is_same_v<tok_ty, ely::token::Colon>)
            {
                return read_incomplete_var(static_cast<Tok&&>(t),
                                           std::move(leading),
                                           std::move(trailing));
            }
            else if constexpr (std::is_same_v<tok_ty, ely::token::Eof>)
            {
                ELY_UNIMPLEMENTED("Expected sexpr, got EOF");
            }
            else
            {
                ELY_UNIMPLEMENTED("Expected sexpr, got ?");
            }
        }();

        // at this point we have a sexp, but possibly also an unconsumed token
        // from no valid token after `:`
        token_type tok_ = next_token();

        return std::move(tok_).visit_all([&](auto&& tok,
                                             auto&& leading,
                                             auto&& trailing) -> stx::Sexpr {
            using tok2_ty = ely::remove_cvref_t<decltype(tok)>;
            if constexpr (std::is_same_v<tok2_ty, ely::token::Colon>)
            {
                return read_var(std::move(sexp),
                                static_cast<decltype(tok)&&>(tok),
                                static_cast<decltype(leading)&&>(leading),
                                static_cast<decltype(trailing)&&>(trailing));
            }
            else
            {
                return std::move(sexp);
            }
        });
    }

    template<typename Tok>
    stx::Syntax
    read_stx(Tok&&                                               t,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        using tok_ty = ely::remove_cvref_t<Tok>;

        if constexpr (std::is_same_v<tok_ty, ely::token::Eof>)
        {
            return stx::Eof{
                std::move(leading), std::move(trailing), static_cast<Tok&&>(t)};
        }
        else
        {
            ELY_MUSTTAIL return read_sexpr(
                static_cast<Tok&&>(t), std::move(leading), std::move(trailing));
        }
    }
};
} // namespace ely