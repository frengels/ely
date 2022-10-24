set(ELY_SRC
    string.cpp
    node.cpp
    runtime.cpp
    type.cpp
    context.cpp)

add_library(ely ${ELY_SRC})
target_include_directories(
    ely
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
