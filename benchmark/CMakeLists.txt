set(ELY_BENCH
    lex)

find_package(benchmark REQUIRED)

function(make_bench target)
    string(CONCAT target_src ${target} ".cpp")
    add_executable(${target} ${target_src})
    target_link_libraries(${target} PRIVATE ely::ely benchmark::benchmark)
    target_compile_features(${target} PRIVATE cxx_std_14)
endfunction()

foreach(b ${ELY_BENCH})
    make_bench(${b})
endforeach()