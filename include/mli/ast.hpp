#pragma once

#include <string>
#include <variant>
#include <vector>

#include "mli/arena.hpp"
#include "mli/lexer.hpp"

namespace mli {
namespace ast {

class fn;
class var;
class val;
class exp;
class stx;
class list;
class id;
class sym;
class lit;
class ast;
class call;

class var {
  std::string name_;

public:
  var() = default;
  var(std::string name) : name_(std::move(name)) {}

  std::string_view name() const { return name_; }
};

class val {
  std::variant<arena_ptr<fn>, arena_ptr<list>, arena_ptr<lit>, arena_ptr<stx>>
      active_;

public:
  constexpr val(arena_ptr<fn> f) : active_(f) {}
  constexpr val(arena_ptr<list> l) : active_(l) {}

  template <typename F> constexpr auto visit(F &&fn) const & -> decltype(auto) {
    return std::visit(static_cast<F &&>(fn), active_);
  }

  template <typename T> constexpr bool isa() const {
    return std::holds_alternative<arena_ptr<T>>(active_);
  }
};

class fn {
  std::vector<arena_ptr<var>> args_;
  arena_ptr<ast> body_;

public:
  fn(std::initializer_list<arena_ptr<var>> args, arena_ptr<ast> body)
      : args_(args), body_(body) {}
  fn(std::vector<arena_ptr<var>> args, arena_ptr<ast> body)
      : args_(std::move(args)), body_(body) {}

  constexpr const auto &args() const { return args_; }
  constexpr arena_ptr<ast> body() const { return body_; }
};

class def {
  arena_ptr<var> name_;
  arena_ptr<ast> init_;

public:
  def(arena_ptr<var> name, arena_ptr<ast> init) : name_(name), init_(init) {}

  constexpr arena_ptr<var> name() const { return name_; }
  constexpr arena_ptr<ast> init() const { return init_; }
};

class sym {
  std::string name_;

public:
  sym() = default;
  sym(std::string name) : name_(std::move(name)) {}

  std::string_view name() const { return name_; }
};

class id {
  sym sym_;

public:
  id() = default;
  id(sym s) : sym_(std::move(s)) {}
};

class string_lit {
  std::string str_;

public:
  string_lit(std::string str) : str_(std::move(str)) {}
};

class int_lit {
  std::string str_;

public:
  int_lit(std::string str) : str_(std::move(str)) {}
};

class dec_lit {
  std::string str_;

public:
  dec_lit(std::string str) : str_(std::move(str)) {}
};

class lit {
  std::variant<arena_ptr<string_lit>, arena_ptr<int_lit>, arena_ptr<dec_lit>>
      active_;

public:
  constexpr lit(arena_ptr<string_lit> l) : active_(l) {}
  constexpr lit(arena_ptr<int_lit> l) : active_(l) {}
  constexpr lit(arena_ptr<dec_lit> l) : active_(l) {}

  template <typename F> constexpr auto visit(F &&fn) const -> decltype(auto) {
    return std::visit(static_cast<F &&>(fn), active_);
  }
};

class call {
  std::vector<arena_ptr<ast>> op_and_operands_;

public:
    call(arena_ptr<ast> op) : op_and_operands_({operator}) {}
    call(arena_ptr<ast> op, std::initializer_list<arena_ptr<ast>> operands)
        : call(op) {
      op_and_operands_.insert(op_and_operands_.end(), operands);
    }
    call(std::initializer_list<arena_ptr<ast>> op_and_operands)
        : op_and_operands_(op_and_operands) {}
    call(std::vector<arena_ptr<ast>> op_and_operands)
        : op_and_operands_(std::move(op_and_operands)) {}
    call(arena_ptr<ast> *op_and_operands, std::size_t len)
        : op_and_operands_(op_and_operands, op_and_operands + len) {}
};

class ast {
  std::variant<arena_ptr<var>, arena_ptr<call>, arena_ptr<val>> active_;

public:
  constexpr ast(arena_ptr<var> v) : active_(v) {}
  constexpr ast(arena_ptr<call> c) : active_(c) {}
  constexpr ast(arena_ptr<val> v) : active_(v) {}

  template <typename F> constexpr auto visit(F &&fn) const -> decltype(auto) {
    return std::visit(static_cast<F &&>(fn), active_);
  }
};

class context {
  mli::arena *alloc_;

public:
  explicit context(arena &alloc) : alloc_(std::addressof(alloc)) {}

  arena_ptr<lit> create_string_lit(std::string str) {
    arena_ptr<lit> res = alloc_->allocate<lit>();
    arena_ptr<string_lit> str_lit = alloc_->allocate<string_lit>();

    new (str_lit.get()) string_lit(std::move(str));
    new (res.get()) lit(str_lit);

    return res;
  }
};
} // namespace ast
} // namespace mli