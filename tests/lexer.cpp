#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <wmc/lexer.hpp>

template <auto F>
constexpr void lex_success(std::string_view strv, wmc::token_kind kind) {
  auto opt_res = F(strv);
  CHECK(opt_res);
  auto res = *opt_res;
  CHECK(res.kind == kind);
  CHECK(res.lexeme == strv);
}

template<auto F>
constexpr void lex_fail(std::string_view strv) {
  auto opt_res = F(strv);
  CHECK(!opt_res);
}

TEST_CASE("lexer") {
  SUBCASE("identifier") {
    SUBCASE("success") {
      lex_success<wmc::lex_identifier>("hello-there",
                                       wmc::token_kind::identifier);
    }
  }

  SUBCASE("string") {
    SUBCASE("success") {
      lex_success<wmc::lex_string>("\"hello world\"", wmc::token_kind::string_literal);
    }

    SUBCASE("fail") {
      lex_fail<wmc::lex_string>("hello");
    }
  }
}