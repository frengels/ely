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
  list() = default;
  explicit constexpr list(std::vector<sexp>&& elements)
      : elements_(std::move(elements)) {}

  template <typename It>
  constexpr list(It begin, It end) : elements_(begin, end) {}

  constexpr std::span<const sexp> elements() const;
  constexpr const sexp& head() const;
  constexpr std::span<const sexp> tail() const;

  template <typename... Args> constexpr sexp& emplace_front(Args&&... args) {
    return *elements_.emplace(elements_.begin(), static_cast<Args&&>(args)...);
  }

  template <typename... Args> constexpr sexp& emplace_back(Args&&... args) {
    return elements_.emplace_back(static_cast<Args&&>(args)...);
  }
};

class identifier {
  std::string text_;

public:
  explicit constexpr identifier(const std::string& text) : text_(text) {}
  explicit constexpr identifier(std::string&& text) : text_(std::move(text)) {}

  constexpr std::string_view text() const { return text_; }
};

class path {
  std::vector<sexp> elements_;

public:
  explicit path(std::vector<sexp> elements) : elements_(std::move(elements)) {}

  constexpr std::span<const sexp> elements() const;

  template <typename... Args> constexpr sexp& emplace_back(Args&&... args) {
    return elements_.emplace_back(static_cast<Args&&>(args)...);
  }
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
    std::variant<unknown, eof, stx::list, stx::identifier, stx::integer_lit,
                 stx::decimal_lit, stx::string_lit,
                 stx::unterminated_string_lit>;
} // namespace detail

class sexp : public detail::sexp_variant {
public:
  using detail::sexp_variant::sexp_variant;

  constexpr sexp() : detail::sexp_variant(std::in_place_type<unknown>) {}

  template <typename T> constexpr bool isa() const {
    return std::holds_alternative<T>(*this);
  }

  constexpr bool is_eof() const { return isa<eof>(); }
  constexpr bool is_list() const { return isa<list>(); }
  constexpr bool is_identifier() const { return isa<identifier>(); }

  template <typename T> constexpr const T* as() const {
    return std::visit(
        []<typename U>(const U& u) -> const T* {
          if constexpr (std::is_same_v<U, T>) {
            return std::addressof(u);
          } else {
            return nullptr;
          }
        },
        *this);
  }

  template <typename T> constexpr T* as() {
    return const_cast<T*>(const_cast<const sexp&>(*this).as<T>());
  }

  constexpr const stx::list* as_list() const { return as<stx::list>(); }
  constexpr stx::list* as_list() { return as<stx::list>(); }
};

template <typename Stx, typename... Args>
constexpr sexp make_sexp(Args&&... args) {
  return sexp(std::in_place_type<Stx>, static_cast<Args&&>(args)...);
}

constexpr std::span<const sexp> list::elements() const { return elements_; }
constexpr const sexp& list::head() const { return elements().front(); }
constexpr std::span<const sexp> list::tail() const {
  return elements().subspan(1);
}

constexpr std::span<const sexp> path::elements() const { return elements_; }
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

template <> struct fmt::formatter<ely::stx::path> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::path& p, Ctx& ctx) const {
    return fmt::format_to(ctx.out(), "path({})", fmt::join(p.elements(), ", "));
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
