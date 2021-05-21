#include <benchmark/benchmark.h>

#include <array>
#include <memory>
#include <string>

#include <ely/lexer.h>
#include <ely/reader.h>
#include <ely/token.h>

constexpr int lines = 1000000;

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
        const char* src_p     = src.data();
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

static void BM_read(benchmark::State& state)
{
    auto src = long_source();
    // make sure we have enough space
    static constexpr std::size_t token_buf_len = 40000000;
    auto token_buf = std::make_unique<ElyToken[]>(token_buf_len);

    auto        src_len = src.size();
    const char* src_p   = src.data();

    auto lex_res = ely_lex(src_p, src_len, token_buf.get(), token_buf_len);
    assert(lex_res.bytes_processed == src_len);

    for (auto _ : state)
    {
        ElyReader reader;
        ely_reader_create(&reader, "hello_world.ely");

        ElyList nodes;
        ely_list_create(&nodes);

        ely_reader_read_all(
            &reader, src.data(), token_buf.get(), lex_res.tokens_read, &nodes);

        ElyNode *e, *tmp;
        ely_list_for_each_safe(e, tmp, &nodes, link)
        {
            ely_node_destroy(e);
        }
    }

    state.SetBytesProcessed(src_len * state.iterations());
    state.SetItemsProcessed(lines * state.iterations());
}
BENCHMARK(BM_read);

BENCHMARK_MAIN();