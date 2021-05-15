project(ely)
cmake_minimum_required(VERSION 3.11)

set(IS_MASTER_PROJECT OFF)

if(${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(IS_MASTER_PROJECT ON)
endif()

option(ELY_TESTS "enabled Ely's unit tests" ${IS_MASTER_PROJECT})

add_subdirectory(src)

if(${ELY_TESTS})
    enable_testing()
    add_subdirectory(test)
endif()