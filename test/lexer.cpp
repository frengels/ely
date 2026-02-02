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
  }
  return 0;
}

#ifndef NO_MAIN
int main() {
  // static_assert(lexer() == 0);
  return lexer();
}
#endif
