#pragma once

#include "ely/scanner.hpp"
#include "ely/token2.hpp"

namespace ely
{
template<typename I, typename S>
class TokenStream2
{
public:
    using value_type = Token2;
    using reference  = value_type;

private:
    ely::ScannerStream<I, S> scanner_;

public:
    TokenStream2() = default;

    constexpr TokenStream2(I it, S end)
        : scanner_(std::move(it), std::move(end))
    {}

    explicit constexpr TokenStream2(ScannerStream<I, S> scanner)
        : scanner_(std::move(scanner))
    {}

    reference next()
    {
        auto tok = scanner_.next();
        return ely::visit(
            [&](auto t) {
                // pass it twice to select the variant to construct and the
                // variant requires it as first arg
                return reference{t, t, tok.span};
            },
            tok.kind);
    }

private:
    ELY_ALWAYS_INLINE constexpr reference next_lexeme() noexcept
    {
        return scanner_.next();
    }
};
} // namespace ely