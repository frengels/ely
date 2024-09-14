#pragma once

#include <string>
#include <variant>

namespace ely {
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

struct identifier {
  static constexpr const char* short_name = "id";

  std::string text;

  constexpr std::size_t size() const { return text.size(); }
  constexpr std::string to_string() const { return text; }
};

struct integer_lit {
  static constexpr const char* short_name = "int";

  std::string text;

  constexpr std::size_t size() const { return text.size(); }
  constexpr std::string to_string() const { return text; }
};
struct decimal_lit {
  static constexpr const char* short_name = "dec";

  std::string text;

  constexpr std::size_t size() const { return text.size(); }
  constexpr std::string to_string() const { return text; }
};
struct string_lit {
  static constexpr const char* short_name = "str";

  std::string text;

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

  std::string text;

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
} // namespace tokens

namespace detail {
using token_variant =
    std::variant<tokens::whitespace, tokens::tab, tokens::newline_lf,
                 tokens::newline_cr, tokens::newline_crlf, tokens::lparen,
                 tokens::rparen, tokens::lbracket, tokens::rbracket,
                 tokens::lbrace, tokens::rbrace, tokens::identifier,
                 tokens::integer_lit, tokens::decimal_lit, tokens::string_lit,
                 tokens::unterminated_string_lit, tokens::unknown, tokens::eof>;
}

class token : public detail::token_variant {
public:
  using detail::token_variant::token_variant;

  constexpr bool is_eof() const {
    return std::holds_alternative<tokens::eof>(*this);
  }

  constexpr bool is_newline() const {
    return std::holds_alternative<tokens::newline_lf>(*this) ||
           std::holds_alternative<tokens::newline_cr>(*this) ||
           std::holds_alternative<tokens::newline_crlf>(*this);
  }

  constexpr bool is_atmosphere() const {
    return is_newline() || std::holds_alternative<tokens::whitespace>(*this) ||
           std::holds_alternative<tokens::tab>(*this);
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