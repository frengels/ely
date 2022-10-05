#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>
#include <variant>
#include <vector>

#include "mli/arena.hpp"
#include "mli/lexer.hpp"

namespace mli {
struct parse_error {
  source_offset offset;
  std::string msg;

  parse_error(std::string m, source_offset offset = {})
      : offset(offset), msg(std::move(m)) {}
};

class sexp;
class list;
class identifier;
class string_literal;
class integer_literal;
class decimal_literal;

class sexp {
  std::variant<arena_ptr<list>, arena_ptr<identifier>,
               arena_ptr<string_literal>, arena_ptr<integer_literal>,
               arena_ptr<decimal_literal>>
      active_;

public:
  sexp() = default;

  explicit constexpr sexp(arena_ptr<list> l) : active_(l) {}
  explicit constexpr sexp(arena_ptr<identifier> id) : active_(id) {}
  explicit constexpr sexp(arena_ptr<string_literal> lit) : active_(lit) {}
  explicit constexpr sexp(arena_ptr<integer_literal> lit) : active_(lit) {}
  explicit constexpr sexp(arena_ptr<decimal_literal> lit) : active_(lit) {}

  template <typename F>
  constexpr auto visit(F &&fn) const
      -> decltype(std::visit(static_cast<F &&>(fn), active_)) {
    return std::visit(static_cast<F &&>(fn), active_);
  }

  constexpr source_offset pos() const;

  template <typename T> constexpr bool isa() const {
    return std::holds_alternative<arena_ptr<T>>(active_);
  }

  template <typename T> constexpr arena_ptr<T> get_if() const {
    const arena_ptr<T> *res = std::get_if<arena_ptr<T>>(&active_);
    if (res) {
      return *res;
    } else {
      return {};
    }
  }
};

class list {
  std::vector<arena_ptr<sexp>> sexps_;
  source_offset lp_pos_;
  source_offset rp_pos_;

public:
  explicit list(std::vector<arena_ptr<sexp>> sexps, source_offset lp_pos = {},
                source_offset rp_pos = {})
      : sexps_(std::move(sexps)), lp_pos_(lp_pos), rp_pos_(rp_pos) {}

  constexpr source_offset pos() const { return lp_pos_; }
  constexpr source_offset lp_pos() const { return lp_pos_; }
  constexpr source_offset rp_pos() const { return rp_pos_; }
  constexpr const auto &sexps() const { return sexps_; }
  constexpr auto &sexps() { return sexps_; }
};

class source {
  std::vector<arena_ptr<sexp>> sexps_;

public:
  source() = default;
  explicit source(std::vector<arena_ptr<sexp>> sexps)
      : sexps_(std::move(sexps)) {}

  template <typename... Args> arena_ptr<sexp> &emplace_back(Args &&...args) {
    return sexps_.emplace_back(
        std::make_unique<sexp>(static_cast<Args &&>(args)...));
  }

  constexpr const auto &sexps() const & { return sexps_; }
  constexpr auto &sexps() & { return sexps_; }
};

class identifier {
  source_offset pos_;
  std::string name_;

public:
  identifier() = default;

  identifier(std::string name, source_offset pos = {})
      : pos_(pos), name_(std::move(name)) {}

  std::string_view name() const { return static_cast<std::string_view>(name_); }

  constexpr source_offset pos() const { return pos_; }
};

class string_literal {
  source_offset pos_;
  std::string str_;

public:
  string_literal() = default;

  string_literal(std::string str, source_offset pos)
      : pos_(pos), str_(std::move(str)) {}

  std::string_view str() const { return static_cast<std::string_view>(str_); }

  constexpr source_offset pos() const { return pos_; }
};

class integer_literal {
  source_offset pos_;
  std::string str_;

public:
  integer_literal() = default;

  integer_literal(std::string str, source_offset pos = {})
      : pos_(pos), str_(std::move(str)) {}

  std::string_view str() const { return static_cast<std::string_view>(str_); }

  constexpr source_offset pos() const { return pos_; }
};

class decimal_literal {
  source_offset pos_;
  std::string str_;

public:
  decimal_literal() = default;

  decimal_literal(std::string str, source_offset pos)
      : pos_(pos), str_(std::move(str)) {}

  std::string_view str() const { return static_cast<std::string_view>(str_); }

  constexpr source_offset pos() const { return pos_; }
};

constexpr source_offset sexp::pos() const {
  return this->visit([](auto p) -> source_offset { return p->pos(); });
}

class parser {
private:
  using token_t = mli::basic_token<mli::string_view>;

