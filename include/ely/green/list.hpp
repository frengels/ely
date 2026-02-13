#pragma once

#include "ely/green/token.hpp"
#include "ely/util/variant.hpp"
#include "ely/util/visit.hpp"

#include <fmt/base.h>

namespace ely {
namespace green {
class list;

namespace detail {
using token_or_list_variant = ely::variant<ely::green::list, ely::green::token>;
}
class list {
private:
  class token_or_list;

  friend class ::fmt::formatter<token_or_list>;

public:
  using value_type = token_or_list;

private:
  void* token_span_;  // implementation defined token span
  std::size_t width_; // cached text width
  std::vector<value_type> children_;

public:
  list() = default;
  constexpr list(std::vector<value_type>&& children, std::size_t width = {},
                 void* token_span = nullptr);

  constexpr auto width() const { return width_; }
  constexpr auto size() const;

  constexpr auto begin() const { return children_.begin(); }
  constexpr auto end() const { return children_.end(); }

  constexpr auto rbegin() const { return children_.rbegin(); }
  constexpr auto rend() const { return children_.rend(); }
};

class list::token_or_list : public ely::variant<green::token, green::list> {
  friend class ::fmt::formatter<list::token_or_list>;

public:
  using ely::variant<green::token, green::list>::variant;
};

constexpr list::list(std::vector<value_type>&& children, std::size_t width,
                     void* token_span)
    : token_span_(token_span), width_(width), children_(std::move(children)) {}

constexpr auto list::size() const { return children_.size(); }

class list_builder {
public:
  using value_type = typename list::value_type;

private:
  std::vector<value_type> children_;

public:
  list_builder() = default;

  constexpr void reserve(std::size_t cap) { return children_.reserve(cap); }
  template <typename... Args>
  constexpr value_type& emplace_back(Args&&... args) {
    return children_.emplace_back(static_cast<Args&&>(args)...);
  }

  constexpr list finish() { return list(std::move(children_)); }
};
} // namespace green
} // namespace ely

template <> struct fmt::formatter<ely::green::list::token_or_list> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::green::list::token_or_list& tl,
                        FmtCtx& ctx) const {
    return ely::visit(
        [&]<typename T>(const T& t_or_l) {
          auto fmt = ::fmt::formatter<T>{};
          return fmt.format(t_or_l, ctx);
        },
        tl);
  }
};

template <> struct fmt::formatter<ely::green::list> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::green::list& l, FmtCtx& ctx) const {
    return fmt::format_to(ctx.out(), "({})", fmt::join(l, " "));
  }
};