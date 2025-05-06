#pragma once

#include <cstdint>
#include <forward_list>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace ely {
// a simple interner implementation, currently doesn't do anything optimal
template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_simple_interner {
public:
  using char_type = CharT;
  using symbol_type = std::uint32_t;

private:
  std::unordered_map<std::string_view, symbol_type> map_;
  std::vector<std::string_view> ref_;

  // TODO: replace with an arena
  // using forward list right now for the constant memory location
  std::forward_list<std::string> buffer_;

public:
  basic_simple_interner() = default;

  constexpr symbol_type intern(std::string_view strv) {
    auto it = map_.find(strv);
    if (it != map_.end())
      return it->second;

    std::string& ref = buffer_.emplace_front(strv);

    // add references to this location in both the map and ref
    symbol_type id = ref_.size();
    ref_.emplace_back(ref);
    map_.emplace(std::piecewise_construct, std::forward_as_tuple(ref),
                 std::forward_as_tuple(id));
    return id;
  }

  constexpr std::string_view lookup(symbol_type sym) {
    return ref_[static_cast<std::size_t>(sym)];
  }
};

using simple_interner = basic_simple_interner<char>;
} // namespace ely
