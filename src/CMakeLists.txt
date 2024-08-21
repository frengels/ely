set(Ely_SRC
  main.cpp)

add_executable(ely ${Ely_SRC})

target_compile_features(ely PUBLIC cxx_std_20)