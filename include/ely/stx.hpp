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
class decimal_lit;
class string_lit;
class unterminated_string_lit;

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
                 std::shared_ptr<stx::integer_lit>,
                 std::shared_ptr<stx::decimal_lit>,
                 std::shared_ptr<stx::string_lit>,
                 std::shared_ptr<stx::unterminated_string_lit>, eof, unknown>;
} // namespace detail

class sexp : public detail::sexp_variant {
public:
  using detail::sexp_variant::sexp_variant;

  constexpr bool is_eof() const { return std::holds_alternative<eof>(*this); }
  constexpr bool is_list() const {
    return std::holds_alternative<std::shared_ptr<list>>(*this);
  }
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
  std::string text_;

public:
  explicit constexpr integer_lit(std::string&& text) : text_(std::move(text)) {}

  constexpr std::string_view text() const { return text_; }
};

class decimal_lit {
  std::string text_;

public:
  explicit constexpr decimal_lit(std::string&& text) : text_(std::move(text)) {}

  constexpr std::string_view text() const { return text_; }
};

class string_lit {
  std::string text_;

public:
  explicit constexpr string_lit(std::string&& text) : text_(std::move(text)) {}

  constexpr std::string_view text() const { return text_; }
};

class unterminated_string_lit {
  std::string text_;

public:
  explicit constexpr unterminated_string_lit(std::string&& text)
      : text_(std::move(text)) {}

  constexpr std::string_view text() const { return text_; }
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

template <> struct fmt::formatter<ely::stx::integer_lit> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::integer_lit& ilit, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "integer_lit({})", ilit.text());
  }
};

template <> struct fmt::formatter<ely::stx::decimal_lit> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::decimal_lit& dlit, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "decimal_lit({})", dlit.text());
  }
};

template <> struct fmt::formatter<ely::stx::string_lit> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::string_lit& str_lit, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "string_lit({})", str_lit.text());
  }
};

template <> struct fmt::formatter<ely::stx::unterminated_string_lit> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::unterminated_string_lit& str_lit,
                        Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "unterminated_string_lit({})",
                          str_lit.text());
  }
};

template <>
struct fmt::formatter<ely::stx::list> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::list& l, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "list({})", fmt::join(l.elements(), ", "));
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
