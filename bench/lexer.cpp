#include <benchmark/benchmark.h>

#include <ely/stx/lexer.hpp>

#include "gen_src.hpp"

static constexpr auto MiB = 1024 * 1024;

auto file_10M = gen_src(10 * MiB);

static void BM_computed_goto_lexer_1M(benchmark::State& state) {
  auto file_1M = gen_src(MiB);
  auto out_buffer = std::make_unique<std::uint8_t[]>(MiB);
  for (auto _ : state) {
    ely::stx::lex(file_1M, {out_buffer.get(), MiB});
  }
}

static void BM_computed_goto_lexer_10M(benchmark::State& state) {
  auto file_1M = gen_src(10 * MiB);
  auto out_buffer = std::make_unique<std::uint8_t[]>(10 * MiB);
  for (auto _ : state) {
    ely::stx::lex(file_1M, {out_buffer.get(), 10 * MiB});
  }
}

BENCHMARK(BM_computed_goto_lexer_1M);
BENCHMARK(BM_computed_goto_lexer_10M);

BENCHMARK_MAIN();