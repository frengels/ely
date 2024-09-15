#pragma once

#include <span>
#include <string>
#include <variant>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>

namespace ely {
namespace stx {
class sexp;
class list {
  std::vector<sexp> elements_;

public:
  explicit constexpr list(std::vector<sexp>&& elements)
      : elements_(std::move(elements)) {}

  template <typename It>
  constexpr list(It begin, It end) : elements_(begin, end) {}

  constexpr std::span<const sexp> elements() const;
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

class eof {
public:
  eof() = default;
};

class unknown {
public:
  unknown() = default;
};

namespace detail {
using sexp_variant =
    std::variant<stx::list, stx::identifier, stx::integer_lit, stx::decimal_lit,
                 stx::string_lit, stx::unterminated_string_lit, eof, unknown>;
} // namespace detail

class sexp : public detail::sexp_variant {
public:
  using detail::sexp_variant::sexp_variant;

  constexpr bool is_eof() const { return std::holds_alternative<eof>(*this); }
  constexpr bool is_list() const { return std::holds_alternative<list>(*this); }

  constexpr bool is_identifier() const {
    return std::holds_alternative<identifier>(*this);
  }
};

template <typename Stx, typename... Args>
constexpr sexp make_sexp(Args&&... args) {
  return sexp(std::in_place_type<Stx>, static_cast<Args&&>(args)...);
}

constexpr std::span<const sexp> list::elements() const { return elements_; }

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

template <> struct fmt::formatter<ely::stx::list> {
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
          return fmt::format_to(ctx.out(), "{}", t);
        },
        s);
  }
};
