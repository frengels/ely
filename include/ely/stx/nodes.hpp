#pragma once

#include <concepts>
#include <string>
#include <vector>

#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include "ely/util/variant.hpp"
#include "ely/util/visit.hpp"

namespace ely {
namespace stx {
class node;

class int_literal {
  void* token_;
  std::int64_t value_;

public:
  explicit constexpr int_literal(std::int64_t val, void* token = nullptr)
      : token_(token), value_(val) {}

  constexpr std::int64_t value() const { return value_; }
  constexpr operator std::int64_t() const { return value(); }
};

class float_literal {
  void* token_;
  float value_;

public:
  explicit constexpr float_literal(float f, void* token = nullptr)
      : token_(token), value_(f) {}

  constexpr float value() const { return value_; }
  constexpr operator float() const { return value(); }
};

class string_literal {
  void* token_;
  std::string value_;

public:
  template <typename S>
    requires(std::is_constructible_v<decltype(value_), S>)
  explicit(!std::is_convertible_v<
           S, decltype(value_)>) constexpr string_literal(S&& lit,
                                                          void* token = nullptr)
      : token_(token), value_(static_cast<S&&>(lit)) {}

  constexpr std::string_view value() const { return value_; }
};

class identifier {
  void* token_;
  std::string id_;

public:
  template <typename S>
    requires(std::constructible_from<std::string, S>)
  explicit(!std::convertible_to<S, std::string>) constexpr identifier(
      S&& str, void* token = nullptr)
      : token_(token), id_(static_cast<S&&>(str)) {}

  constexpr std::string_view value() const { return id_; }
};

// ( ... )
class list {
private:
  void* tokens_;
  std::vector<node> nodes_;

public:
  list() = default;
  constexpr list(std::vector<node>&& l, void* tokens = nullptr);

  constexpr auto begin();
  constexpr auto end();
  constexpr auto begin() const;
  constexpr auto end() const;

  template <typename... Args> constexpr node& emplace_back(Args&&... args);
};

using node_variant =
    ely::variant<int_literal, string_literal, identifier, list>;

class node : public node_variant {
public:
  using node_variant::node_variant;
};

constexpr list::list(std::vector<node>&& l, void* tokens)
    : tokens_(tokens), nodes_(std::move(l)) {}

constexpr auto list::begin() { return nodes_.begin(); }
constexpr auto list::end() { return nodes_.end(); }
constexpr auto list::begin() const { return nodes_.begin(); }
constexpr auto list::end() const { return nodes_.end(); }

template <typename... Args> constexpr node& list::emplace_back(Args&&... args) {
  return nodes_.emplace_back(static_cast<Args&&>(args)...);
}
} // namespace stx
} // namespace ely

template <> struct fmt::formatter<ely::stx::node> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::stx::node& n, FmtCtx& ctx) const {
    return ely::visit(
        [&](const auto& x) { return fmt::format_to(ctx.out(), "{}", x); }, n);
  }
};

template <> struct fmt::formatter<ely::stx::int_literal> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::stx::int_literal& ilit, FmtCtx& ctx) const {
    return fmt::format_to(ctx.out(), "{}", ilit.value());
  }
};

template <> struct fmt::formatter<ely::stx::float_literal> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::stx::float_literal& flit,
                        FmtCtx& ctx) const {
    return fmt::format_to(ctx.out(), "{}", flit.value());
  }
};

template <> struct fmt::formatter<ely::stx::string_literal> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::stx::string_literal& slit,
                        FmtCtx& ctx) const {
    return fmt::format_to(ctx.out(), "\"{}\"", slit.value());
  }
};

template <> struct fmt::formatter<ely::stx::identifier> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::stx::identifier& id, FmtCtx& ctx) const {
    // TODO handle escaping identifiers with uncommon values, such as spaces or
    // tabs
    return fmt::format_to(ctx.out(), "{}", id.value());
  }
};

template <> struct fmt::formatter<ely::stx::list> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::stx::list& l, FmtCtx& ctx) const {
    return fmt::format_to(ctx.out(), "({})", fmt::join(l, " "));
  }
};