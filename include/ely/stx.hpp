#pragma once

#include <boost/intrusive/list_hook.hpp>
#include <ranges>
#include <span>
#include <string>
#include <variant>
#include <vector>

#include <fmt/format.h>
#include <fmt/ranges.h>

#include <boost/intrusive/list.hpp>

#include "ely/symbol.hpp"

namespace ely {
namespace stx {
class sexp;
class list;

namespace detail {
class sexp_base
    : public boost::intrusive::list_base_hook<boost::intrusive::link_mode<
          boost::intrusive::link_mode_type::normal_link>> {};
} // namespace detail

class list {
  boost::intrusive::list<detail::sexp_base> impl_;

public:
  list() = default;

  template <typename It, typename Sent>
  constexpr list(It begin, Sent end) : list() {
    for (auto it = begin; it != end; ++it)
      impl_.push_back(**it);
  }

  constexpr auto begin() const;
  constexpr auto end() const;

  constexpr const sexp& head() const;

  constexpr void push_front(stx::sexp*);
  constexpr void push_back(stx::sexp*);
};

class identifier {
  ely::symbol sym_;

public:
  explicit constexpr identifier(ely::symbol sym) : sym_(sym) {}

  constexpr ely::symbol sym() const { return sym_; }
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
  std::string_view text_;

public:
  explicit constexpr integer_lit(std::string_view text) : text_(text) {}

  constexpr std::string_view text() const { return text_; }
};

class decimal_lit {
  std::string_view text_;

public:
  explicit constexpr decimal_lit(std::string_view text) : text_(text) {}

  constexpr std::string_view text() const { return text_; }
};

class string_lit {
  std::string_view text_;

public:
  explicit constexpr string_lit(std::string_view text) : text_(text) {}

  constexpr std::string_view text() const { return text_; }
};

class unterminated_string_lit {
  std::string_view text_;

public:
  explicit constexpr unterminated_string_lit(std::string_view text)
      : text_(text) {}

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

class sexp : public detail::sexp_base {
private:
  // TODO: add actual syntax information like span etc
  std::variant<unknown, eof, stx::list, stx::identifier, stx::integer_lit,
               stx::decimal_lit, stx::string_lit, stx::unterminated_string_lit>
      impl_;

public:
  sexp() = default;

  template <typename T, typename... Args>
  constexpr sexp(std::in_place_type_t<T> ipt, Args&&... args)
      : impl_(ipt, static_cast<Args&&>(args)...) {}

  // copies shouldn't be done here
  sexp(const sexp&) = delete;
  sexp& operator=(const sexp&) = delete;

  // cannot be moved
  sexp(sexp&&) = delete;
  sexp& operator=(sexp&&) = delete;

  template <typename T> constexpr bool isa() const {
    return std::holds_alternative<T>(impl_);
  }

  constexpr bool is_eof() const { return isa<eof>(); }
  constexpr bool is_list() const { return isa<list>(); }
  constexpr bool is_identifier() const { return isa<identifier>(); }

  template <typename F, typename S>
  friend constexpr auto visit(F&& f, S&& s)
      -> std::enable_if_t<std::is_same_v<sexp, std::remove_cvref_t<S>>,
                          decltype(std::visit(static_cast<F&&>(f),
                                              static_cast<S&&>(s).impl_))> {
    return std::visit(static_cast<F&&>(f), static_cast<S&&>(s).impl_);
  }

  template <typename T> constexpr const T* as() const {
    return visit(
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

  constexpr const stx::identifier* as_identifier() const {
    return as<stx::identifier>();
  }
  constexpr stx::identifier* as_identifier() { return as<stx::identifier>(); }

  constexpr const stx::list* as_list() const { return as<stx::list>(); }
  constexpr stx::list* as_list() { return as<stx::list>(); }

public:
  template <typename T, typename Arena, typename... Args>
  static constexpr sexp* create(Arena& a, Args&&... args) {
    sexp* res = a.allocate(1);
    a.construct(res, std::in_place_type<T>, static_cast<Args&&>(args)...);
    return res;
  }

  template <typename Arena>
  static constexpr sexp* create_copy(Arena& a, const sexp& other) {
    sexp* res = a.allocate(1);
    visit(
        [&]<typename T>(const T& val) {
          if constexpr (std::is_same_v<stx::list, T>) {
            // list requires copying all members of the list
            for (const auto& list_member : val) {
              create_copy(a, list_member);
            }
          } else {
            a.construct(res, std::in_place_type<T>, val);
          }
        },
        other);
    return res;
  }
};

namespace detail {
template <typename I, typename S> constexpr auto make_cast_range(I it, S end) {
  return std::ranges::subrange{it, end} |
         std::ranges::views::transform(
             [](const detail::sexp_base& sb) -> const sexp& {
               return static_cast<const sexp&>(sb);
             });
}
} // namespace detail

constexpr auto list::begin() const {
  return detail::make_cast_range(impl_.begin(), impl_.end()).begin();
}
constexpr auto list::end() const {
  return detail::make_cast_range(impl_.begin(), impl_.end()).end();
}

constexpr const sexp& list::head() const {
  return *static_cast<const sexp*>(std::addressof(*begin()));
}
constexpr void list::push_front(stx::sexp* s) { impl_.push_front(*s); }
constexpr void list::push_back(stx::sexp* s) { impl_.push_back(*s); }
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
    return fmt::format_to(ctx.out(), "identifier({})", id.sym());
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
    return fmt::format_to(ctx.out(), "list({})",
                          fmt::join(l.begin(), l.end(), ", "));
  }
};

template <> struct fmt::formatter<ely::stx::sexp> {
  constexpr auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
  template <typename Ctx>
  constexpr auto format(const ely::stx::sexp& s, Ctx& ctx) const {
    using std::visit;
    return visit([&]<typename T>(
                     const T& t) { return fmt::format_to(ctx.out(), "{}", t); },
                 s);
  }
};
