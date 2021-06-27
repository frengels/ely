set(ELY_TESTS
    ilist
    memory
    #union
    variant)

find_package(doctest REQUIRED)

function(make_test target)
    string(CONCAT target_src ${target} ".cpp")
    add_executable(${target} ${target_src})
    target_link_libraries(${target} PRIVATE Ely::Ely doctest::doctest)
    add_test(NAME ${target} COMMAND ${target})
endfunction()

add_executable(union union.cpp)
target_include_directories(union PRIVATE ${Ely_SOURCE_DIR}/include)
target_link_libraries(union PRIVATE doctest::doctest)
add_test(NAME union COMMAND union)
target_compile_features(union PRIVATE cxx_std_20)

foreach(t ${ELY_TESTS})
    make_test(${t})
endforeach()
