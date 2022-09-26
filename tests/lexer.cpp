#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <wmc/lexer.hpp>

TEST_CASE("lexer") {
  SUBCASE("string") {
    SUBCASE("success") {
      auto src = std::string_view{"\"hello world\""};

      auto opt_res = wmc::lex_string(src);
      CHECK(opt_res);
      auto res = *opt_res;
      CHECK(res.lexeme == src);
    }

    SUBCASE("fail") {
        auto src = std::string_view{"hello"};

        auto opt_res = wmc::lex_string(src);
        CHECK(!opt_res);
    }
  }
}