cmake_minimum_required(VERSION 3.12)
project(ely VERSION 0.1.0 LANGUAGES C)

set(ELY_LEX_SRC
    "src/lex/lexer.c"
    "src/lex/token.c")

add_library(ely_lex SHARED ${ELY_LEX_SRC})
target_include_directories(ely_lex PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(ELY_STX_SRC
    "src/stx/literal.c"
    "src/stx/list.c"
    "src/stx/identifier.c"
    "src/stx/parser.c")

add_library(ely_stx SHARED ${ELY_STX_SRC})
target_include_directories(ely_stx PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(ELY_SRC
    "src/main.c")

add_executable(ely ${ELY_SRC})
target_link_libraries(ely PRIVATE ely_lex ely_stx)
target_include_directories(ely PRIVATE src/lex)

enable_testing()
add_subdirectory(tests)