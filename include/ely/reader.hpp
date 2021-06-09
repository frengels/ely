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

    constexpr ast::Syntax read(ely::token::StringLit str_lit, ely::Token tok)
    {

    }

    constexpr ast::Syntax read(ely::token::LParen, ely::Token tok)
    {
        std::vector<ely::ast::Syntax> values{};
        auto                          tok_next = tok_stream_.next();

        std::move(tok_next).visit([&](auto&& tok) {
            using tok_ty = std::remove_cvref_t<decltype(tok)>;

            if constexpr (std::is_same_v<ely::token::RParen, tok_ty>)
            {
                auto list = ely::ast::List(std::move(values));
                auto stx  = ely::ast::Syntax(std::move(list),
                                            ely::ast::LexicalContext{});
                return std::optional<ast::Syntax>(std::move(stx));
            }
            else
            {
                auto stx = read(std::move(tok), std::move(token));
                values.emplace_back(std::move(stx));
            }
        });
    }
};
} // namespace ely