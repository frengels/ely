#pragma once

#include "ely/ast.hpp"
#include "ely/tokenstream.hpp"

namespace ely
{
template<typename I, typename S>
class Reader
{
private:
    TokenStream<I, S> tok_stream_;

public:
    Reader() = default;

    constexpr Reader(I it, S end) : tok_stream_(std::move(it), std::move(end))
    {}

    explicit constexpr Reader(ely::TokenStream<I, S> tok_stream)
        : tok_stream_(std::move(tok_stream))
    {}

    constexpr std::optional<ast::Syntax> next()
    {
        return std::nullopt;

        auto token = tok_stream_.next();

        return next_impl(std::move(token));
    }

private:
    constexpr std::optional<ast::Syntax> next_impl(ely::Token&& token)
    {

        return std::move(token).visit(
            [&](auto&& tok) -> std::optional<ast::Syntax> {
                using tok_ty = std::remove_cvref_t<decltype(tok)>;

                if constexpr (std::is_same_v<ely::token::LParen, tok_ty>)
                {
                    return read_list(std::move(tok), std::move(token));
                }
                else if constexpr (std::is_same_v<ely::token::Eof, tok_ty>)
                {
                    return std::nullopt;
                }
            });
    }

    template<typename Lit>
    constexpr ast::Syntax read_stx_lit(Lit&& lit, ely::Token&& tok)
    {
        using lit_ty = std::remove_cvref_t<decltype(lit)>;
        return ast::Syntax{std::in_place_type<ast::SyntaxLiteral>,
                           ast::LiteralContext{},
                           static_cast<Lit&&>(lit)};
    }

    constexpr ast::Syntax read(ely::token::IntLit&& int_lit, ely::Token&& tok)
    {
        return read_stx_lit(std::move(int_lit), std::move(tok));
    }

    constexpr ast::Syntax read(ely::token::FloatLit&& float_lit,
                               ely::Token&&           tok)
    {
        return read_stx_lit(std::move(float_lit), std::move(tok));
    }

    constexpr ast::Syntax read(ely::token::StringLit&& str_lit,
                               ely::Token&&            tok)
    {
        return read_stx_lit(std::move(str_lit), std::move(tok));
    }

    constexpr ast::Syntax read(ely::token::CharLit&& ch_lit, ely::Token&& tok)
    {
        return read_stx_lit(std::move(ch_lit), std::move(tok));
    }

    constexpr ast::Syntax read(ely::token::KeywordLit&& kw_lit,
                               ely::Token&&             tok)
    {
        return read_stx_lit(std::move(kw_lit), std::move(tok));
    }

    constexpr ast::Syntax read(ely::token::BoolLit&& b_lit, ely::Token&& tok)
    {
        return read_stx_lit(std::move(b_lit), std::move(tok));
    }

    constexpr ast::Syntax read(ely::token::LParen&&, ely::Token&& tok)
    {
        std::vector<ely::ast::Syntax> values{};
        auto                          tok_next = tok_stream_.next();

        while (true)
        {
            if (holds<ely::token::RParen>(tok_next))
            {
                auto list = ely::ast::SyntaxList(std::move(values));
                auto stx =
                    ely::ast::Syntax(std::move(list), ely::astLexicalContext{});
                return stx;
            }
            else if (holds<ely::token::Eof>(tok_next))
            {
                assert(false && "TODO: poison the syntax, missing `)`");
            }

            values.emplace_back(next_impl(std::move(tok_next)));

            tok_next = tok_stream_.next();
        }
    }
};
} // namespace ely