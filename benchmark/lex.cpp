#include <benchmark/benchmark.h>

#include <array>
#include <string>

#include <ely/lexer.h>
#include <ely/token.h>

constexpr int lines = 10000;

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

    for (int i = 0; i < lines; i += 5)
    {
        res.insert(res.end(), src.begin(), src.end());
    }

    return res;
}

static void BM_lex(benchmark::State& state)
{
    auto src = long_source();

    auto tok_buf = std::array<ElyToken, 1024>{};

    for (auto _ : state)
    {
        auto        src_len   = src.size();
        const char* src_p       = src.data();
        ElyToken*   tok_buf_p = tok_buf.data();
        auto        buf_len   = tok_buf.size();
        while (true)
        {
            auto lex_res = ely_lex(src_p, src_len, tok_buf_p, buf_len);
            auto read    = lex_res.tokens_read;

            for (int i = 0; i != read; ++i)
            {
                benchmark::DoNotOptimize(tok_buf[i]);
            }

            src_p += lex_res.bytes_processed;
            src_len -= lex_res.bytes_processed;
            if (src_len == 0)
                break;
        }
        benchmark::ClobberMemory();
    }

    state.SetBytesProcessed(src.size() * state.iterations());
    state.SetItemsProcessed(lines * state.iterations());
}
BENCHMARK(BM_lex);

BENCHMARK_MAIN();