#include <benchmark/benchmark.h>

#include <array>
#include <memory>
#include <string>

#include <ely/lex/scanner.hpp>

//#include <ely/lex/token.hpp>
//#include <ely/lex/tokenizer.hpp>

#include <ely/stx/tape.hpp>

//#include <ely/stx/reader.hpp>

constexpr int         lines       = 5;
constexpr std::size_t buffer_size = 1024;

std::string long_source()
{
    std::string src(
        R"r((def long:f64 5.0) ; this will be a very long source
(def (f x:u32) (* x x))
(def (laugh) #`(#,print "hahaha":Str))
(print #:out file '5.4)
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

        auto buf_it = tok_buf.begin();

        auto lexeme = stream.next();

        while (lexeme.kind != ely::lexeme::LexemeKind::Eof &&
               buf_it != tok_buf.end())
        {
            *buf_it = lexeme;
            benchmark::DoNotOptimize(*buf_it);
            lexeme = stream.next();
            ++buf_it;
        }

        auto buf_size = buf_it - tok_buf.begin();
    }

    state.SetBytesProcessed(src.size() * state.iterations());
    state.SetItemsProcessed(lines * state.iterations());
}
BENCHMARK(BM_scan_stream);

/*
static void BM_tokenizer(benchmark::State& state)
{
    auto src      = long_source();
    auto src_view = static_cast<std::string_view>(src);

    auto tok_buf = std::array<ely::Token, buffer_size>{};

    for (auto _ : state)
    {
        auto stream = ely::Tokenizer{src_view.begin(), src_view.end()};
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
BENCHMARK(BM_tokenizer);
*/
BENCHMARK_MAIN();
