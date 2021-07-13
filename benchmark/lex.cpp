#include <benchmark/benchmark.h>

#include <array>
#include <memory>
#include <string>

#include "ely/lex/scanner.hpp"
#include "ely/token_stream2.hpp"
#include "ely/lex/token.hpp"

//#include <ely/parser.hpp>
// #include <ely/reader.hpp>
//#include <ely/tokenstream.hpp>

constexpr int         lines       = 5;
constexpr std::size_t buffer_size = 1024;

std::string long_source()
{
    std::string src(
        R"r((def long 5.0) ; this will be a very long source
(def (f x) (* x x))
(def (laugh) (print "hahaha"))
(print #:out file 5.4)
; just another comment about the code
)r");
    std::string res{src.begin(), src.end()};
    return res;
}

static void BM_scan_stream(benchmark::State& state)
{
    auto src      = long_source();
    auto src_view = static_cast<std::string_view>(src);

    auto tok_buf =
        std::array<ely::Lexeme<std::string_view::iterator>, buffer_size>{};

    for (auto _ : state)
    {
        auto stream = ely::ScannerStream{src_view.begin(), src_view.end()};

        while (true)
        {
            auto buf_it = tok_buf.begin();

            auto lexeme = stream.next();

            while (lexeme && buf_it != tok_buf.end())
            {
                *buf_it = lexeme;
                lexeme  = stream.next();
                ++buf_it;
            }

            auto buf_size = buf_it - tok_buf.begin();

            for (std::size_t i = 0; i != buf_size; ++i)
            {
                benchmark::DoNotOptimize(tok_buf[i]);
            }

            if (!lexeme)
            {
                break;
            }
        }
    }

    state.SetBytesProcessed(src.size() * state.iterations());
    state.SetItemsProcessed(lines * state.iterations());
}
BENCHMARK(BM_scan_stream);

static void BM_token_stream(benchmark::State& state)
{
    auto src      = long_source();
    auto src_view = static_cast<std::string_view>(src);

    auto tok_buf = std::array<ely::Token2, buffer_size>{};

    for (auto _ : state)
    {
        auto stream = ely::TokenStream2{src_view.begin(), src_view.end()};
        auto buf_it = tok_buf.begin();

        auto tok = stream.next();

        while (tok && buf_it != tok_buf.end())
        {
            *buf_it = std::move(tok);
            benchmark::DoNotOptimize(*buf_it);
            tok = stream.next();
            ++buf_it;
        }

        auto buf_size = buf_it - tok_buf.begin();
    }

    state.SetBytesProcessed(src.size() * state.iterations());
    state.SetItemsProcessed(lines * state.iterations());
}
BENCHMARK(BM_token_stream);

// static void BM_token_stream(benchmark::State& state)
// {
//     auto src      = long_source();
//     auto src_view = static_cast<std::string_view>(src);

//     auto tok_alloc = std::allocator<ely::Token>{};
//     auto tok_buf   = std::allocator_traits<decltype(tok_alloc)>::allocate(
//         tok_alloc, buffer_size);

//     for (auto _ : state)
//     {
//         auto stream = ely::TokenStream(src_view.begin(), src_view.end());

//         while (true)
//         {
//             auto buf_it = tok_buf;

//             auto tok = stream.next();

//             while (!ely::holds<ely::token::Eof>(tok) &&
//                    buf_it != tok_buf + buffer_size)
//             {
//                 std::allocator_traits<decltype(tok_alloc)>::construct(
//                     tok_alloc, buf_it, std::move(tok));
//                 tok = stream.next();
//                 ++buf_it;
//             }

//             auto buf_size = buf_it - tok_buf;

//             for (std::size_t i = 0; i != buf_size; ++i)
//             {
//                 benchmark::DoNotOptimize(tok_buf[i]);
//                 std::allocator_traits<decltype(tok_alloc)>::destroy(
//                     tok_alloc, tok_buf + i);
//             }

//             if (ely::holds<ely::token::Eof>(tok))
//             {
//                 break;
//             }
//         }
//     }

//     std::allocator_traits<decltype(tok_alloc)>::deallocate(
//         tok_alloc, tok_buf, buffer_size);

//     state.SetBytesProcessed(src.size() * state.iterations());
//     state.SetItemsProcessed(lines * state.iterations());
// }
// BENCHMARK(BM_token_stream);

// static void BM_read(benchmark::State& state)
// {
//     auto src      = long_source();
//     auto src_view = static_cast<std::string_view>(src);

//     for (auto _ : state)
//     {
//         auto reader = ely::Reader(src_view.begin(), src_view.end());

//         auto stx = reader.next();

//         while (stx)
//         {
//             stx = reader.next();
//         }
//     }

//     state.SetBytesProcessed(src.size() * state.iterations());
//     state.SetItemsProcessed(lines * state.iterations());
// }
// BENCHMARK(BM_read);

BENCHMARK_MAIN();
