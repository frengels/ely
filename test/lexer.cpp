#include <ely/stx/lexer.hpp>

#include <cassert>
#include <string_view>

#include <fmt/base.h>

#include "ely/stx/tokens.h"
// testing internal encoding api as well
#include "stx/cont.h"
#include "stx/encode.h"

#include "support.hpp"

template <std::size_t N>
constexpr std::string_view make_src(const char (&cstr)[N]) {
  return std::string_view{cstr, N}; // include trailing \0
}

template <std::size_t N>
constexpr std::string_view make_block(const char (&cstr)[N]) {
  return std::string_view{cstr, N - 1};
}

bool check_equal(const std::uint8_t* got, const std::uint8_t* expected,
                 std::size_t length) {
  for (std::size_t i = 0; i != length; ++i) {
    if (got[i] != expected[i]) {
      fmt::print("mismatch at {}: got {}, expected {}\n", i, got[i],
                 expected[i]);
      return false;
    }
  }
  return true;
}

constexpr int lexer() {
  std::string_view spaces = make_src("    ");
  std::string_view tabs = make_src("\t\t\t\t");

  std::string_view spaces_block = make_block("    ");
  std::string_view tabs_block = make_block("\t\t\t\t");

  std::uint8_t buffer[1024];
  std::uint8_t expected[1024];

  const char terminate = '\0';

  {
    {
      auto expected_len = encode_whitespace(expected, 4);
      expected_len += encode_eof(expected + expected_len);

      auto res = ely::stx::lex(spaces, buffer);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));
    }
    {
      auto expected_len = encode_tab(expected, 4);
      expected_len += encode_eof(expected + expected_len);

      auto res = ely::stx::lex(tabs, buffer);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));
    }

    {
      // can it handle empty buffer?
      auto res = ely::stx::lex(spaces, {});
      assert(res == 0); // this is the only case that can return 0
    }

    {
      auto expected_len = encode_spill(expected, 0, CONT_START);
      // what about an empty src block
      auto res = ely::stx::lex({}, buffer);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));

      expected_len = encode_spill(expected, 4, CONT_WHITESPACE);
      // let's refill and continue, but again with an unfinished block
      res = ely::stx::lex(spaces_block, buffer, buffer[1]);

      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));

      // use these for testing a different sequence later on
      auto len = buffer[0];
      auto cont = buffer[1];

      expected_len = encode_whitespace(expected, 4);
      expected_len += encode_eof(expected + expected_len);
      // finally, finish it
      res = ely::stx::lex(spaces, buffer, cont);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));

      expected_len = encode_whitespace(expected, 0);
      expected_len += encode_tab(expected + expected_len, 4);
      expected_len += encode_eof(expected + expected_len);
      // finish with tabs now
      res = ely::stx::lex(tabs, buffer, cont);
      assert(res == expected_len);
    }
    {
      auto id = make_src("hello_world");
      auto expected_len = encode_identifier(expected, id.size() - 1); // for \0
      expected_len += encode_eof(expected + expected_len);
      auto res = ely::stx::lex(id, buffer);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));
    }
    {
      auto id = make_src("var123/other/more");
      auto expected_len = encode_identifier(expected, 6);
      expected_len += encode_slash(expected + expected_len);
      expected_len += encode_identifier(expected + expected_len, 5);
      expected_len += encode_slash(expected + expected_len);
      expected_len += encode_identifier(expected + expected_len, 4);
      expected_len += encode_eof(expected + expected_len);
      auto res = ely::stx::lex(id, buffer);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));
    }
    {
      std::string_view id[] = {"hello", "world", "this_is_a_test",
                               "variant123"};
      auto expected_len = encode_spill(expected, id[0].size(), CONT_IDENTIFIER);
      auto res = ely::stx::lex(id[0], buffer);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));

      expected_len = encode_spill(expected, id[1].size(), CONT_IDENTIFIER);
      res = ely::stx::lex(id[1], buffer, buffer[res - 2]);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));

      expected_len = encode_spill(expected, id[2].size(), CONT_IDENTIFIER);
      res = ely::stx::lex(id[2], buffer, buffer[res - 2]);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));

      expected_len = encode_spill(expected, id[3].size(), CONT_IDENTIFIER);
      // expected_len += encode_eof(expected + expected_len);
      res = ely::stx::lex(id[3], buffer, buffer[res - 2]);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));

      expected_len = encode_identifier(expected, 0);
      expected_len += encode_eof(expected + expected_len);
      res = ely::stx::lex(std::string_view{&terminate, 1}, buffer,
                          buffer[res - 2]);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));
    }
    {
      // decimal literal with spill
      std::string_view num[] = {"123.", "45"};

      auto expected_len =
          encode_spill(expected, num[0].size(), CONT_DECIMAL_LIT);
      auto res = ely::stx::lex(num[0], buffer);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));

      // continuing after the dot should be in decimal mode
      expected_len = encode_spill(expected, num[1].size(), CONT_DECIMAL_LIT);
      res = ely::stx::lex(num[1], buffer, buffer[res - 2]);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));

      expected_len = encode_decimal_lit(expected, 0);
      expected_len += encode_eof(expected + expected_len);
      res = ely::stx::lex(std::string_view{&terminate, 1}, buffer,
                          buffer[res - 2]);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));
    }
    {
      // string literal with spill
      std::string_view str[] = {"\"Hello, ", "world!\""};

      auto expected_len =
          encode_spill(expected, str[0].size(), CONT_STRING_LIT);
      auto res = ely::stx::lex(str[0], buffer);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));

      expected_len = encode_string_lit(expected, str[1].size());
      expected_len += encode_spill(expected + expected_len, 0, CONT_START);
      res = ely::stx::lex(str[1], buffer, buffer[res - 2]);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));

      expected_len = encode_eof(expected);
      res = ely::stx::lex(std::string_view{&terminate, 1}, buffer);
      assert(res == expected_len);
      assert(check_equal(buffer, expected, res));
    }
  }
  return 0;
}

#ifndef NO_MAIN
int main() {
  // static_assert(lexer() == 0);
  return lexer();
}
#endif
