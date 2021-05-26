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
    Lexeme<I>                cache_{{},
                     std::numeric_limits<uint32_t>::max(),
                     static_cast<LexemeKind>(0)};

public:
    TokenStream() = default;

    constexpr TokenStream(I it, S end) : scanner_(std::move(it), std::move(end))
    {}

    constexpr TokenStream(ely::ScannerStream<I, S> scanner)
        : scanner_(std::move(scanner))
    {}

    reference next()
    {
        std::vector<Atmosphere> leading_atmosphere{};
        std::vector<Atmosphere> trailing_atmosphere{};

        Lexeme<I> lexeme;

        if (cache_.len != std::numeric_limits<uint32_t>::max())
        {
            if (lexeme_is_atmosphere(cache_.kind))
            {
                leading_atmosphere.emplace_back(cache_);
                cache_.len = std::numeric_limits<uint32_t>::max();
            }
            else
            {
                lexeme     = cache_;
                cache_.len = std::numeric_limits<uint32_t>::max();
                goto skip_leading_atmo;
            }
        }

        lexeme = scanner_.next();

        while (lexeme && ely::lexeme_is_atmosphere(lexeme.kind))
        {
            leading_atmosphere.emplace_back(lexeme);
            lexeme = scanner_.next();
        }
    skip_leading_atmo:

        Lexeme<I> token_lexeme = lexeme;

        if (token_lexeme.kind == LexemeKind::Eof)
        {
            return Token(std::move(leading_atmosphere),
                         std::move(trailing_atmosphere),
                         std::move(token_lexeme));
        }

        lexeme = scanner_.next();

        while (lexeme_is_atmosphere(lexeme.kind) &&
               lexeme_is_newline(lexeme.kind))
        {
            trailing_atmosphere.emplace_back(lexeme);
            lexeme = scanner_.next();
        }

        cache_ = lexeme;

        return Token(std::move(leading_atmosphere),
                     std::move(trailing_atmosphere),
                     std::move(token_lexeme));
    }
};
} // namespace ely
