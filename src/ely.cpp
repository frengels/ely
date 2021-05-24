#include <array>
#include <iostream>
#include <iterator>

#include <ely/scanner.hpp>

template<typename I, typename S, typename Out>
constexpr Out copy(I first, S last, Out out)
{
    for (; first != last; ++first, ++out)
    {
        *out = *first;
    }

    return out;
}

int main(int argc, char** argv)
{
    const char*                filename = "test";
    constexpr std::string_view src =
        "(def (f x) #:x)\n123.123\n123\n123.\n123fd\n13.34fds\n";

    std::cout << "source:\n" << src << '\n';

    auto scanner = ely::Scanner{src.begin(), src.end()};

    std::array<decltype(scanner)::value_type, 32> lexeme_buffer{};

    auto buffer_end =
        copy(scanner.begin(), scanner.end(), lexeme_buffer.begin());

    std::cout << "read " << std::distance(lexeme_buffer.begin(), buffer_end)
              << " tokens\n";

    for (auto it = lexeme_buffer.begin(); it != buffer_end; ++it)
    {
        auto lexeme = *it;

        if (!ely::lexeme_is_atmosphere(lexeme.kind))
            std::cout << std::string_view{lexeme.start, lexeme.size()} << '\n';
    }
}