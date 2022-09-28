cmake_minimum_required(VERSION 3.8)

project(mlisp LANGUAGES CXX)

set(IS_MASTER_PROJECT OFF)

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
  set(IS_MASTER_PROJECT ON)
endif()

option(MLI_TESTS "Build unit tests" ${IS_MASTER_PROJECT})
option(MLI_EXAMPLES "Build examples" ${IS_MASTER_PROJECT})

if (MLI_EXAMPLES)
  add_subdirectory(examples)
endif()

add_library(mli INTERFACE)
add_library(mli::mli ALIAS mli)
target_compile_features(mli INTERFACE cxx_std_20)

target_include_directories(
  mli
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

if (MLI_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()