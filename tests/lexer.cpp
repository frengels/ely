#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <mli/lexer.hpp>

template <auto F>
constexpr void lex_success(std::string_view strv, mli::token_kind kind) {
  auto opt_res = F(strv);
  CHECK(opt_res);
  auto res = *opt_res;
  CHECK(res.kind == kind);
  CHECK(res.lexeme == strv);
}

template <auto F> constexpr void lex_fail(std::string_view strv) {
  auto opt_res = F(strv);
  CHECK(!opt_res);
}

TEST_CASE("lexer") {
  SUBCASE("identifier") {
    SUBCASE("success") {
      lex_success<mli::lex_identifier>("hello-there",
                                       mli::token_kind::identifier);
    }
  }

  SUBCASE("string") {
    SUBCASE("success") {
      lex_success<mli::lex_string>("\"hello world\"",
                                   mli::token_kind::string_literal);
    }

    SUBCASE("fail") { lex_fail<mli::lex_string>("hello"); }
  }

  SUBCASE("token") {
    auto src = std::string_view{"(hello world)"};
    auto lp_tok = mli::lex(src);
    src = src.substr(lp_tok.lexeme.size());
    auto hello_tok = mli::lex(src);
    src = src.substr(hello_tok.lexeme.size());
    auto ws_tok = mli::lex(src);
    src = src.substr(ws_tok.lexeme.size());
    auto world_tok = mli::lex(src);
    src = src.substr(world_tok.lexeme.size());
    auto rp_tok = mli::lex(src);

    CHECK(lp_tok.kind == mli::token_kind::lparen);
    CHECK(hello_tok.kind == mli::token_kind::identifier);
    CHECK(ws_tok.kind == mli::token_kind::atmosphere);
    CHECK(world_tok.kind == mli::token_kind::identifier);
    CHECK(rp_tok.kind == mli::token_kind::rparen);
  }
}

TEST_CASE("pos_lexer") {
  auto src = std::string_view{"hello\nworld 123\r\n()"};

  auto lex = mli::pos_lexer{src};
  auto hello_tok = lex.next();
  CHECK(hello_tok.kind == mli::token_kind::identifier);
  CHECK(hello_tok.lexeme.pos() == mli::source_position{1, 1});

  auto world_tok = lex.next();
  CHECK(world_tok.kind == mli::token_kind::identifier);
  CHECK(world_tok.lexeme.pos() == mli::source_position{2, 1});

  auto num_tok = lex.next();
  CHECK(num_tok.kind == mli::token_kind::integer_literal);
  CHECK(num_tok.lexeme.pos() == mli::source_position{2, 7});

  auto lp_tok = lex.next();
  CHECK(lp_tok.kind == mli::token_kind::lparen);
  CHECK(lp_tok.lexeme.pos() == mli::source_position{3, 1});

  auto rp_tok = lex.next();
  CHECK(rp_tok.kind == mli::token_kind::rparen);
  CHECK(rp_tok.lexeme.pos() == mli::source_position{3, 2});

  auto eof_tok = lex.next();
  CHECK(eof_tok.kind == mli::token_kind::eof);
}