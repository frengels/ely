#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include <llvm/ADT/Hashing.h>

namespace ely {
namespace detail {
template <typename T> struct llvm_hash {
  constexpr std::size_t operator()(const T& t) const {
    using llvm::hash_value;
    return hash_value(t);
  }
};
} // namespace detail

template <typename T> class uniquer {
public:
  using storage_type = typename T::storage_type;
  using key_type = typename storage_type::key_type;

private:
  std::unordered_map<key_type, std::unique_ptr<storage_type>,
                     detail::llvm_hash<key_type>>
      storage_map_;

public:
  uniquer() = default;

  template <typename... Args> [[nodiscard]] T get_or_create(Args&&... args) {
    key_type key = storage_type::get_key(static_cast<Args&&>(args)...);
    auto it = storage_map_.find(key);
    if (it != storage_map_.end()) {
      return T(it->second.get());
    }

    std::unique_ptr<storage_type> storage =
        std::make_unique<storage_type>(static_cast<Args&&>(args)...);

    storage_type* p = storage.get();
    storage_map_.emplace(key, std::move(storage));
    return T(p);
  }
};
} // namespace ely