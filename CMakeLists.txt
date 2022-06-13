cmake_minimum_required(VERSION 3.12)
project(ely VERSION 0.1.0 LANGUAGES C CXX)

include(GNUInstallDirs)

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)