#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include <llvm/ADT/Hashing.h>

#include "ely/util/concepts.hpp"
#include "ely/util/hash.hpp"
#include "ely/util/traits.hpp"
#include "ely/util/tuple.hpp"
#include "ely/util/variant.hpp"
#include "ely/util/variant_hash.hpp"
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
        [&]<typename T>(const T& x) -> std::size_t {
          // we assume hashes are unique enough and index won't affect it
          return llvm_hash<T>{}(x);
        },
        v);
  }
};

template <> struct llvm_hash<std::string_view> {
  constexpr std::size_t operator()(std::string_view strv) const {
    return llvm::hash_combine_range(strv);
  }
};
} // namespace detail

template <typename T>
using get_storage_t =
    std::remove_pointer_t<decltype(std::declval<T>().get_storage())>;

template <typename T>
using get_variant_t =
    std::remove_cvref_t<decltype(std::declval<T>().get_variant())>;

template <typename T>
concept single_uniqued = requires { typename get_storage_t<T>; };

template <typename T>
concept variant_uniqued = requires { typename get_variant_t<T>; };

template <typename T>
concept uniqued = single_uniqued<T> || variant_uniqued<T>;

template <typename T>
concept llvm_hashable = requires(const T& t) {
  { detail::llvm_hash<T>{}(t) } -> std::same_as<std::size_t>;
};

template <typename StorageT>
concept storage = requires { typename StorageT::key_type; } &&
                  llvm_hashable<typename StorageT::key_type>;

template <typename StorageT, typename... Args>
concept storage_key_from =
    storage<StorageT> && std::constructible_from<StorageT, Args...> &&
    requires(Args&&... args) {
      {
        StorageT::get_key(static_cast<Args&&>(args)...)
      } -> std::same_as<typename StorageT::key_type>;
    };

template <typename... StorageTs> class variant_storage_uniquer {
public:
  template <typename... Us> using variant_type = ely::variant<Us...>;
  using key_variant_type = variant_type<typename StorageTs::key_type...>;

private:
  std::unordered_map<variant_type<typename StorageTs::key_type...>,
                     variant_type<std::unique_ptr<StorageTs>...>>
      storage_map_;

public:
  variant_storage_uniquer() = default;

  template <any_of<StorageTs...> StorageT, typename... Args>
    requires(storage_key_from<StorageT, Args...>)
  [[nodiscard]] const StorageT* get_or_emplace(Args&&... args) {
    auto key = StorageT::get_key(static_cast<Args&&>(args)...);
    return get_or_emplace_k<StorageT>(key, static_cast<Args&&>(args)...);
  }

  template <any_of<StorageTs...> StorageT, typename... Args>
    requires(storage_key_from<StorageT, Args...>)
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

  template <any_of<StorageTs...> StorageT, typename... Args>
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

  template <any_of<StorageTs...> StorageT, typename... Args>
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

  template <any_of<StorageTs...> StorageT, typename... Args>
  [[nodiscard]] const StorageT* try_get(Args&&... args) {
    auto key = StorageT::get_key(static_cast<Args&&>(args)...);
    return try_get_k<StorageT>(key, static_cast<Args&&>(args)...);
  }

  template <any_of<StorageTs...> StorageT, typename... Args>
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

  template <any_of<StorageTs...> StorageT, typename... Args>
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

  template <any_of<StorageTs...> StorageT, typename... Args>
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

template <typename T> struct get_storage_uniquer;

template <ely::single_uniqued T> struct get_storage_uniquer<T> {
  using type = storage_uniquer<get_storage_t<T>>;
};

namespace detail {
template <typename T> struct get_storage_uniquer_impl;

template <typename... Ts> struct get_storage_uniquer_impl<ely::variant<Ts...>> {
  using type = ely::variant_storage_uniquer<ely::get_storage_t<Ts>...>;
};
} // namespace detail

template <ely::variant_uniqued T> struct get_storage_uniquer<T> {
  using type =
      typename detail::get_storage_uniquer_impl<ely::get_variant_t<T>>::type;
};

template <ely::uniqued T> class uniquer {
public:
  using value_type = T;

private:
  typename get_storage_uniquer<T>::type impl_;
  // storage_uniquer<storage_type> impl_;

public:
  uniquer() = default;

  template <typename... Args>
  [[nodiscard]] value_type get_or_emplace(Args&&... args) {
    return value_type(impl_.get_or_emplace(static_cast<Args&&>(args)...));
  }

  template <typename U, typename... Args>
    requires(std::constructible_from<value_type, U>)
  [[nodiscard]] U get_or_emplace(Args&&... args) {
    return U(impl_.template get_or_emplace<ely::get_storage_t<U>>(
        static_cast<Args&&>(args)...));
  }
};
} // namespace ely
