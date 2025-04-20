cmake_minimum_required(VERSION 3.20)

project(Ely LANGUAGES CXX)

option(ELY_ENABLE_TESTS "Enable tests" ON)

include(FetchContent)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

set(SRC
  src/main.cpp)

add_library(ely INTERFACE)
target_include_directories(ely INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(ely INTERFACE fmt::fmt)
target_compile_features(ely INTERFACE cxx_std_20)
set_target_properties(ely PROPERTIES CXX_EXTENSIONS OFF)

add_executable(ely_exe ${SRC})
set_target_properties(ely_exe PROPERTIES OUTPUT_NAME ely)
target_link_libraries(ely_exe PRIVATE ely)

if(${ELY_ENABLE_TESTS})
  enable_testing()
  add_subdirectory(test)
endif()
