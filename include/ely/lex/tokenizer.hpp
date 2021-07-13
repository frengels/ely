#pragma once

#include "ely/lex/scanner.hpp"
#include "ely/lex/token.hpp"

namespace ely
{
template<typename I, typename S>
class Tokenizer
{
public:
    using value_type = Token;
    using reference  = value_type;

private:
    ely::ScannerStream<I, S> scanner_;

public:
    Tokenizer() = default;

    constexpr Tokenizer(I it, S end) : scanner_(std::move(it), std::move(end))
    {}

    explicit constexpr Tokenizer(ScannerStream<I, S> scanner)
        : scanner_(std::move(scanner))
    {}

    reference next()
    {
        return reference(scanner_.next());
    }
};
} // namespace ely