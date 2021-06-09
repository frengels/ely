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
    }
};
} // namespace ely