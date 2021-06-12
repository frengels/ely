#pragma once

#include <numeric>
#include <span>
#include <type_traits>
#include <utility>
#include <vector>

#include "ely/atmosphere.hpp"
#include "ely/scanner.hpp"
#include "ely/token.hpp"
#include "ely/variant.hpp"

namespace ely
{
template<typename I, typename S>
class TokenStream
{
public:
    using value_type = Token;
    using reference  = Token;

private:
    ely::ScannerStream<I, S> scanner_;
    std::optional<Lexeme<I>> cache_{};

public:
    TokenStream() = default;

    constexpr TokenStream(I it, S end) : scanner_(std::move(it), std::move(end))
    {}

    explicit constexpr TokenStream(ely::ScannerStream<I, S> scanner)
        : scanner_(std::move(scanner))
    {}

    reference next()
    {
        AtmosphereList<AtmospherePosition::Leading>  leading_atmosphere{};
        AtmosphereList<AtmospherePosition::Trailing> trailing_atmosphere{};

        Lexeme<I> lexeme = [&] {
            if (cache_)
            {
                return *cache_;
            }
            else
            {
                return scanner_.next();
            }
        }();

        while (leading_atmosphere.try_emplace_back(lexeme))
        {
            lexeme = scanner_.next();
        }

        Lexeme<I> token_lexeme = lexeme;

        lexeme = scanner_.next();

        while (trailing_atmosphere.try_emplace_back(lexeme))
        {
            lexeme = scanner_.next();
        }

        cache_ = std::optional<Lexeme<I>>(lexeme);

        return Token(std::move(leading_atmosphere),
                     std::move(trailing_atmosphere),
                     std::move(token_lexeme));
    }
};
} // namespace ely
