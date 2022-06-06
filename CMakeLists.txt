cmake_minimum_required(VERSION 3.12)
project(ely VERSION 0.1.0 LANGUAGES C)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/scan.c
    COMMENT "Building lexer with re2c."
    COMMAND re2c -8 ${CMAKE_CURRENT_SOURCE_DIR}/src/lex/scan.re
    -o ${CMAKE_CURRENT_BINARY_DIR}/scan.c
    -i --case-ranges --computed-gotos --bit-vectors # -W -Werror 
    DEPENDS 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lex/scan.re
        ${CMAKE_CURRENT_SOURCE_DIR}/src/lex/unicode_categories.re
    VERBATIM)

set(ELY_LEX_SRC
    "src/lex/lexer.c"
    ${CMAKE_CURRENT_BINARY_DIR}/scan.c)

add_library(ely_lex SHARED ${ELY_LEX_SRC})
target_include_directories(ely_lex PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lex)

set(ELY_SRC
    "src/main.c")

add_executable(ely ${ELY_SRC})
target_link_libraries(ely PRIVATE ely_lex)
target_include_directories(ely PRIVATE src/lex)
