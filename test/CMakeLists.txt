set(ELY_TEST_FILES
    interner
    scope
    lexer
    nodes
    uniquer
    variant
    union_storage
)

function(make_test target)
    string(CONCAT target_src ${target} ".cpp")
    add_executable(${target} ${target_src})
    target_compile_options(${target} PRIVATE -fsanitize=address)
    target_link_options(${target} PRIVATE -fsanitize=address)
    target_compile_definitions(${target} PRIVATE ELY_DBG_VERBOSE=1)
    set_target_properties(${target} PROPERTIES ELY_PRIVATE ON)
    add_test(NAME ${target} COMMAND ${target})
endfunction()

foreach(t ${ELY_TEST_FILES})
    make_test(${t})
    target_link_libraries(${t} PRIVATE ely)
endforeach()

add_executable(lexer2 lexer.cpp)
target_compile_options(lexer2 PRIVATE -fsanitize=address)
target_link_options(lexer2 PRIVATE -fsanitize=address)
target_compile_definitions(lexer2 PRIVATE ELY_DBG_VERBOSE=1 NEW_LEXER)
set_target_properties(lexer2 PROPERTIES ELY_PRIVATE ON)
target_link_libraries(lexer2 PRIVATE ely)
add_test(NAME lexer2 COMMAND lexer2)
