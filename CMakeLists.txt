cmake_minimum_required(VERSION 3.20)

# Check if ely is built as a standalone project
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(ely LANGUAGES CXX)
  set(ELY_STANDALONE_BUILD ON)
endif ()

option(ELY_ENABLE_TESTS "Enable tests" ON)
option(ELY_TEST_SANITIZE_ADDRESS "enable address sanitizer for tests" ON)
option(ELY_ENABLE_BENCHMARKS "Enable benchmarks" ON)

include(FetchContent)

find_package(Boost COMPONENTS program_options REQUIRED)
find_package(fmt)

if(NOT fmt_FOUND)
  message(STATUS "couldn't find fmt, fetching from github")
  FetchContent_Declare(fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG master
  )
  FetchContent_MakeAvailable(fmt)
endif()

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)

set(SRC
  src/main.cpp)


add_library(ely INTERFACE)
target_include_directories(ely INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:$<$<BOOL:$<TARGET_PROPERTY:ELY_PRIVATE>>:${CMAKE_CURRENT_SOURCE_DIR}/src>>
  $<INSTALL_INTERFACE:include>)
target_link_libraries(ely INTERFACE fmt::fmt Boost::program_options)
target_compile_features(ely INTERFACE cxx_std_26)
set_target_properties(ely PROPERTIES CXX_EXTENSIONS ON)

add_executable(ely_exe ${SRC})
set_target_properties(ely_exe PROPERTIES OUTPUT_NAME ely)
target_link_libraries(ely_exe PRIVATE ely)

if(ipo_supported)
  message(STATUS "LTO enabled")
  set_target_properties(ely PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
  set_target_properties(ely_exe PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(WARNING "IPO is not supported: ${ipo_error}")
endif()

if(${ELY_ENABLE_TESTS})
  enable_testing()
  add_subdirectory(test)
endif()

if(${ELY_ENABLE_BENCHMARKS})
  add_subdirectory(bench)
endif()
