#pragma once

#include <optional>
#include <type_traits>

#include "ely/node.hpp"
#include "ely/tokenstream.hpp"

namespace ely
{
template<typename I, typename S>
class ParseStream
{
private:
    TokenStream<I, S>    tokens_;
    std::optional<Token> saved_{std::nullopt};

public:
    ParseStream() = default;

    constexpr ParseStream(I it, S end) : tokens_(std::move(it), std::move(end))
    {}

    constexpr ParseStream(TokenStream<I, S> tokens) : tokens_(std::move(tokens))
    {}

    constexpr Node next()
    {
        if (saved_)
        {
            return next_impl(*std::move(saved_));
        }
        else
        {
            return next_impl(tokens_.next());
        }
    }

private:
    constexpr Node next_impl(ely::Token&& tok)
    {
        return std::move(tok).visit([&](auto&& innert) -> Node {
            using ty = std::remove_cvref_t<decltype(innert)>;

            if constexpr (std::is_same_v<ty, ely::token::LParen>)
            {
                return parse_list<token::LParen, token::RParen>(
                    std::move(innert));
            }
            else if constexpr (std::is_same_v<ty, ely::token::LBracket>)
            {
                return parse_list<token::LBracket, token::RBracket>(
                    std::move(innert));
            }
            else if constexpr (std::is_same_v<ty, ely::token::LBrace>)
            {
                return parse_list<token::LBrace, token::RBrace>(
                    std::move(innert));
            }
            else if constexpr (std::is_same_v<ty, ely::token::RParen>)
            {
                // TODO poison the node
                ELY_UNIMPLEMENTED(
                    "give a proper error for ) without matching (");
            }
            else if constexpr (std::is_same_v<ty, ely::token::RBracket>)
            {
                ELY_UNIMPLEMENTED(
                    "give a proper error for ] without matching [");
            }
            else if constexpr (std::is_same_v<ty, ely::token::RBrace>)
            {
                ELY_UNIMPLEMENTED(
                    "give a proper error for } without matching {");
            }
            else
            {
                return Node(std::in_place_type<ty>, std::move(innert));
            }
        });
    }

    template<typename L, typename R>
    constexpr Node parse_list(L&& lparen)
    {
        std::list<Node> nodes;

        ely::Token t = tokens_.next();

        while (!holds<ely::token::RParen>(t))
        {
            nodes.emplace_back(next_impl(std::move(t)));
            t = tokens_.next();
        }

        ELY_ASSERT(holds<R>(t), "Should hold ) at this point");

        return Node(std::in_place_type<node::List<L, R>>,
                    std::move(nodes),
                    static_cast<L&&>(lparen),
                    unsafe_get<R>(std::move(t)));
    }
};
} // namespace ely