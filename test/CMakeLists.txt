set(ELY_TESTS
    ilist
    memory)

set(TESTS
    union
    variant
    pair)

find_package(doctest REQUIRED)

function(make_test target)
    string(CONCAT target_src ${target} ".cpp")
    add_executable(${target} ${target_src})
    target_link_libraries(${target} PRIVATE doctest::doctest)
    target_include_directories(${target} PRIVATE ${Ely_SOURCE_DIR}/include)
    target_compile_features(${target} PRIVATE cxx_std_17)
    add_test(NAME ${target} COMMAND ${target})
endfunction()

function(make_ely_test target)
    make_test(${target})
    target_link_libraries(${target} PRIVATE Ely::Ely)
endfunction()

foreach(t ${TESTS})
    make_test(${t})
endforeach()

foreach(t ${ELY_TESTS})
    make_ely_test(${t})
endforeach()
