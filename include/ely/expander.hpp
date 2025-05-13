#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "ely/binding.hpp"
#include "ely/dbg.hpp"
#include "ely/stx.hpp"
#include "ely/symbol.hpp"

namespace ely {
using transformer = std::function<const stx::sexp*(const stx::sexp&)>;

template <typename Arena> class expander {
private:
  ely::binding_map bindings_;
  ely::scope_set current_ss_;
  Arena* arena_;

public:
  explicit constexpr expander(Arena& arena) : arena_(std::addressof(arena)) {}

  constexpr const stx::sexp* expand_once(const stx::sexp& s) {
    using std::visit;
    return visit(
        [&]<typename T>(const T& t) -> const stx::sexp* {
          if constexpr (std::is_same_v<T, stx::list>) {
            ELY_DBG(fmt::println("Expanding list {}", t));
            // provide the original sexp as well
            return expand_once_impl(s, t);
          } else {
            assert(false && "unimplemented");
          }
        },
        s);
  }

  constexpr const stx::sexp* expand_all(const stx::sexp& s) {
    auto maybe_expanded = expand_once(s);
    auto res = std::addressof(s);
    while (maybe_expanded) {
      // store in res as expand_once could return nullptr
      res = maybe_expanded;
      maybe_expanded = expand_once(*res);
    }

    return res;
  }

  void add_builtin(ely::symbol sym, transformer&& fn) {
    bindings_.try_emplace(sym, current_ss_, std::move(fn));
  }

private:
  const stx::sexp* expand_once_impl(const stx::sexp& s, const stx::list& l) {
    // empty list, do nothing for now
    if (l.begin() == l.end())
      nullptr;
    auto& maybe_call = *l.begin();

    if (maybe_call.is_identifier()) {
      if (auto maybe_bind =
              bindings_.resolve(maybe_call.as_identifier()->sym(), current_ss_);
          maybe_bind) {
        auto bind = *maybe_bind;
        auto transform_args_stx = stx::sexp::create<stx::list>(*arena_);
        auto transform_args = transform_args_stx->template as<stx::list>();
        for (auto it = std::next(l.begin()), end = l.end(); it != end; ++it) {
          // make copies
          transform_args->push_back(stx::sexp::create_copy(*arena_, *it));
        }

        /* call with stx_list as arg */
        if (auto* fn = bind.as_function()) {
          return (*fn)(s);
        } else {
          assert(false && "Cannot yet do anything with raw sexp in expansion");
        }
      }
    }

    // return nullptr if no expansion happened
    return nullptr;
  }
};
} // namespace ely