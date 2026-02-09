#pragma once

#include <cstddef>
#include <utility>

#include "ely/config.h"
#include "ely/stx/tokens.hpp"

#include "cont.hpp"

namespace ely {
namespace stx {

template <token_kind Kind> struct encode_fn {};

template <> struct encode_fn<token_kind::whitespace> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out,
                                                     std::uint8_t num) const {
    *out++ = static_cast<std::uint8_t>(token_kind::whitespace);
    *out = num;
    return 2;
  }
};

template <> struct encode_fn<token_kind::tab> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out,
                                                     std::uint8_t num) const {
    *out++ = static_cast<std::uint8_t>(token_kind::tab);
    *out = num;
    return 2;
  }
};

template <> struct encode_fn<token_kind::eof> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::eof);
    return 1;
  }
};

template <> struct encode_fn<token_kind::buffer_full> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::buffer_full);
    return 1;
  }
};

template <> struct encode_fn<token_kind::unknown> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out,
                                                     std::uint8_t len) const {
    *out++ = static_cast<std::uint8_t>(token_kind::unknown);
    *out = len;
    return 2;
  }
};

template <> struct encode_fn<token_kind::spill> {
  ELY_ALWAYS_INLINE constexpr std::size_t
  operator()(std::uint8_t* out, std::uint8_t num, std::uint8_t cont_id) const {
    *out++ = (uint8_t)num;
    *out++ = (uint8_t)cont_id;
    *out = static_cast<std::uint8_t>(token_kind::spill);
    return 3;
  }

  ELY_ALWAYS_INLINE constexpr std::size_t
  operator()(std::uint8_t* out, std::uint8_t num,
             ely::stx::cont cont_id) const {
    *out++ = (uint8_t)num;
    *out++ = (uint8_t)cont_id;
    *out = std::to_underlying(token_kind::spill);
    return 3;
  }
};

template <> struct encode_fn<token_kind::identifier> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out,
                                                     std::uint8_t num) const {
    *out++ = static_cast<std::uint8_t>(token_kind::identifier);
    *out = num;
    return 2;
  }
};

template <> struct encode_fn<token_kind::integer_lit> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out,
                                                     std::uint8_t num) const {
    *out++ = static_cast<std::uint8_t>(token_kind::integer_lit);
    *out = num;
    return 2;
  }
};

template <> struct encode_fn<token_kind::decimal_lit> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out,
                                                     std::uint8_t num) const {
    *out++ = static_cast<std::uint8_t>(token_kind::decimal_lit);
    *out = num;
    return 2;
  }
};

template <> struct encode_fn<token_kind::string_lit> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out,
                                                     std::uint8_t num) const {
    *out++ = static_cast<std::uint8_t>(token_kind::string_lit);
    *out = num;
    return 2;
  }
};

template <> struct encode_fn<token_kind::line_comment> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out,
                                                     std::uint8_t num) const {
    *out++ = static_cast<std::uint8_t>(token_kind::line_comment);
    *out = num;
    return 2;
  }
};

template <> struct encode_fn<token_kind::block_comment> {
  ELY_ALWAYS_INLINE constexpr std::size_t
  operator()(std::uint8_t* out, std::uint8_t num, std::uint8_t newlines) const {
    *out++ = static_cast<std::uint8_t>(token_kind::block_comment);
    *out++ = num;
    *out = newlines;
    return 3;
  }
};

template <> struct encode_fn<token_kind::lparen> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::lparen);
    return 1;
  }
};

template <> struct encode_fn<token_kind::rparen> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::rparen);
    return 1;
  }
};

template <> struct encode_fn<token_kind::lbracket> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::lbracket);
    return 1;
  }
};

template <> struct encode_fn<token_kind::rbracket> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::rbracket);
    return 1;
  }
};

template <> struct encode_fn<token_kind::lbrace> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::lbrace);
    return 1;
  }
};

template <> struct encode_fn<token_kind::rbrace> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::rbrace);
    return 1;
  }
};

template <> struct encode_fn<token_kind::slash> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::slash);
    return 1;
  }
};

template <> struct encode_fn<token_kind::newline_crlf> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::newline_crlf);
    return 1;
  }
};

template <> struct encode_fn<token_kind::newline_cr> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::newline_cr);
    return 1;
  }
};

template <> struct encode_fn<token_kind::newline_lf> {
  ELY_ALWAYS_INLINE constexpr std::size_t operator()(std::uint8_t* out) const {
    *out = static_cast<std::uint8_t>(token_kind::newline_lf);
    return 1;
  }
};

template <token_kind Kind> inline constexpr auto encode = encode_fn<Kind>{};
} // namespace stx
} // namespace ely