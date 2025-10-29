cmake_minimum_required(VERSION 3.20)

# Check if ely is built as a standalone project
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(ely LANGUAGES C CXX)
  set(ELY_STANDALONE_BUILD ON)
endif ()

option(ELY_ENABLE_TESTS "Enable tests" ON)
option(ELY_TEST_SANITIZE_ADDRESS "enable address sanitizer for tests" ON)

include(FetchContent)

find_package(fmt)

if(NOT fmt_FOUND)
  message(STATUS "couldn't find fmt, fetching from github")
  FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
  )
  FetchContent_MakeAvailable(fmt)
endif()

find_package(LLVM REQUIRED CONFIG)

set(SRC
  src/main.cpp)

add_library(ely INTERFACE)
target_include_directories(ely INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ely INTERFACE fmt::fmt LLVMSupport)
target_compile_features(ely INTERFACE cxx_std_23)
set_target_properties(ely PROPERTIES CXX_EXTENSIONS OFF)

add_executable(ely_exe ${SRC})
set_target_properties(ely_exe PROPERTIES OUTPUT_NAME ely)
target_link_libraries(ely_exe PRIVATE ely)

if(${ELY_ENABLE_TESTS})
  enable_testing()
  add_subdirectory(test)
endif()
