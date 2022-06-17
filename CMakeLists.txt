cmake_minimum_required(VERSION 3.12)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(ely)

    find_package(LLVM REQUIRED HINTS ${LLVM_CMAKE_PATH})
    list(APPEND CMAKE_MODULE_PATH ${LLVM_DIR})
    include(ChooseMSVCCRT)

    include(AddLLVM)
    include(HandleLLVMOptions)

    include_directories("${LLVM_BINARY_DIR}/include" "${LLVM_INCLUDE_DIR}")
    link_directories("${LLVM_LIBRARY_DIR}")

    set(ELY_BUILT_STANDALONE 1)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE)
    message(FATAL_ERROR "In-source builds are not allowed.")
endif()

set(ELY_VERSION_STRING "0.1.0")

include(AddEly)

include_directories(BEFORE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(tools)
