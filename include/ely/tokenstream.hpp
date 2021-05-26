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
        std::vector<Atmosphere> atmosphere_collector{};
        std::size_t             trailing_start = 0;

        Lexeme<I> lexeme;

        if (cache_.len != std::numeric_limits<uint32_t>::max())
        {
            if (lexeme_is_atmosphere(cache_.kind))
            {
                ++trailing_start;
                atmosphere_collector.emplace_back(cache_);
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
            ++trailing_start;

            atmosphere_collector.emplace_back(lexeme);
            lexeme = scanner_.next();
        }
    skip_leading_atmo:

        RawToken raw_tok = RawToken(lexeme);

        if (raw_tok.is_eof())
        {
            return Token(std::move(atmosphere_collector),
                         trailing_start,
                         std::move(raw_tok));
        }

        lexeme = scanner_.next();

        while (lexeme_is_atmosphere(lexeme.kind) &&
               lexeme_is_newline(lexeme.kind))
        {
            atmosphere_collector.emplace_back(lexeme);
            lexeme = scanner_.next();
        }

        cache_ = lexeme;

        return Token(std::move(atmosphere_collector),
                     trailing_start,
                     std::move(raw_tok));
    }
};
} // namespace ely
