#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ely/arena/growing.hpp"
#include "ely/hash/fnv.hpp"
#include "ely/symbol.hpp"
#include "ely/util/cx_or_rt.hpp"
#include "ely/util/optional.hpp"

namespace ely {
// a simple interner implementation, currently doesn't do anything optimal
template <typename CharT, typename Traits = std::char_traits<CharT>>
class basic_simple_interner {
public:
  using char_type = CharT;
  using symbol_type = ely::symbol;
  using string_view_type = std::basic_string_view<CharT, Traits>;

private:
  ely::cx_or_rt<std::vector<std::pair<string_view_type, symbol_type>>,
                std::unordered_map<string_view_type, symbol_type>>
      map_storage_;
  // std::unordered_map<string_view_type, symbol_type> map_;
  std::vector<string_view_type> ref_;
  ely::arena::growing arena_{4096};

public:
  basic_simple_interner() = default;

  constexpr symbol_type intern(string_view_type strv) {
    return map_storage_
        .visit(
            [&](const auto& vec) -> ely::optional<symbol_type> {
              for (auto target : vec) {
                if (strv == target.first) {
                  return target.second;
                }
              }

              return ely::nullopt;
            },
            [&](const auto& map) -> ely::optional<symbol_type> {
              auto it = map.find(strv);
              if (it != map.end()) {
                return it->second;
              }

              return ely::nullopt;
            })
        .value_or_else([&]() {
          char_type* p = arena_.allocate<char>(strv.size());
          std::copy(strv.begin(), strv.end(), p);
          string_view_type internal = string_view_type{p, strv.size()};

          auto id = symbol_type{static_cast<std::uint32_t>(ref_.size())};
          ref_.emplace_back(internal);

          map_storage_.visit(
              [&](std::vector<std::pair<string_view_type, symbol_type>>& vec) {
                vec.emplace_back(internal, id);
              },
              [&](auto& map) {
                map.emplace(std::piecewise_construct,
                            std::forward_as_tuple(internal),
                            std::forward_as_tuple(id));
              });
          return id;
        });
  }

  constexpr string_view_type lookup(symbol_type sym) const {
    return ref_[static_cast<std::size_t>(sym.id)];
  }
};

using simple_interner = basic_simple_interner<char, std::char_traits<char>>;
} // namespace ely
