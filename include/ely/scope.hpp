#pragma once

#include <functional>
#include <memory>
#include <ranges>
#include <set>

#include <fmt/format.h>
#include <fmt/ranges.h>

namespace ely {
class scope {
private:
  inline static unsigned current_id = 0;

private:
  unsigned id_;

private:
  explicit constexpr scope(unsigned id) : id_(id) {}

private:
  scope() : scope(current_id++) {}

public:
  constexpr unsigned id() const { return id_; }

  friend constexpr auto operator<=>(const scope& lhs, const scope& rhs) {
    return lhs.id_ <=> rhs.id_;
  }

  friend bool operator==(const scope&, const scope&) = default;

  static scope generate() { return scope(); }
};

class scope_set {
public:
  using const_iterator = typename std::set<scope>::const_iterator;

private:
  std::set<scope> set_;

public:
  scope_set() = default;
  scope_set(std::initializer_list<scope> il) : set_(il) {}

  const_iterator begin() const { return set_.begin(); }
  const_iterator end() const { return set_.end(); }

  friend auto operator<=>(const scope_set& lhs, const scope_set& rhs) {
    return lhs.set_.size() <=> rhs.set_.size();
  }

  friend bool operator==(const scope_set&, const scope_set&) = default;

  bool contains(const scope& s) const { return set_.find(s) != set_.end(); }

  bool contains(const scope_set& s) const { return subset_size(s) != 0; }

  // check if the supplies scope_set matches a definition for this scope_set
  // returns size of the matched subset
  unsigned subset_size(const scope_set& other) const {
    unsigned res = 0;
    for (auto it = other.begin(), end = other.end(); it != end; ++it) {
      if (!contains(*it))
        return 0;
      ++res;
    }

    return res;
  }

  void add_scope(scope sc) { set_.insert(sc); }
  void emplace_scope() { add_scope(scope::generate()); }
  bool remove_scope(scope sc) { return set_.erase(sc) == 1; }
};

class scope_guard {
  ely::scope s_;
  ely::scope_set* ss_;

public:
  scope_guard(ely::scope_set& ss)
      : s_(ely::scope::generate()), ss_(std::addressof(ss)) {
    ss_->add_scope(s_);
  }

  ~scope_guard() { ss_->remove_scope(s_); }

  scope_guard(const scope_guard&) = delete;
  scope_guard(scope_guard&&) = delete;
  scope_guard& operator=(const scope_guard&) = delete;
  scope_guard& operator=(scope_guard&&) = delete;

  constexpr ely::scope get() const { return s_; }
  constexpr ely::scope_set& get_ss() const { return *ss_; }
};

template <typename F>
constexpr decltype(auto) with_new_scope(ely::scope_set& ss, F&& fn) {
  ely::scope_guard g{ss};
  return std::invoke(static_cast<F&&>(fn), g.get());
}
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
