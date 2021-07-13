#pragma once

#include "ely/lex/tokens.hpp"

namespace ely
{
namespace lexeme
{
using variant_type = token2::token_types::template apply_each<
    std::in_place_type_t>::template apply_all<ely::Variant>;

} // namespace lexeme
} // namespace ely