#pragma once

#include <ely/stx/stx.hpp>

namespace ely
{
template<typename I, typename S>
class Reader
{
public:
    using value_type  = ely::stx::Stx;
    using reference   = value_type;
    using stream_type = TokStream;
    using token_type  = typename stream_type::value_type;

    static_assert(std::is_same_v<token_type, ely::Token>,
                  "Stream does not give ely::Token as output");

private:
    I it_;
    S end_;

public:
    Reader() = default;

    explicit constexpr Reader(stream_type&& stream)
        : tok_stream_(std::move(stream))
    {}

    constexpr reference next()
    {
        return read_stx();
    }

private:
    constexpr ely::stx::Sexpr read_sexpr()
    {
        const token_type& tok = *it_;

        return ely::visit(
            [](const auto& x) -> stx::Sexpr {
                using tok_ty = ely::remove_cvref_t<decltype(x)>;

                if constexpr (std::is_same_v<tok_ty, ely::token::LParen>)
                {
                    ELY_UNIMPLEMENTED("Not implemented list reading");
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::RParen>)
                {
                    ELY_UNIMPLEMENTED("expected sexpr , got `)`");
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::LBrace>)
                {
                    ELY_UNIMPLEMENTED("read attribute list");
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::RBrace>)
                {
                    ELY_UNIMPLEMENTED("expected sexpr, got `}`");
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::LBracket>)
                {
                    ELY_UNIMPLEMENTED("read tuple");
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::RBracket>)
                {
                    ELY_UNIMPLEMENTED("expected sexpr, got `]`");
                }
                else if constexpr (std::is_same_v<tok_ty,
                                                  ely::token::Identifier>)
                {
                    ELY_UNIMPLEMENTED("");
                }
                else if constexpr (ely::is_same_one_of_v<
                                       tok_ty,
                                       ely::token::IntLit,
                                       ely::token::FloatLit,
                                       ely::token::CharLit,
                                       ely::token::StringLit,
                                       ely::token::KeywordLit,
                                       ely::token::BoolLit,
                                       ely::token::UnterminatedStringLit>)
                {
                    ELY_UNIMPLEMENTED("");
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::Colon>)
                {
                    ELY_UNIMPLEMENTED("incomplete var reading");
                }
                else if constexpr (std::is_same_v<tok_ty, ely::token::Eof>)
                {
                    ELY_UNIMPLEMENTED("Expected sexpr, got EOF");
                }
                else
                {
                    ELY_UNIMPLEMENTED("Expected sexpr, got ?");
                }
            },
            tok);
    }

    constexpr ely::stx::Stx read_stx()
    {
        const token_type& tok = *it_;

        ely::visit(
            [](const auto& x) {
                using tok_ty = ely::remove_cvref_t<decltype(x)>;
                if constexpr (std::is_same_v<tok_ty, ely::token::Eof>)
                {
                    return stx::Eof{it_++};
                }
                else
                {
                    ELY_MUSTTAIL return read_sexpr();
                }
            },
            tok);
    }
};
} // namespace ely