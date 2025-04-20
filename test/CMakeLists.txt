set(ELY_TEST_FILES
    arena
)

function(make_test target)
    string(CONCAT target_src ${target} ".cpp")
    add_executable(${target} ${target_src})
    add_test(NAME ${target} COMMAND ${target})
endfunction()

foreach(t ${ELY_TEST_FILES})
    make_test(${t})
    target_link_libraries(${t} PRIVATE ely)
endforeach()
