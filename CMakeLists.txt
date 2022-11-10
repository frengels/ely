cmake_minimum_required(VERSION 3.8)

project(Ely LANGUAGES C CXX)

set(IS_MASTER_PROJECT OFF)

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
  set(IS_MASTER_PROJECT ON)
endif()

option(ELY_TESTS "Build unit tests" ${IS_MASTER_PROJECT})
option(ELY_EXAMPLES "Build examples" ${IS_MASTER_PROJECT})

if (ELY_EXAMPLES)
  add_subdirectory(examples)
endif()

add_subdirectory(src)

add_library(ely INTERFACE)
target_include_directories(
  ely
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
target_compile_features(ely INTERFACE cxx_std_20)

if (ELY_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()