set(ELY_SRC
    "ilist.c"
    "printer.cpp"
    "lex/token.cpp"
    "sema/sema.cpp")

include(GenerateExportHeader)
include(CheckIPOSupported)

check_ipo_supported(RESULT IPO_FOUND OUTPUT IPO_ERROR)

find_package(fmt REQUIRED)

add_ely_library(ely ${ELY_SRC})
target_include_directories(ely PUBLIC
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/exports)
target_compile_features(ely PUBLIC cxx_std_20)
target_link_libraries(ely PUBLIC fmt::fmt)

set_target_properties(ely PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
    INTERPROCEDURAL_OPTIMIZATION ${IPO_FOUND})

generate_export_header(ely
    EXPORT_FILE_NAME "exports/ely/export.h")

# set(ELYC_SRC
# "main.cpp")

# add_executable(elyc ${ELYC_SRC})
# target_link_libraries(elyc PRIVATE ely)
# set_target_properties(elyc PROPERTIES
# INTERPROCEDURAL_OPTIMIZATION ${IPO_FOUND})