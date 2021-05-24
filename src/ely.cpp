#include <array>
#include <iostream>
#include <iterator>
#include <span>

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

void scan_iterator(
    std::string_view                                   src,
    std::span<ely::Lexeme<std::string_view::iterator>> lexeme_buffer)
{
    auto scanner = ely::Scanner{src.begin(), src.end()};

    auto buffer_end =
        copy(scanner.begin(), scanner.end(), lexeme_buffer.begin());

    std::cout << "read " << std::distance(lexeme_buffer.begin(), buffer_end)
              << " tokens\n";

    for (auto it = lexeme_buffer.begin(); it != buffer_end; ++it)
    {
        auto lexeme = *it;

        if (!ely::lexeme_is_atmosphere(lexeme.kind))
        {
            std::cout << std::string_view{lexeme.start, lexeme.size()} << '\n';
        }
    }
}

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