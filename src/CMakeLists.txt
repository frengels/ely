set(ELY_SRC
    lexer.c
    reader.c)

include(GenerateExportHeader)

add_library(ely SHARED ${ELY_SRC})
add_library(ely::ely ALIAS ely)
generate_export_header(ely)

target_include_directories(ely PUBLIC 
    $<BUILD_INTERFACE:${ely_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(ely
    PROPERTIES 
    EXPORT_NAME lexer
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden)