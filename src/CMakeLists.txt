set(ELY_SRC
    token.c
    lexer.c
    reader.c
    list.c)

include(GenerateExportHeader)

add_library(ely SHARED ${ELY_SRC})
add_library(ely::ely ALIAS ely)
generate_export_header(ely
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/ely/export.h)

target_include_directories(ely PUBLIC 
    $<BUILD_INTERFACE:${ely_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties(ely
    PROPERTIES 
    EXPORT_NAME lexer
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON) # only for c++
target_compile_options(ely PRIVATE
    $<BUILD_INTERFACE:$<$<C_COMPILER_ID:MSVC>:/W4 /WX>>
    $<BUILD_INTERFACE:$<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra>>)
target_compile_features(ely PRIVATE c_std_99)


add_executable(elyc main.c)
target_link_libraries(elyc PUBLIC ely)
