#pragma once

#include <algorithm>
#include <cassert>
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

template <typename Id, typename V> struct binding {
  Id id;
  V val;

  constexpr const V& value() const { return val; }
  constexpr V& value() { return val; }
};

template <typename Id, typename V> class binding_map {
public:
  using binding_type = binding<Id, V>;
  using value_type = binding_type;
  using symbol_type =
      std::remove_cvref_t<decltype(std::declval<Id>().symbol())>;

private:
  std::unordered_map<symbol_type, std::vector<value_type>> map_;

public:
  binding_map() = default;

  bool insert(const Id& id, const V& val) {
    auto it = map_.find(id.symbol());
    if (it == map_.end()) {
      auto emplace_res = map_.emplace(id.symbol(), std::vector<value_type>{});
      assert(emplace_res.second);
      it = emplace_res.first;
    }

    // TODO check for duplicates here, we should probably return false if there
    // is a duplicate, and true otherwise

    auto& vec = it->second;
    vec.emplace_back(binding_type{id, val});
    return true;
  }

  // this will find all bindings for the given id, regardless of scope, this is
  // useful for error reporting and diagnostics, but not for name resolution
  std::span<value_type> find_bindings(const Id& id) {
    auto it = map_.find(id.symbol());
    if (it == map_.end()) {
      return {};
    }
    return it->second;
  }

  // should always check the base of the filter_view for empty before checking
  // the filter itself
  auto find_matching_bindings(const Id& id) {
    // TODO: this could probably be optimized by storing by ascending scope_set
    // size
    return find_bindings(id) |
           std::views::filter([&](const value_type& binding) {
             return binding.id.scope_set().subset_of(id.scope_set());
           });
  }

  std::expected<value_type, lookup_error> lookup(const Id& id) {
    auto matching_bindings = find_matching_bindings(id);

    std::size_t largest = 0;
    std::optional<value_type> best{};
    for (const value_type& binding : matching_bindings) {
      if (binding.id.scope_set().subset_of(id.scope_set()) &&
          binding.id.scope_set().size() > largest) {
        best = binding;
        largest = binding.id.scope_set().size();
      }
    }
    if (best) {
      return *best;
    } else if (matching_bindings.base().empty()) {
      return std::unexpected(lookup_error::key_not_found);
    } else {
      return std::unexpected(lookup_error::scope_not_found);
    }
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
