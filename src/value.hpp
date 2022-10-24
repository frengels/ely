#pragma once

#include <cinttypes>
#include <cstdio>

#include <charconv>

#include "ely/runtime.h"
#include "ely/string.h"
#include "ely/type.hpp"

struct ely_value
{
    ely_value_kind kind;
    union
    {
        char       nothing;
        uint64_t   uval;
        int64_t    sval;
        float      f;
        double     d;
        ely_string lit;
    } as;

    ely_value(int64_t val) : kind(ELY_VALUE_S64)
    {
        as.sval = val;
    }

    ely_value(int32_t val) : kind(ELY_VALUE_S32)
    {
        as.sval = val;
    }

    ely_value(uint64_t val) : kind(ELY_VALUE_U64)
    {
        as.uval = val;
    }

    ely_value(uint32_t val) : kind(ELY_VALUE_U32)
    {
        as.uval = val;
    }

    ely_value(float f) : kind(ELY_VALUE_F32)
    {
        as.f = f;
    }

    ely_value(double val) : kind(ELY_VALUE_F64)
    {
        as.d = val;
    }

    ely_value(ely_value_kind kind, const char* str, size_t len)
        : ely_value(kind, ely_string_create_len(str, len))
    {}

    ely_value(ely_value_kind kind, ely_string str) : kind(kind)
    {
        as.lit = str;
    }

    ~ely_value()
    {
        switch (kind)
        {
        case ELY_VALUE_STRING_LIT:
        case ELY_VALUE_INT_LIT:
        case ELY_VALUE_DEC_LIT:
            ely_string_destroy(as.lit);
            break;
        default:
            break;
        }
    }

private:
    static bool type_eq(const ely_type& lhs, const ely_type& rhs)
    {
        return ely_type_eq(&lhs, &rhs);
    }

public:
    template<typename T>
    T get() const
    {
        assert(type_eq(type(), ely::get_type<T>()));

        if constexpr (std::is_same_v<T, std::int32_t>)
        {
            return static_cast<std::int32_t>(as.sval);
        }
        else if constexpr (std::is_same_v<T, std::int64_t>)
        {
            return static_cast<std::int64_t>(as.sval);
        }
        else if constexpr (std::is_same_v<T, std::uint32_t>)
        {
            return static_cast<std::uint32_t>(as.uval);
        }
        else if constexpr (std::is_same_v<T, std::uint64_t>)
        {
            return static_cast<std::uint64_t>(as.uval);
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            return static_cast<float>(as.f);
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            return static_cast<double>(as.d);
        }
        else
        {
            static_assert(!std::is_same_v<int, int>,
                          "Cannot get value for this type");
        }
    }

    ely_type type() const
    {
        switch (kind)
        {
        case ELY_VALUE_POISON:
            return ely::get_type<void>();
        case ELY_VALUE_U64:
            return ely::get_type<std::uint64_t>();
        case ELY_VALUE_U32:
            return ely::get_type<std::uint32_t>();
        case ELY_VALUE_S64:
            return ely::get_type<std::int64_t>();
        case ELY_VALUE_S32:
            return ely::get_type<std::int32_t>();
        case ELY_VALUE_F32:
            return ely::get_type<float>();
        case ELY_VALUE_F64:
            return ely::get_type<double>();
        case ELY_VALUE_STRING_LIT:
            return {ELY_TYPE_STRING_LIT, {}};
        case ELY_VALUE_INT_LIT:
            return {ELY_TYPE_INT_LIT, {}};
        case ELY_VALUE_DEC_LIT:
            return {ELY_TYPE_DEC_LIT, {}};
        default:
            assert(0 && "unreachable");
            return {ELY_TYPE_POISON, {}};
        }
    }

    void print(FILE* f) const
    {
        switch (kind)
        {
        case ELY_VALUE_F32:
            fprintf(f, "%f", as.f);
            break;
        case ELY_VALUE_F64:
            fprintf(f, "%f", as.d);
            break;
        case ELY_VALUE_S64:
            fprintf(f, PRIi64, as.sval);
            break;
        case ELY_VALUE_S32:
            fprintf(f, PRIi32, static_cast<int16_t>(as.sval));
            break;
        case ELY_VALUE_U64:
            fprintf(f, PRIu64, as.uval);
            break;
        case ELY_VALUE_U32:
            fprintf(f, PRIu32, static_cast<uint32_t>(as.uval));
            break;
        case ELY_VALUE_INT_LIT:
        case ELY_VALUE_DEC_LIT:
        case ELY_VALUE_STRING_LIT:
            fprintf(f, "%s", as.lit.str);
            break;
        default:
            assert(0 && "unhandled printing case");
        }
    }

    size_t to_chars(char* buf, size_t buf_len) const
    {
        std::to_chars_result res;

        switch (kind)
        {
        case ELY_VALUE_F32:
            res = std::to_chars(
                buf, buf + buf_len, as.f, std::chars_format::fixed);
            break;
        case ELY_VALUE_F64:
            res = std::to_chars(
                buf, buf + buf_len, as.d, std::chars_format::fixed);
            break;
        case ELY_VALUE_S64:
        case ELY_VALUE_S32:
            res = std::to_chars(buf, buf + buf_len, as.uval);
            break;
        case ELY_VALUE_U64:
        case ELY_VALUE_U32:
            res = std::to_chars(buf, buf + buf_len, as.uval);
            break;
        case ELY_VALUE_INT_LIT:
        case ELY_VALUE_DEC_LIT:
        case ELY_VALUE_STRING_LIT:
            if (buf_len < as.lit.len)
                return -1;
            std::copy(as.lit.str, as.lit.str + as.lit.len, buf);
            return as.lit.len;
            break;
        default:
            assert(0 && "to_chars case not handled for ely_value");
            break;
        }

        if (res.ec != std::errc())
        {
            return -1;
        }

        return res.ptr - buf;
    }
};