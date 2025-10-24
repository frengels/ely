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
namespace stx {

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

template <typename SS>
concept scope_set =
    requires(const SS& a, const SS& b, scope s, std::span<scope> ss) {
      { a.subset_of(b) } -> std::same_as<bool>;
      { a == b } -> std::same_as<bool>;
      { a.hash() } -> std::convertible_to<std::size_t>;
      { a.add_scope(s) } -> std::same_as<SS>;
      { a.add_scopes(ss) } -> std::same_as<SS>;
      { a.remove_scope(s) } -> std::same_as<SS>;
      { a.remove_scopes(ss) } -> std::same_as<SS>;
      { a.flip_scope(s) } -> std::same_as<SS>;
      { a.flip_scopes(ss) } -> std::same_as<SS>;
      { a.size() } -> std::same_as<std::size_t>;
    };

class simple_scope_set {
public:
  using const_iterator = typename std::set<scope>::const_iterator;

private:
  std::set<scope> set_;

public:
  simple_scope_set() = default;
  simple_scope_set(std::initializer_list<scope> il) : set_(il) {}

  std::size_t size() const { return set_.size(); }
  const_iterator begin() const { return set_.begin(); }
  const_iterator end() const { return set_.end(); }

  friend bool operator==(const simple_scope_set&,
                         const simple_scope_set&) = default;

  bool subset_of(const simple_scope_set& other) const {
    return std::includes(other.begin(), other.end(), begin(), end());
  }

  bool has_scope(const scope& sc) const { return set_.find(sc) != set_.end(); }

  [[nodiscard]]
  simple_scope_set add_scope(const scope& sc) const {
    simple_scope_set res = *this;
    res.set_.insert(sc);
    return res;
  }

  [[nodiscard]]
  simple_scope_set add_scopes(std::span<scope> scopes) const {
    simple_scope_set res = *this;
    res.set_.insert(scopes.begin(), scopes.end());
    return res;
  }

  [[nodiscard]]
  simple_scope_set remove_scope(const scope& sc) const {
    simple_scope_set res = *this;
    res.set_.erase(sc);
    return res;
  }

  [[nodiscard]]
  simple_scope_set remove_scopes(std::span<scope> scopes) const {
    simple_scope_set res = *this;
    for (const auto& sc : scopes) {
      res.set_.erase(sc);
    }
    return res;
  }

  [[nodiscard]]
  simple_scope_set flip_scope(const scope& sc) const {
    if (has_scope(sc)) {
      return remove_scope(sc);
    } else {
      return add_scope(sc);
    }
  }

  [[nodiscard]]
  simple_scope_set flip_scopes(std::span<scope> scopes) const {
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

  std::size_t hash() const { return 0; }
};

static_assert(scope_set<simple_scope_set>);

template <std::size_t N> class bitset_scope_set {
private:
  std::bitset<N> scope_set_;

public:
  bitset_scope_set() = default;

  friend constexpr bool operator==(const bitset_scope_set& lhs,
                                   const bitset_scope_set& rhs) {
    return lhs.scope_set_ == rhs.scope_set_;
  }

  constexpr std::size_t size() const { return scope_set_.count(); }

  constexpr bool subset_of(const bitset_scope_set& other) const {
    return (other.scope_set_ & scope_set_) == scope_set_;
  }

  [[nodiscard]]
  constexpr bitset_scope_set add_scope(const scope& sc) const {
    assert(sc.id() < N && "out of range");
    auto res = *this;
    res.scope_set_.set(sc.id());
    return res;
  }

  [[nodiscard]]
  constexpr bitset_scope_set add_scopes(std::span<scope> scopes) const {
    auto res = *this;
    for (const auto& sc : scopes) {
      assert(sc.id() < N && "out of range");
      res.scope_set_.set(sc.id());
    }
    return res;
  }

  [[nodiscard]]
  constexpr bitset_scope_set remove_scope(const scope& sc) const {
    assert(sc.id() < N && "out of range");
    auto res = *this;
    res.scope_set_.reset(sc.id());
    return res;
  }

  [[nodiscard]]
  constexpr bitset_scope_set remove_scopes(std::span<scope> scopes) const {
    auto res = *this;
    for (const auto& sc : scopes) {
      assert(sc.id() < N && "out of range");
      res.scope_set_.reset(sc.id());
    }
    return res;
  }

  [[nodiscard]]
  constexpr bitset_scope_set flip_scope(const scope& sc) const {
    auto res = *this;
    res.scope_set_.flip(sc.id());
    return res;
  }

  [[nodiscard]]
  constexpr bitset_scope_set flip_scopes(std::span<scope> scopes) const {
    auto res = *this;
    for (const auto& sc : scopes) {
      res.scope_set_.flip(sc.id());
    }
    return res;
  }

  constexpr std::size_t hash() const {
    return std::hash<std::bitset<N>>{}(scope_set_);
  }
};

enum struct lookup_error { key_not_found, scope_not_found, ambiguous };

template <scope_set SS, typename K, typename T> class resolver {
private:
  struct scoped_value {
    SS scope;
    T val;
  };

  std::unordered_map<K, std::vector<scoped_value>> symbol_table_;

public:
  resolver() = default;

  std::expected<T, lookup_error> lookup(K name, SS scope_set) {
    auto it = symbol_table_.find(name);
    if (it == symbol_table_.end()) {
      return std::unexpected(lookup_error::key_not_found);
    }

    // found something, now get the largest subset
    std::size_t largest = 0;
    std::optional<T> best{};
    for (const scoped_value& sv : it->second) {
      if (scope_set.subset_of(sv.scope) && sv.scope.size() > largest) {
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
  bool insert(K name, SS scope_set, T val) {
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
} // namespace stx
} // namespace ely

template <> struct fmt::formatter<ely::stx::scope> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::scope& s, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "scope({})", s.id());
  }
};

template <> struct fmt::formatter<ely::stx::simple_scope_set> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::simple_scope_set& ss, Ctx& ctx) const {
    return fmt::format_to(
        ctx.out(), "simple_scope_set({})",
        fmt::join(
            std::ranges::subrange(ss.begin(), ss.end()) |
                std::views::transform([](ely::stx::scope s) { return s.id(); }),
            ", "));
  }
};
