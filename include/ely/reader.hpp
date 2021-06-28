#pragma once

#include "ely/stx.hpp"
#include "ely/token.hpp"
#include "ely/tokenstream.hpp"
#include "ely/utility.hpp"

namespace ely
{
template<typename I, typename S>
class Reader
{
public:
    using value_type = ely::stx::Syntax;
    using reference  = value_type;

private:
    ely::TokenStream<I, S> tok_stream_;

public:
    constexpr Reader(I it, S end) : tok_stream_(std::move(it), std::move(end))
    {}

    explicit constexpr Reader(ely::TokenStream<I, S> stream)
        : tok_stream_(std::move(stream))
    {}

    reference next()
    {
        return tok_stream_.next().visit_all(
            [&](auto&& tok, auto&& leading, auto&& trailing) {
                return read(
                    std::move(tok), std::move(leading), std::move(trailing));
            });
    }

private:
    reference read(ely::token::LParen                                  lp,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        ely::Vector<stx::Syntax> values{};
        std::size_t              values_size{0};
        bool                     value_poisoned{false};

        std::optional<reference> res{};
        do
        {
            res = tok_stream_.next().visit_all([&](auto&& tok,
                                                   auto&& tok_leading,
                                                   auto&& tok_trailing)
                                                   -> std::optional<
                                                       stx::Syntax> {
                using tok_ty = ely::remove_cvref_t<decltype(tok)>;

                if constexpr (std::is_same_v<tok_ty, ely::token::RParen>)
                {
                    return stx::Syntax{
                        std::in_place_type<stx::List>,
                        std::forward_as_tuple(std::move(leading),
                                              std::move(trailing),
                                              std::in_place_type<token::LParen>,
                                              std::move(lp)),
                        std::forward_as_tuple(std::move(tok_leading),
                                              std::move(tok_trailing),
                                              std::in_place_type<token::RParen>,
                                              std::move(tok)),
                        std::move(values),
                        values_size,
                        value_poisoned};
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::RBracket>)
                {
                    ELY_UNIMPLEMENTED("unimplemented! expected `)`, got `]`");
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::RBrace>)
                {
                    ELY_UNIMPLEMENTED("unimplemented! expected `)`, got `}`");
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::Eof>)
                {
                    ELY_UNIMPLEMENTED("unimplemented! expected `)`, got EOF");
                }
                else
                {
                    static_assert(!std::is_const_v<decltype(tok)>);
                    values.emplace_back(read(std::move(tok),
                                             std::move(tok_leading),
                                             std::move(tok_trailing)));
                    return std::nullopt;
                }

                return std::nullopt;
            });
        } while (!res);

        return *std::move(res);
    }

    reference read(ely::token::RParen,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        ELY_UNIMPLEMENTED("TODO: produce error, `)` without preceding `(`");
    }

    reference read(ely::token::LBracket,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        ELY_UNIMPLEMENTED("TODO: read [...]");
    }

    reference read(ely::token::RBracket,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        ELY_UNIMPLEMENTED("TODO: produce error, `]` without preceding `[`");
    }

    reference read(ely::token::LBrace,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        ELY_UNIMPLEMENTED("TODO: read {...}");
    }

    reference read(ely::token::RBrace,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        ELY_UNIMPLEMENTED("TODO: produce error, `}` without preceding `{`");
    }

    reference read(ely::token::Identifier&&                            ident,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Syntax{std::in_place_type<stx::Identifier>,
                           std::move(leading),
                           std::move(trailing),
                           std::in_place_type<token::Identifier>,
                           std::move(ident)};
    }

    template<typename Lit>
    reference
    read_lit(Lit&&                                               lit,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        using lit_ty = ely::remove_cvref_t<decltype(lit)>;
        return stx::Syntax{std::in_place_type<stx::Literal>,
                           std::move(leading),
                           std::move(trailing),
                           std::in_place_type<lit_ty>,
                           static_cast<Lit&&>(lit)};
    }

    reference read(ely::token::IntLit&&                                int_lit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return read_lit(
            std::move(int_lit), std::move(leading), std::move(trailing));
    }

    reference read(ely::token::FloatLit&&                             float_lit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&& leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return read_lit(
            std::move(float_lit), std::move(leading), std::move(trailing));
    }

    reference read(ely::token::CharLit&&                               ch_lit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return read_lit(
            std::move(ch_lit), std::move(leading), std::move(trailing));
    }

    reference read(ely::token::StringLit&&                             str_lit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return read_lit(
            std::move(str_lit), std::move(leading), std::move(trailing));
    }

    reference read(ely::token::KeywordLit&&                            kw_lit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return read_lit(
            std::move(kw_lit), std::move(leading), std::move(trailing));
    }

    reference read(ely::token::BoolLit&&                               b_lit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return read_lit(
            std::move(b_lit), std::move(leading), std::move(trailing));
    }

    reference read(ely::token::UnterminatedStringLit&&                 str_lit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Syntax{std::in_place_type<stx::Literal>,
                           std::move(leading),
                           std::move(trailing),
                           std::in_place_type<token::UnterminatedStringLit>,
                           std::move(str_lit)};
    }

    reference read(ely::token::InvalidNumberSign&&                     num_sign,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Syntax{std::in_place_type<stx::Identifier>,
                           std::move(leading),
                           std::move(trailing),
                           std::in_place_type<token::InvalidNumberSign>,
                           std::move(num_sign)};
    }

    reference read(ely::token::Eof&&                                   eof,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        return stx::Syntax{std::in_place_type<stx::Eof>,
                           std::move(leading),
                           std::move(trailing),
                           std::move(eof)};
    }
};
} // namespace ely