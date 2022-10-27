#include "ely/value.h"

#include <cinttypes>
#include <cstdio>

#include <algorithm>
#include <charconv>

size_t ely_value_to_chars(const struct ely_value* v, char* buf, size_t buf_len)
{
    std::to_chars_result res;

    switch (v->kind)
    {
    case ELY_VALUE_F32:
        res = std::to_chars(
            buf, buf + buf_len, v->as.f, std::chars_format::fixed);
        break;
    case ELY_VALUE_F64:
        res = std::to_chars(
            buf, buf + buf_len, v->as.d, std::chars_format::fixed);
        break;
    case ELY_VALUE_U64:
    case ELY_VALUE_U32:
        res = std::to_chars(buf, buf + buf_len, v->as.uval);
        break;
    case ELY_VALUE_S64:
    case ELY_VALUE_S32:
        res = std::to_chars(buf, buf + buf_len, v->as.sval);
        break;
    case ELY_VALUE_INT_LIT:
    case ELY_VALUE_DEC_LIT:
    case ELY_VALUE_STRING_LIT:
        if (buf_len < v->as.lit.len)
            return -1;
        std::copy(v->as.lit.s, v->as.lit.s + v->as.lit.len, buf);
        return v->as.lit.len;
        break;
    default:
        assert(0 && "unreachable");
        break;
    }

    if (res.ec != std::errc())
    {
        return -1;
    }

    return res.ptr - buf;
}

int ely_value_print(const struct ely_value* v, FILE* f)
{
    switch (v->kind)
    {
    case ELY_VALUE_F32:
        return std::fprintf(f, "%f", v->as.f);
    case ELY_VALUE_F64:
        return std::fprintf(f, "%f", v->as.d);
    case ELY_VALUE_S64:
        return std::fprintf(f, PRIi64, v->as.sval);
    case ELY_VALUE_S32:
        return std::fprintf(f, PRIi32, static_cast<int32_t>(v->as.sval));
    case ELY_VALUE_U64:
        return std::fprintf(f, PRIu64, v->as.uval);
    case ELY_VALUE_U32:
        return std::fprintf(f, PRIu32, static_cast<uint32_t>(v->as.uval));
    case ELY_VALUE_INT_LIT:
    case ELY_VALUE_DEC_LIT:
    case ELY_VALUE_STRING_LIT:
        return std::fprintf(f, "%s", v->as.lit.s);
    default:
        assert(0 && "unreachable");
    }
}