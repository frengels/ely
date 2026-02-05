add_executable(gen_file gen_file.cpp)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex_1M.ely
  COMMAND gen_file 1024 > ${CMAKE_CURRENT_BINARY_DIR}/lex_1M.ely
  DEPENDS gen_file
  COMMENT "Generating 1MB file for lexer tests"
)

add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/lex_10M.ely
  COMMAND gen_file 10240 > ${CMAKE_CURRENT_BINARY_DIR}/lex_10M.ely
  DEPENDS gen_file
  COMMENT "Generating 10MB file for lexer tests"
)

add_custom_target(generate_inputs ALL
  DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/lex_1M.ely
          ${CMAKE_CURRENT_BINARY_DIR}/lex_10M.ely
  COMMENT "Generating input files for tests"
)