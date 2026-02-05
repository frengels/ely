#pragma once

#include <source_location>

#include <fmt/format.h>

#ifndef ELY_DBG_VERBOSE
#define ELY_DBG_VERBOSE 0
#endif

#if ELY_DBG_VERBOSE
#define ELY_DBG(...) [&]() { __VA_ARGS__; }()
#else
// still get syntax validated, just not run
#define ELY_DBG(...) [&]() { __VA_ARGS__; }
#endif

namespace ely {
namespace detail {
template <typename... Ts>
[[noreturn]] constexpr void unimplemented(std::source_location loc,
                                          fmt::format_string<Ts...> fmt,
                                          Ts&&... ts) {
  if (!std::is_constant_evaluated()) {
    fmt::print("error (unimplemented): {}({}:{}): `{}`: ", loc.file_name(),
               loc.line(), loc.column(), loc.function_name());
    fmt::println(fmt, static_cast<Ts&&>(ts)...);
  }
  std::abort();
}
} // namespace detail
} // namespace ely

#ifndef ELY_UNIMPLEMENTED
#define ELY_UNIMPLEMENTED(fmt, ...)                                            \
  ::ely::detail::unimplemented(std::source_location::current(), fmt,           \
                               ##__VA_ARGS__)
#endif
