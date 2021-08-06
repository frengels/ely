#pragma once

#include <utility>

namespace ely
{
namespace stx2
{
template<typename Stream>
class TrailingAtmosphere
{
public:
    using token_type = typename Stream::value_type;

private:
    token_type tok_;
    Stream     tok_stream_;

public:
    constexpr void skip() const&&
    {}

    template<typename F>
    constexpr std::pair<F, LeadingAtmosphere<Stream>> consume_with(F fn) const&&
    {
        if (lexeme_kind_is_leading_atmosphere(tok_.kind))
        {
            fn(tok_);

            auto tok = tok_stream_.next();
        }
    }
};

template<typename Stream, typename Stx>
class Body
{
    using token_type = typename Stream::value_type;

private:
    token_type tok_;
    Stream     tok_stream_;

public:
    constexpr Body(token_type&& tok, Stream&& stream)
        : tok_(std::move(tok)), tok_stream_(std::move(stream))
    {}

    template<typename F>
    constexpr std::pair<F, TrailingAtmosphere<Stream>>
    consume_with(F fn) const&&
    {}
};

template<typename Stream>
class LeadingAtmosphere
{
public:
    using token_type = typename Stream::value_type;

private:
    token_type tok_;
    Stream     tok_stream_;

public:
    constexpr void skip() const&&
    {}

    template<typename F>
    constexpr std::pair<F, Body<ely::stx2::Stx>> consume_with(F fn) const&&
    {
        if (lexeme_kind_is_leading_atmosphere(tok_.kind))
        {
            fn(tok_);

            auto tok = tok_stream_.next();

            while (lexeme_kind_is_leading_atmosphere(tok.kind))
            {
                fn(tok);
                tok = tok_stream_.next();
            }

            return {std::move(fn), {std::move(tok), std::move(tok_stream_)}};
        }
        else
        {
            return {std::move(fn), {std::move(tok_), std::move(tok_stream_)}};
        }
    };
} // namespace stx2
} // namespace ely