  mli::string_view filename_;
  mli::lexer lex_;
  source_offset offset_{0};
  std::vector<parse_error> errors_;

public:
  parser(mli::string_view filename, mli::string_view src)
      : filename_(filename), lex_(src) {}

  explicit parser(mli::string_view src) : filename_("<unknown>"), lex_(src) {}

  source parse(arena &alloc) {
    auto sexps = std::vector<arena_ptr<sexp>>{};
    auto [tok, tok_offset] = next_token();

    while (tok.kind() != token_kind::eof) {
      arena_ptr<sexp> s = parse_sexp(tok, tok_offset, alloc);
      sexps.push_back(s);
      std::tie(tok, tok_offset) = next_token();
    }

    return source(std::move(sexps));
  }

private:
  constexpr std::pair<token_t, source_offset> next_token() {
    auto res = lex_.next();
    offset_ += res.preceding_atmosphere();
    source_offset res_offset = offset_;
    offset_ += res.size();
    return {res.token(), res_offset};
  }

  arena_ptr<sexp> parse_sexp(token_t tok, source_offset offset, arena &alloc) {
    arena_ptr<sexp> res = alloc.allocate<sexp>();

  loop:
    switch (tok.kind()) {
    case token_kind::atmosphere:
      __builtin_unreachable();
    case token_kind::lparen: {
      arena_ptr<list> l = parse_list(tok, offset, alloc);
      new (res.get()) sexp(l);
    } break;
    case token_kind::rparen:
      errors_.emplace_back(std::string{"Unexpected `)`"}, offset);
      goto loop;
    case token_kind::eof:
      break;
    case token_kind::integer_literal: {
      arena_ptr<integer_literal> lit =
          parse_integer_literal(tok, offset, alloc);
      new (res.get()) sexp(lit);
    } break;
    case token_kind::decimal_literal: {
      arena_ptr<decimal_literal> lit =
          parse_decimal_literal(tok, offset, alloc);
      new (res.get()) sexp(lit);
    } break;
    case token_kind::string_literal: {
      arena_ptr<string_literal> lit = parse_string_literal(tok, offset, alloc);
      new (res.get()) sexp(lit);
    } break;
    case token_kind::identifier: {
      arena_ptr<identifier> id = parse_identifier(tok, offset, alloc);
      new (res.get()) sexp(id);
    } break;
    case token_kind::unknown_char:
      __builtin_unreachable();
      break;
    }

    return res;
  }

  arena_ptr<list> parse_list(token_t tok, source_offset offset, arena &alloc) {
    assert(tok.kind() == token_kind::lparen);
    auto lp_pos = offset;

    auto sexps = std::vector<arena_ptr<sexp>>{};

    std::tie(tok, offset) = next_token();

    while (tok.kind() != token_kind::rparen) {
      if (!tok) {
        errors_.emplace_back(std::string{"missing closing `)`"}, offset);
        break;
      }

      arena_ptr<sexp> p = parse_sexp(tok, offset, alloc);
      sexps.push_back(p);
      std::tie(tok, offset) = next_token();
    }

    auto rp_pos = offset;

    arena_ptr<list> l = alloc.allocate<list>();
    new (l.get()) list(std::move(sexps), lp_pos, rp_pos);

    return l;
  }

  arena_ptr<string_literal>
  parse_string_literal(token_t tok, source_offset offset, arena &alloc) {
    assert(tok.kind() == token_kind::string_literal);
    arena_ptr<string_literal> lit = alloc.allocate<string_literal>();

    new (lit.get()) string_literal(std::string{tok.begin(), tok.end()}, offset);
    return lit;
  }

  arena_ptr<integer_literal>
  parse_integer_literal(token_t tok, source_offset offset, arena &alloc) {
    assert(tok.kind() == token_kind::integer_literal);
    arena_ptr<integer_literal> lit = alloc.allocate<integer_literal>();

    new (lit.get())
        integer_literal(std::string{tok.begin(), tok.end()}, offset);
    return lit;
  }

  arena_ptr<decimal_literal>
  parse_decimal_literal(token_t tok, source_offset offset, arena &alloc) {
    assert(tok.kind() == token_kind::decimal_literal);
    arena_ptr<decimal_literal> lit = alloc.allocate<decimal_literal>();

    new (lit.get())
        decimal_literal(std::string{tok.begin(), tok.end()}, offset);
    return lit;
  }

  arena_ptr<identifier> parse_identifier(token_t tok, source_offset offset,
                                         arena &alloc) {
    assert(tok.kind() == token_kind::identifier);
    arena_ptr<identifier> id = alloc.allocate<identifier>();

    new (id.get()) identifier(std::string{tok.begin(), tok.end()}, offset);
    return id;
  }
};
} // namespace mli