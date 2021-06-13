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

    reference read(ely::token::IntLit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::FloatLit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::CharLit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::StringLit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::KeywordLit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::BoolLit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

    reference read(ely::token::UnterminatedStringLit,
                   ely::AtmosphereList<AtmospherePosition::Leading>&&  leading,
                   ely::AtmosphereList<AtmospherePosition::Trailing>&& trailing)
    {}

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