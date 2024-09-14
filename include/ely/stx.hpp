#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

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
using sexp_variant =
    std::variant<std::shared_ptr<stx::list>, std::shared_ptr<stx::identifier>,
                 std::shared_ptr<stx::integer_lit>, eof, unknown>;
}

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