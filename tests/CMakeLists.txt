set(ELY_TESTS
    lexer)

find_package(doctest REQUIRED)

function(make_test target)
    string(CONCAT target_src ${target} ".c")
    add_executable(${target} ${target_src})
    add_test(NAME ${target} COMMAND ${target})
endfunction()

foreach(t ${ELY_TESTS})
    make_test(${t})
endforeach()

target_link_libraries(lexer PRIVATE ely_lex)