#include "ely/lex/token.hpp"

namespace ely
{
const char* token_type_to_string(token_type ty)
{
    switch (ty)
    {
#define X(val, str)                                                            \
    case token_type::val:                                                                  \
        return str;
#include "ely/lex/token.def"
#undef X
    default:
        return "<unknown>";
    }
}
} // namespace ely