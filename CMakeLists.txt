cmake_minimum_required(VERSION 3.8)

project(wmc LANGUAGES CXX)

set(IS_MASTER_PROJECT OFF)

if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
  set(IS_MASTER_PROJECT ON)
endif()

option(WMC_TESTS "Build unit tests" ${IS_MASTER_PROJECT})
option(WMC_EXAMPLES "Build examples" ${IS_MASTER_PROJECT})

if (WMC_EXAMPLES)
  add_subdirectory(examples)
endif()

add_library(wmc INTERFACE)
add_library(wmc::wmc ALIAS wmc)
target_compile_features(wmc INTERFACE cxx_std_20)

target_include_directories(
  wmc
  INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)