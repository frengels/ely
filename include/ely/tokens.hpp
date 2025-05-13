#pragma once

#include <cstdint>
#include <string>
#include <variant>

#include "ely/symbol.hpp"

namespace ely {
namespace detail {
template <typename T, typename... Us>
inline constexpr bool is_one_of_v = (std::is_same_v<T, Us> || ...);
}
namespace tokens {
struct whitespace {
  static constexpr const char* short_name = "ws";

  std::size_t len;

  constexpr std::size_t size() const { return len; }
  constexpr std::string to_string() const { return std::string(len, ' '); }
};
struct tab {
  static constexpr const char* short_name = "\\t";

  std::size_t len;

  constexpr std::size_t size() const { return len; }
  constexpr std::string to_string() const { return std::string(len, '\t'); }
};
struct newline_lf {
  static constexpr const char* short_name = "\\n";

  static constexpr std::size_t size() { return 1; }
  static constexpr std::string to_string() { return short_name; }
};
struct newline_cr {
  static constexpr const char* short_name = "\\r";

  static constexpr std::size_t size() { return 1; }
  static constexpr std::string to_string() { return short_name; }
};
struct newline_crlf {
  static constexpr const char* short_name = "\\r\\n";

  static constexpr std::size_t size() { return 2; }
  static constexpr std::string to_string() { return short_name; }
};

struct rparen;
struct rbracket;
struct rbrace;

struct lparen {
  static constexpr const char* short_name = "(";
  using matching_end = rparen;

  static constexpr std::size_t size() { return 1; }
  static constexpr std::string to_string() { return short_name; }
};
struct rparen {
  static constexpr const char* short_name = ")";
  using matching_begin = lparen;

  static constexpr std::size_t size() { return 1; }
  static constexpr std::string to_string() { return short_name; }
};
struct lbracket {
  static constexpr const char* short_name = "[";
  using matching_end = rbracket;

  static constexpr std::size_t size() { return 1; }
  static constexpr std::string to_string() { return short_name; }
};
struct rbracket {
  static constexpr const char* short_name = "]";
  using matching_begin = lbracket;

  static constexpr std::size_t size() { return 1; }
  static constexpr std::string to_string() { return short_name; }
};
struct lbrace {
  static constexpr const char* short_name = "{";
  using matching_end = rbrace;

  static constexpr std::size_t size() { return 1; }
  static constexpr std::string to_string() { return short_name; }
};
struct rbrace {
  static constexpr const char* short_name = "}";
  using matching_begin = lbrace;

  static constexpr std::size_t size() { return 1; }
  static constexpr std::string to_string() { return short_name; }
};

struct quote {
  static constexpr const char* short_name = "'";
  static constexpr const char* symbol = "quote";

  static constexpr std::size_t size() { return 1; }
  static constexpr std::string to_string() { return short_name; }
};

struct quasiquote {
  static constexpr const char* short_name = "`";
  static constexpr const char* symbol = "quasiquote";

  static constexpr std::size_t size() { return 1; }
  static constexpr std::string to_string() { return short_name; }
};

struct unquote {
  static constexpr const char* short_name = ",";
  static constexpr const char* symbol = "unquote";
  static constexpr std::size_t size() { return 1; }
  static constexpr std::string to_string() { return short_name; }
};

struct unquote_splicing {
  static constexpr const char* short_name = ",@";
  static constexpr const char* symbol = "unquote-splicing";
  static constexpr std::size_t size() { return 2; }
  static constexpr std::string to_string() { return short_name; }
};

struct syntax {
  static constexpr const char* short_name = "#'";
  static constexpr const char* symbol = "syntax";
  static constexpr std::size_t size() { return 2; }
  static constexpr std::string to_string() { return short_name; }
};

struct quasisyntax {
  static constexpr const char* short_name = "#`";
  static constexpr const char* symbol = "quasisyntax";
  static constexpr std::size_t size() { return 2; }
  static constexpr std::string to_string() { return short_name; }
};

struct unsyntax {
  static constexpr const char* short_name = "#,";
  static constexpr const char* symbol = "unsyntax";
  static constexpr std::size_t size() { return 2; }
  static constexpr std::string to_string() { return short_name; }
};

struct unsyntax_splicing {
  static constexpr const char* short_name = "#,@";
  static constexpr const char* symbol = "unsyntax-splicing";
  static constexpr std::size_t size() { return 3; }
  static constexpr std::string to_string() { return short_name; }
};

struct identifier {
  static constexpr const char* short_name = "id";

  ely::symbol sym;

  constexpr std::size_t size() const { return std::to_string(sym.id).size(); }
  constexpr std::string to_string() const { return std::to_string(sym.id); }
};

struct slash {
  static constexpr const char* short_name = "/";

