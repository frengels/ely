#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <fmt/core.h>

#include "ely/scope.hpp"
#include "ely/stx.hpp"
#include "ely/symbol.hpp"
#include "ely/value.hpp"

namespace ely {
struct binding {
  using fn_type = std::function<const stx::sexp*(const stx::sexp&)>;

  ely::scope_set ss;
  std::variant<fn_type, ely::stx::sexp*> fn_or_exp;

  template <typename... SArgs>
  constexpr binding(const ely::scope_set& ss, SArgs&&... args)
      : ss(ss), fn_or_exp(static_cast<SArgs&&>(args)...) {}

  constexpr fn_type* as_function() { return std::get_if<fn_type>(&fn_or_exp); }

  constexpr stx::sexp* as_exp() {
    return *std::get_if<ely::stx::sexp*>(&fn_or_exp);
  }
};

class binding_map {
public:
  using symbol_type = ely::symbol;

private:
  std::unordered_map<symbol_type, std::vector<binding>> impl_;

public:
  binding_map() = default;

  template <typename... SArgs>
  binding try_emplace(symbol_type sym, const ely::scope_set& ss,
                      SArgs&&... args) {
    auto& bindings = impl_[sym];
    return bindings.emplace_back(ss, static_cast<SArgs&&>(args)...);
  }

  // find the best binding for the passed in id and scope set combination
  std::optional<binding> resolve(symbol_type sym,
                                 const ely::scope_set& ss) const {
    if (auto it = impl_.find(sym); it != impl_.end()) {
      auto& bindings = *it;

      auto best_match = std::max_element(
          std::begin(bindings.second), std::end(bindings.second),
          [&](auto largest, auto current) {
            return ss.subset_size(current.ss) > ss.subset_size(largest.ss);
          });
      return *best_match;
    }

    fmt::println("no match found");
    return {};
  }
};
} // namespace ely