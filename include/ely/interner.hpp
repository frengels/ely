#pragma once

#include <cstdint>
#include <forward_list>
#include <limits>
#include <llvm/ADT/Hashing.h>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ely/arena/block.hpp"
#include "ely/symbol.hpp"

namespace ely {
// a simple interner implementation, currently doesn't do anything optimal
template <typename CharT, typename Traits = std::char_traits<CharT>,
          typename Arena = ely::arena::fixed_block<CharT, 256 * 1024>>
class basic_simple_interner {
public:
  using char_type = CharT;
  using symbol_type = ely::symbol;
  using string_view_type = std::basic_string_view<CharT, Traits>;

private:
  std::unordered_map<string_view_type, symbol_type> map_;
  std::vector<string_view_type> ref_;
  Arena* alloc_;

public:
  constexpr basic_simple_interner(Arena& alloc)
      : alloc_(std::addressof(alloc)) {}

  constexpr symbol_type intern(string_view_type strv) {
    auto it = map_.find(strv);
    if (it != map_.end())
      return it->second;

    char_type* p = alloc_->allocate(strv.size());
    std::uninitialized_copy(strv.begin(), strv.end(), p);
    string_view_type internal = string_view_type{p, strv.size()};
    // std::string& ref = buffer_.emplace_front(strv);

    // add references to this location in both the map and ref
    auto id = symbol_type{static_cast<std::uint32_t>(ref_.size() + 1)};
    ref_.emplace_back(internal);
    map_.emplace(std::piecewise_construct, std::forward_as_tuple(internal),
                 std::forward_as_tuple(id));
    return id;
  }

  constexpr string_view_type lookup(symbol_type sym) const {
    return ref_[static_cast<std::size_t>(sym.id) - 1];
  }
};

template <typename Ptr, typename T>
concept dereference_to =
    std::same_as<typename std::pointer_traits<Ptr>::element_type, T> &&
    requires(Ptr& p) {
      typename std::pointer_traits<Ptr>::element_type;
      typename std::pointer_traits<Ptr>::pointer;
      { std::to_address(p) } -> std::same_as<T*>;
      { *p } -> std::same_as<T&>;
    };

static_assert(dereference_to<int*, int>);
static_assert(dereference_to<std::unique_ptr<int>, int>);

template <typename T, typename... Args>
concept creates_storage = requires(Args&&... args) {
  {
    T::create_storage(static_cast<Args&&>(args)...)
  } -> dereference_to<typename T::storage_type>;
};

template <typename Alloc>
using simple_interner =
    basic_simple_interner<char, std::char_traits<char>, Alloc>;
} // namespace ely
