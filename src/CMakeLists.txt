set(LEX_SRC
    lexer.c
    reader.c)

add_library(elyc_lexer STATIC ${LEX_SRC})
add_library(elyc::lexer ALIAS elyc_lexer)

target_include_directories(elyc_lexer PUBLIC 
    $<BUILD_INTERFACE:${elyc_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)
set_target_properties(elyc_lexer PROPERTIES EXPORT_NAME lexer)