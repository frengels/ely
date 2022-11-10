#pragma once

#include <functional>
#include <memory>
#include <variant>

#include <cstdint>

#include "ely/ilist.hpp"

#include "ely/node/stx.hpp"

namespace ely
{
using node_variant = std::variant<stx::id_prim,
                                  stx::id_user,
                                  stx::lit_int,
                                  stx::lit_dec,
                                  stx::lit_string,
                                  stx::list>;

class node : private ely::ilink
{
    node_variant impl_;

public:
    node() = default;
};
} // namespace ely