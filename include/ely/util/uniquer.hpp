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

template <typename StorageT> class storage_uniquer {
public:
  using storage_type = StorageT;
  using key_type = typename storage_type::key_type;

private:
  std::unordered_map<key_type, std::unique_ptr<storage_type>,
                     detail::llvm_hash<key_type>>
      storage_map_;

public:
  storage_uniquer() = default;

  template <typename... Args>
  [[nodiscard]] storage_type* get_or_emplace(Args&&... args) {
    key_type key = storage_type::get_key(static_cast<Args&&>(args)...);
    return get_or_emplace_k(key, static_cast<Args&&>(args)...);
  }

  template <typename... Args>
  [[nodiscard]] storage_type* get_or_emplace_k(const key_type& key,
                                               Args&&... args) {
    storage_type* res = try_get_k(key, static_cast<Args&&>(args)...);
    if (!res) {
      std::unique_ptr<storage_type> storage =
          std::make_unique<storage_type>(static_cast<Args&&>(args)...);

      res = storage.get();
      storage_map_.emplace(key, std::move(storage));
    }

    return res;
  }

  template <typename... Args>
  [[nodiscard]] storage_type* try_get(Args&&... args) {
    key_type key = storage_type::get_key(static_cast<Args&&>(args)...);
    return try_get_k(key, static_cast<Args&&>(args)...);
  }

  template <typename... Args>
  [[nodiscard]] storage_type* try_get_k(const key_type& key, Args&&... args) {
    auto it = storage_map_.find(key);
    if (it != storage_map_.end()) {
      return it->second.get();
    }

    return nullptr;
  }
};

template <typename T> class uniquer {
public:
  using value_type = T;
  using storage_type = typename T::storage_type;
  using key_type = typename storage_type::key_type;

private:
  storage_uniquer<storage_type> impl_;

public:
  uniquer() = default;

  template <typename... Args>
  [[nodiscard]] value_type get_or_emplace(Args&&... args) {
    return value_type(impl_.get_or_emplace(static_cast<Args&&>(args)...));
  }
};
} // namespace ely