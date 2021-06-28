set(ELY_SRC
    assert.cpp)

include(GenerateExportHeader)

add_library(Ely SHARED ${ELY_SRC})
add_library(Ely::Ely ALIAS Ely)
generate_export_header(Ely
    EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/ely/export.h)

target_include_directories(Ely PUBLIC 
    $<BUILD_INTERFACE:${Ely_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>)
set_target_properties(Ely
    PROPERTIES 
    EXPORT_NAME lexer
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON) # only for c++
target_compile_features(Ely PUBLIC cxx_std_17)

add_executable(elycpp ely.cpp)
target_link_libraries(elycpp PUBLIC Ely::Ely)
