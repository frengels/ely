set(ELY_SRC
    ast.cpp
    runtime.cpp)

add_library(ely ${ELY_SRC})
target_include_directories(
    ely
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
