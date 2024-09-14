#pragma once

#include <memory>
#include <span>
#include <string>
#include <variant>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>

namespace ely {
namespace stx {
class list;
class identifier;
class integer_lit;

class eof {
public:
  eof() = default;
};

class unknown {
public:
  unknown() = default;
};

namespace detail {
template <typename T> struct is_shared_ptr : std::false_type {};
template <typename T>
struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

using sexp_variant =
    std::variant<std::shared_ptr<stx::list>, std::shared_ptr<stx::identifier>,
                 std::shared_ptr<stx::integer_lit>, eof, unknown>;
} // namespace detail

class sexp : public detail::sexp_variant {
public:
  using detail::sexp_variant::sexp_variant;

  constexpr bool is_eof() const { return std::holds_alternative<eof>(*this); }
};

enum struct list_kind {
  parentheses,
  brackets,
  braces,
};
class list {
  list_kind kind_;
  std::vector<sexp> elements_;

public:
  explicit constexpr list(std::vector<sexp>&& elements)
      : kind_(list_kind::parentheses), elements_(std::move(elements)) {}

  constexpr std::span<const sexp> elements() const { return elements_; }
};

class identifier {
  std::string text_;

public:
  explicit constexpr identifier(const std::string& text) : text_(text) {}
  explicit constexpr identifier(std::string&& text) : text_(std::move(text)) {}

  constexpr std::string_view text() const { return text_; }
};

class integer_lit {
  std::variant<std::int64_t, std::string> val_;

public:
  explicit constexpr integer_lit(std::int64_t val)
      : val_(std::in_place_type<std::int64_t>, val) {}
};

template <typename Stx, typename... Args>
constexpr sexp make_sexp(Args&&... args) {
  return sexp(std::in_place_type<std::shared_ptr<Stx>>,
              std::make_shared<Stx>(static_cast<Args&&>(args)...));
}
} // namespace stx
} // namespace ely

template <> struct fmt::formatter<ely::stx::eof> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::eof&, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "EOF");
  }
};

template <> struct fmt::formatter<ely::stx::unknown> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::unknown&, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "unknown");
  }
};

template <> struct fmt::formatter<ely::stx::identifier> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::identifier& id, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "identifier({})", id.text());
  }
};

template <> struct fmt::formatter<std::shared_ptr<ely::stx::identifier>> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const std::shared_ptr<ely::stx::identifier>& id,
                        Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "{}", *id);
  }
};

template <> struct fmt::formatter<ely::stx::integer_lit> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::integer_lit& ilit, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "integer_lit(TODO)");
  }
};

template <> struct fmt::formatter<std::shared_ptr<ely::stx::integer_lit>> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const std::shared_ptr<ely::stx::integer_lit>& ilit,
                        Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "{}", *ilit);
  }
};

template <> struct fmt::formatter<ely::stx::list> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::list& l, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "list({})", fmt::join(l.elements(), ", "));
  }
};

template <> struct fmt::formatter<std::shared_ptr<ely::stx::list>> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const std::shared_ptr<ely::stx::list>& l,
                        Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "{}", *l);
  }
};

template <> struct fmt::formatter<ely::stx::sexp> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::sexp& s, Ctx& ctx) const {
    return std::visit(
        [&]<typename T>(const T& t) {
          if constexpr (ely::stx::detail::is_shared_ptr<T>::value) {
            return fmt::format_to(ctx.out(), "{}", *t);
          } else {
            return fmt::format_to(ctx.out(), "{}", t);
          }
        },
        s);
  }
};
