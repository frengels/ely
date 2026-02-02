set(ELY_TEST_FILES
    arena
    interner
    stream
    scope
    lexer
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
