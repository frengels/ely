#pragma once

namespace ely {

template <typename To, typename From> constexpr bool isa(const From& f) {
  return To::classof(f);
}

template <typename To, typename From> constexpr To* dyn_cast(From& f) {
  if (To::classof(f))
    return static_cast<To*>(&f);
}

template <typename To, typename From>
constexpr const To* dyn_cast(const From& f) {
  if (To::classof(f))
    return static_cast<const To*>(&f);
}

template <typename To, typename From> constexpr To* dyn_cast(From* f) {
  return dyn_cast<To>(*f);
}

template <typename To, typename From>
constexpr const To* dyn_cast(const From* f) {
  return dyn_cast<To>(*f);
}
} // namespace ely