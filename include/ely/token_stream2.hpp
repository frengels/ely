#pragma once

#include "ely/lex/scanner.hpp"
#include "ely/lex/token.hpp"

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
        return reference(scanner_.next());
    }
};
} // namespace ely