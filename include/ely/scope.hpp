#pragma once

#include <algorithm>
#include <bitset>
#include <cassert>
#include <concepts>
#include <expected>
#include <ranges>
#include <set>
#include <span>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>

namespace ely {
class scope_generator;

class scope {
  friend class scope_generator;

private:
  unsigned id_;

private:
  explicit constexpr scope(unsigned id) : id_(id) {
    assert(id != 0 && "use scope() to generate the null scope");
  }

public:
  // default generated scope is the null scope
  scope() = default;
  constexpr unsigned id() const { return id_; }

  friend constexpr bool operator<(const scope& lhs, const scope& rhs) {
    return lhs.id() < rhs.id();
  }

  friend bool operator==(const scope&, const scope&) = default;
};

class scope_generator {
  unsigned current_id_;

public:
  scope_generator() = default;

  constexpr scope operator()() { return scope{++current_id_}; }

  constexpr scope next() { return this->operator()(); }
};

// TODO: this is currently quite expensive to copy, this should be memoized or
// uniqued, we'll have to implement a basic_scope_set_storage and similar
class scope_set {
public:
  using const_iterator = typename std::set<scope>::const_iterator;

private:
  std::set<scope> set_;

public:
  scope_set() = default;
  scope_set(std::initializer_list<scope> il) : set_(il) {}

  std::size_t size() const { return set_.size(); }
  const_iterator begin() const { return set_.begin(); }
  const_iterator end() const { return set_.end(); }

  friend bool operator==(const scope_set& lhs, const scope_set& rhs) = default;

  bool subset_of(const scope_set& other) const {
    // checks this set is a subset (subsequence) of other.
    return std::includes(other.begin(), other.end(), begin(), end());
  }

  bool has_scope(const scope& sc) const { return set_.find(sc) != set_.end(); }

  [[nodiscard]]
  scope_set add_scope(const scope& sc) const {
    scope_set res = *this;
    res.set_.insert(sc);
    return res;
  }

  [[nodiscard]]
  scope_set add_scopes(std::span<scope> scopes) const {
    scope_set res = *this;
    res.set_.insert(scopes.begin(), scopes.end());
    return res;
  }

  [[nodiscard]]
  scope_set remove_scope(const scope& sc) const {
    scope_set res = *this;
    res.set_.erase(sc);
    return res;
  }

  [[nodiscard]]
  scope_set remove_scopes(std::span<scope> scopes) const {
    scope_set res = *this;
    for (const auto& sc : scopes) {
      res.set_.erase(sc);
    }
    return res;
  }

  [[nodiscard]]
  scope_set flip_scope(const scope& sc) const {
    if (has_scope(sc)) {
      return remove_scope(sc);
    } else {
      return add_scope(sc);
    }
  }

  [[nodiscard]]
  scope_set flip_scopes(std::span<scope> scopes) const {
    auto res = *this;
    for (const auto& sc : scopes) {
      if (res.has_scope(sc)) {
        res.set_.erase(sc);
      } else {
        res.set_.insert(sc);
      }
    }
    return res;
  }
};

enum struct lookup_error { key_not_found, scope_not_found, ambiguous };

template <typename K, typename T> class resolver {
private:
  struct scoped_value {
    scope_set scope;
    T val;
  };

  std::unordered_map<K, std::vector<scoped_value>> symbol_table_;

public:
  resolver() = default;

  std::expected<T, lookup_error> lookup(K name, const scope_set& scope_set) {
    auto it = symbol_table_.find(name);
    if (it == symbol_table_.end()) {
      return std::unexpected(lookup_error::key_not_found);
    }

    // found something, now get the largest subset
    std::size_t largest = 0;
    std::optional<T> best{};
    for (const scoped_value& sv : it->second) {
      if (sv.scope.subset_of(scope_set) && sv.scope.size() > largest) {
        best = sv.val;
      }
    }

    if (!best) {
      return std::unexpected(lookup_error::scope_not_found);
    }

    return *best;
  }

  // returns true if inserted, false otherwise
  [[nodiscard]]
  bool insert(K name, const scope_set& scope_set, T val) {
    auto vec_it = symbol_table_.find(name);
    if (vec_it == symbol_table_.end()) {
      auto emplace_res =
          symbol_table_.emplace(name, std::vector<scoped_value>{});
      assert(emplace_res.second);
      vec_it = emplace_res.first;
    }

    auto& vec = vec_it->second;
    vec.emplace_back(scope_set, val);
    return true;
  }
};
} // namespace ely

template <> struct fmt::formatter<ely::scope> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::scope& s, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "scope({})", s.id());
  }
};

template <> struct fmt::formatter<ely::scope_set> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::scope_set& ss, Ctx& ctx) const {
    return fmt::format_to(
        ctx.out(), "scope_set({})",
        fmt::join(
            std::ranges::subrange(ss.begin(), ss.end()) |
                std::views::transform([](ely::scope s) { return s.id(); }),
            ", "));
  }
};