  constexpr std::size_t size() const { return 1; }
  constexpr std::string to_string() const { return short_name; }
};

struct integer_lit {
  static constexpr const char* short_name = "int";

  std::string_view text;

  constexpr std::size_t size() const { return text.size(); }
  constexpr std::string to_string() const { return std::string{text}; }
};
struct decimal_lit {
  static constexpr const char* short_name = "dec";

  std::string_view text;

  constexpr std::size_t size() const { return text.size(); }
  constexpr std::string to_string() const { return std::string{text}; }
};
struct string_lit {
  static constexpr const char* short_name = "str";

  std::string_view text;

  constexpr std::size_t size() const { return text.size() + 2; }
  constexpr std::string to_string() const {
    std::string res;
    res.push_back('"');
    res.append(text);
    res.push_back('"');
    return res;
  }
};

struct unterminated_string_lit {
  static constexpr const char* short_name = "unterm str";

  std::string_view text;

  constexpr std::size_t size() const { return text.size() + 1; }
  constexpr std::string to_string() const {
    std::string res;
    res.push_back('"');
    res.append(text);
    return res;
  }
};
struct unknown {
  static constexpr const char* short_name = "unknown";

  char c;

  static constexpr std::size_t size() { return 1; }
  constexpr std::string to_string() const { return std::string(1, c); }
};

struct eof {
  static constexpr const char* short_name = "eof";

  static constexpr std::size_t size() { return 0; }
  static constexpr std::string to_string() { return {}; }
};

template <typename T> inline constexpr bool is_eof_v = std::is_same_v<T, eof>;

template <typename T>
inline constexpr bool is_list_start_v =
    detail::is_one_of_v<T, lparen, lbrace, lbracket>;

template <typename T>
inline constexpr bool is_list_end_v =
    detail::is_one_of_v<T, rparen, rbrace, rbracket>;

template <typename T>
inline constexpr bool is_newline_v =
    detail::is_one_of_v<T, newline_lf, newline_cr, newline_crlf>;

template <typename T>
inline constexpr bool is_atmosphere_v =
    is_newline_v<T> || detail::is_one_of_v<T, whitespace, tab>;
} // namespace tokens

namespace detail {
using token_variant =
    std::variant<tokens::whitespace, tokens::tab, tokens::newline_lf,
                 tokens::newline_cr, tokens::newline_crlf, tokens::lparen,
                 tokens::rparen, tokens::lbracket, tokens::rbracket,
                 tokens::lbrace, tokens::rbrace, tokens::quote,
                 tokens::quasiquote, tokens::unquote, tokens::unquote_splicing,
                 tokens::syntax, tokens::quasisyntax, tokens::unsyntax,
                 tokens::unsyntax_splicing, tokens::identifier, tokens::slash,
                 tokens::integer_lit, tokens::decimal_lit, tokens::string_lit,
                 tokens::unterminated_string_lit, tokens::unknown, tokens::eof>;
}

class token : public detail::token_variant {
public:
  using detail::token_variant::token_variant;

  template <typename T> constexpr bool isa() const {
    return std::holds_alternative<T>(*this);
  }

  constexpr bool is_eof() const {
    return std::visit([]<typename T>(const T&) { return tokens::is_eof_v<T>; },
                      *this);
  }

  constexpr bool is_newline() const {
    return std::visit(
        []<typename T>(const T&) { return tokens::is_newline_v<T>; }, *this);
  }

  constexpr bool is_atmosphere() const {
    return std::visit(
        []<typename T>(const T&) { return tokens::is_atmosphere_v<T>; }, *this);
  }

  template <typename Tok> constexpr bool ends_list() const {
    return std::holds_alternative<typename Tok::matching_end>(*this);
  }

  template <typename Tok> constexpr bool begins_list() const {
    return std::holds_alternative<typename Tok::matching_begin>(*this);
  }

  constexpr std::size_t size() const {
    return std::visit([](const auto& t) -> std::size_t { return t.size(); },
                      *this);
  }

  constexpr const char* short_name() const {
    return std::visit(
        []<typename T>(const T&) -> const char* { return T::short_name; },
        *this);
  }

  constexpr std::string to_string() const {
    return std::visit(
        [](const auto& t) -> std::string { return t.to_string(); }, *this);
  }
};

constexpr std::size_t token_size(const token& t) {
  return std::visit([](const auto& v) -> std::size_t { return v.size(); }, t);
}

constexpr std::string_view token_short_name(const token& t) {
  return std::visit(
      []<typename T>(const T&) -> std::string_view { return T::short_name; },
      t);
}

constexpr std::string_view token_to_string(const token& t) {
  return std::visit([](const auto& v) { return v.to_string(); }, t);
}

constexpr bool token_is_eof(const token& t) {
  return std::holds_alternative<tokens::eof>(t);
}
} // namespace ely