#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include <llvm/ADT/Hashing.h>

#include "ely/util/concepts.hpp"
#include "ely/util/traits.hpp"
#include "ely/util/variant.hpp"
#include "ely/util/visit.hpp"

namespace ely {
namespace detail {
template <typename T> struct llvm_hash {
  constexpr std::size_t operator()(const T& t) const {
    using llvm::hash_value;
    return hash_value(t);
  }
};

// TODO: this shouldn't be required as the hash simply gets forwarded to the
// member and therefore a variant shouldn't have to be created before
// constructing a hash
template <typename... Ts> struct llvm_hash<::ely::variant<Ts...>> {
  constexpr std::size_t operator()(const ::ely::variant<Ts...>& v) const {
    using llvm::hash_value;
    return ely::visit(
        [&](const auto& x) -> std::size_t {
          // we assume hashes are unique enough and index won't affect it
          return hash_value(x);
        },
        v);
  }
};
} // namespace detail

template <typename... StorageTs> class variant_storage_uniquer {
public:
  template <typename... Us> using variant_type = ely::variant<Us...>;
  using key_variant_type = variant_type<typename StorageTs::key_type...>;

private:
  std::unordered_map<
      variant_type<typename StorageTs::key_type...>,
      variant_type<std::unique_ptr<StorageTs>...>,
      detail::llvm_hash<variant_type<typename StorageTs::key_type...>>>
      storage_map_;

public:
  variant_storage_uniquer() = default;

  template <any_of<StorageTs...> StorageT, typename... Args>
    requires(std::constructible_from<StorageT, Args...>)
  [[nodiscard]] const StorageT* get_or_emplace(Args&&... args) {
    auto key = StorageT::get_key(static_cast<Args&&>(args)...);
    return get_or_emplace_k<StorageT>(key, static_cast<Args&&>(args)...);
  }

  template <typename StorageT, typename... Args>
  [[nodiscard]] const StorageT* get_or_emplace(std::in_place_type_t<StorageT>,
                                               Args&&... args) {
    return get_or_emplace<StorageT>(static_cast<Args&&>(args)...);
  }

  template <typename... Args>
    requires(sizeof...(StorageTs) == 1)
  [[nodiscard]] const first_element_t<StorageTs...>*
  get_or_emplace(Args&&... args) {
    return get_or_emplace<first_element_t<StorageTs...>>(
        static_cast<Args&&>(args)...);
  }

  template <typename StorageT, typename... Args>
  [[nodiscard]] const StorageT*
  get_or_emplace_k(const typename StorageT::key_type& key, Args&&... args) {
    const StorageT* res =
        try_get_k<StorageT>(key, static_cast<Args&&>(args)...);
    if (!res) {
      auto storage = std::make_unique<StorageT>(static_cast<Args&&>(args)...);
      res = storage.get();
      storage_map_.emplace(
          std::piecewise_construct,
          std::make_tuple(std::in_place_type<typename StorageT::key_type>, key),
          std::forward_as_tuple(std::in_place_type<std::unique_ptr<StorageT>>,
                                std::move(storage)));
    }

    return res;
  }

  template <typename StorageT, typename... Args>
  [[nodiscard]] const StorageT*
  get_or_emplace_k(std::in_place_type_t<StorageT>,
                   const typename StorageT::key_type& key, Args&&... args) {
    return get_or_emplace_k<StorageT>(key, static_cast<Args&&>(args)...);
  }

  template <typename... Args>
    requires(sizeof...(StorageTs) == 1)
  [[nodiscard]] const first_element_t<StorageTs...>*
  get_or_emplace_k(const typename first_element_t<StorageTs...>::key_type& key,
                   Args&&... args) {
    return get_or_emplace_k<first_element_t<StorageTs...>>(
        key, static_cast<Args&&>(args)...);
  }

  template <typename StorageT, typename... Args>
  [[nodiscard]] const StorageT* try_get(Args&&... args) {
    auto key = StorageT::get_key(static_cast<Args&&>(args)...);
    return try_get_k<StorageT>(key, static_cast<Args&&>(args)...);
  }

  template <typename StorageT, typename... Args>
  [[nodiscard]] const StorageT* try_get(std::in_place_type_t<StorageT>,
                                        Args&&... args) {
    return try_get<StorageT>(static_cast<Args&&>(args)...);
  }

  template <typename... Args>
    requires(sizeof...(StorageTs) == 1)
  [[nodiscard]] const first_element_t<StorageTs...>* try_get(Args&&... args) {
    return try_get(std::in_place_type<first_element_t<StorageTs...>>,
                   static_cast<Args&&>(args)...);
  }

  template <typename StorageT, typename... Args>
  [[nodiscard]] const StorageT*
  try_get_k(const typename StorageT::key_type& key, Args&&... args) {
    // TODO: remove construction to variant, shouldn't be required
    auto it = storage_map_.find(
        key_variant_type(std::in_place_type<typename StorageT::key_type>, key));
    if (it != storage_map_.end()) {
      return ely::get_unchecked<std::unique_ptr<StorageT>>(it->second).get();
    }
    return nullptr;
  }

  template <typename StorageT, typename... Args>
  [[nodiscard]] const StorageT*
  try_get_k(std::in_place_type_t<StorageT>,
            const typename StorageT::key_type& key, Args&&... args) {
    return try_get_k<StorageT>(key, static_cast<Args&&>(args)...);
  }

  template <typename... Args>
    requires(sizeof...(StorageTs) == 1)
  [[nodiscard]] const first_element_t<StorageTs...>*
  try_get_k(const typename first_element_t<StorageTs...>::key_type& key,
            Args&&... args) {
    return try_get_k<first_element_t<StorageTs...>>(
        key, static_cast<Args&&>(args)...);
  }
};

template <typename T> using storage_uniquer = variant_storage_uniquer<T>;

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
