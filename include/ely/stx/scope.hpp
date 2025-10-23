#pragma once

#include <algorithm>
#include <concepts>
#include <ranges>
#include <set>
#include <span>

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
  explicit constexpr scope(unsigned id) : id_(id) {}

public:
  constexpr unsigned id() const { return id_; }

  friend constexpr auto operator<=>(const scope& lhs, const scope& rhs) {
    return lhs.id_ <=> rhs.id_;
  }

  friend bool operator==(const scope&, const scope&) = default;
};

class scope_generator {
  unsigned current_id_;

public:
  scope_generator() = default;

  constexpr scope operator()() { return scope{current_id_++}; }

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

  [[nodiscard]]
  simple_scope_set add_scope(scope sc) const {
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

  std::size_t hash() const { return 0; }
};

static_assert(scope_set<simple_scope_set>);

template <scope_set SS> struct resolver {};
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
