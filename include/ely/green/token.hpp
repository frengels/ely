#pragma once

#include <cstdint>
#include <string>

#include "ely/util/variant.hpp"
#include "ely/util/visit.hpp"

#include <fmt/base.h>
#include <fmt/ranges.h>

namespace ely {
namespace green {
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

namespace detail {
using token_variant = ely::variant<green::int_literal, green::float_literal,
                                   green::string_literal, green::identifier>;
}

class token : public detail::token_variant {
public:
  using detail::token_variant::token_variant;
};
} // namespace green
} // namespace ely

template <> struct fmt::formatter<ely::green::int_literal> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::green::int_literal& ilit,
                        FmtCtx& ctx) const {
    return fmt::format_to(ctx.out(), "{}", ilit.value());
  }
};

template <> struct fmt::formatter<ely::green::float_literal> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::green::float_literal& flit,
                        FmtCtx& ctx) const {
    return fmt::format_to(ctx.out(), "{}", flit.value());
  }
};

template <> struct fmt::formatter<ely::green::string_literal> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::green::string_literal& slit,
                        FmtCtx& ctx) const {
    return fmt::format_to(ctx.out(), "\"{}\"", slit.value());
  }
};

template <> struct fmt::formatter<ely::green::identifier> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::green::identifier& id, FmtCtx& ctx) const {
    // TODO handle escaping identifiers with uncommon values, such as spaces or
    // tabs
    return fmt::format_to(ctx.out(), "{}", id.value());
  }
};

template <> struct fmt::formatter<ely::green::token> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }

  template <typename FmtCtx>
  constexpr auto format(const ely::green::token& t, FmtCtx& ctx) const {
    return ely::visit(
        [&]<typename T>(const T& x) {
          auto fmt = ::fmt::formatter<T>{};
          return fmt.format(x, ctx);
        },
        t);
  }
};
