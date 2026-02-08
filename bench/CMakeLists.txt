find_package(benchmark REQUIRED)

add_executable(bench_lexer lexer.cpp)

target_link_libraries(bench_lexer PRIVATE ely benchmark::benchmark)
