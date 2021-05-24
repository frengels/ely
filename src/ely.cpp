#include <array>
#include <iostream>
#include <iterator>
#include <span>

#include <ely/scanner.hpp>

void scan_stream(
    std::string_view                                   src,
    std::span<ely::Lexeme<std::string_view::iterator>> lexeme_buffer)
{
    auto scanner = ely::ScannerStream{src.begin(), src.end()};

    auto lexeme = scanner.next();
    auto out    = lexeme_buffer.begin();

    while (lexeme)
    {
        *out = lexeme;

        lexeme = scanner.next();
        ++out;
    }

    for (auto it = lexeme_buffer.begin(); it != out; ++it)
    {
        auto lexeme = *it;

        if (!ely::lexeme_is_atmosphere(lexeme.kind))
        {
            std::cout << std::string_view{lexeme.start, lexeme.size()} << '\n';
        }
    }
}

int main(int argc, char** argv)
{
    const char*                filename = "test";
    constexpr std::string_view src =
        "(def (f x) #:x)\n123.123\n123\n123.\n123fd\n13.34fds\n";

    std::cout << "source:\n" << src << '\n';
    std::array<ely::Lexeme<std::string_view::iterator>, 32> lexeme_buffer{};

    scan_stream(src, lexeme_buffer);
}