#pragma once

#include "ely/stx.hpp"
#include "ely/token.hpp"
#include "ely/tokenstream.hpp"

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

    constexpr reference next()
    {
        return tok_stream_.next().visit_all(
            [&](auto&& tok, auto&& leading, auto&& trailing) {
                return read(
                    std::move(tok), std::move(leading), std::move(trailing));
            });
    }

private:
    reference read(ely::token::LParen,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::RParen,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::LBracket,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::RBracket,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::LBrace,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::RBrace,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::Identifier,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    template<typename Lit>
    reference
    read_lit(Lit&&                                               lit,
             ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
             ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {
        using lit_ty = std::remove_cvref_t<decltype(lit)>;
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

    reference read(ely::token::InvalidNumberSign,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::Eof,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}
};
} // namespace ely