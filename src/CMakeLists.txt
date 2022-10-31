set(ELY_SRC
    arena.cpp
    string.cpp
    node.cpp
    primitive.cpp
    runtime.cpp
    context.cpp
    scope.cpp
    value.cpp)

find_package(Boost)

add_library(ely ${ELY_SRC})
target_include_directories(
    ely
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
target_link_libraries(ely PRIVATE Boost::boost)
