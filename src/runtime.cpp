#include "ely/runtime.h"

#include <cassert>
#include <cinttypes>
#include <cstdarg>
#include <cstdint>
#include <cstdio>

#include <algorithm>
#include <charconv>

#include "node.hpp"
#include "primitive.hpp"

#include "ely/string.h"
#include "ely/type.h"
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
    std::enable_if_t<std::is_same_v<uint32_t, T>, uint32_t> get() const
    {
        assert(type_eq(type(), ely::get_type<uint32_t>()));
        return static_cast<uint32_t>(as.uval);
    }

    template<typename T>
    std::enable_if_t<std::is_same_v<uint64_t, T>, uint64_t> get() const
    {
        assert(type_eq(type(), ely::get_type<uint64_t>()));
        return as.uval;
    }

    template<typename T>
    std::enable_if_t<std::is_same_v<float, T>, float> get() const
    {
        assert(type_eq(type(), ely::get_type<float>()));
        return as.f;
    }

    template<typename T>
    std::enable_if_t<std::is_same_v<double, T>, double> get() const
    {
        assert(type_eq(type(), ely::get_type<double>()));
        return as.d;
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

struct ely_runtime
{
    std::vector<ely_string> errs;

    ely_runtime() = default;

    int emit_err(const char* fmt, std::va_list args)
    {
        constexpr std::size_t buf_size         = 1024;
        char                  buffer[buf_size] = {}; // should be large enough
        int                   ret = std::vsnprintf(buffer, buf_size, fmt, args);

        std::fputs(buffer, stderr);
        errs.push_back(
            ely_string_create_len(buffer, static_cast<std::size_t>(ret)));
        return ret;
    }

    int emit_err(const char* fmt, ...)
    {
        std::va_list args;
        va_start(args, fmt);
        return emit_err(fmt, args);
    }

    ely_value* eval(ely_expr* e)
    {
        switch (e->base.kind)
        {
        case ELY_NODE_LIT_INT: {
            ely_int_literal* ilit = static_cast<ely_int_literal*>(e);
            return new ely_value(ELY_VALUE_INT_LIT, ilit->str, ilit->len);
        }
        break;
        case ELY_NODE_LIT_DEC: {
            ely_dec_literal* dlit = static_cast<ely_dec_literal*>(e);
            return new ely_value(ELY_VALUE_DEC_LIT, dlit->str, dlit->len);
        }
        break;
        case ELY_NODE_LIT_STRING: {
            ely_string_literal* strlit = static_cast<ely_string_literal*>(e);
            return new ely_value(
                ELY_VALUE_STRING_LIT, strlit->str, strlit->len);
        }
        break;
        case ELY_NODE_PRIM_CALL: {
            ely_prim_call*          pcall = static_cast<ely_prim_call*>(e);
            std::vector<ely_value*> vals;

            ely_expr* e;
            ELY_ILIST_FOR_EACH(e, &pcall->operands_head, link)
            {
                vals.push_back(eval(e));
            }

            return eval_prim(pcall->kind, vals.data(), vals.size());
        }
        default:
            assert(0 && "Unhandled case");
            break;
        }

        return nullptr;
    }

    template<typename T>
    static constexpr ely_value* do_from_chars(ely_value* arg)
    {
        T val;
        assert(arg->kind == ELY_VALUE_DEC_LIT ||
               arg->kind == ELY_VALUE_INT_LIT);
        auto res = std::from_chars(
            arg->as.lit.str, arg->as.lit.str + arg->as.lit.len, val);
        assert(res.ec == std::errc());
        return new ely_value(val);
    }

    ely_value* eval_prim(ely_prim_kind kind, ely_value** args, size_t args_len)
    {
        std::vector<ely_type> types;
        types.reserve(args_len);
        for (size_t i = 0; i != args_len; ++i)
        {
            types.push_back(args[i]->type());
        }
        const auto& ol = select_primitive_overload(
            kind, ely::get_type_generic(), types.data(), types.size());
        return ol(args);
    }
};

ely_runtime* ely_runtime_create()
{
    return new ely_runtime();
}

ely_value* ely_runtime_eval(ely_runtime* rt, ely_expr* e)
{
    return rt->eval(e);
}

void ely_runtime_destroy(ely_runtime* rt)
{
    delete rt;
}

int ely_runtime_emit_err(ely_runtime* rt, const char* fmt, ...)
{
    std::va_list args;
    va_start(args, fmt);
    return rt->emit_err(fmt, args);
}

void ely_value_print(const ely_value* v, FILE* f)
{
    v->print(f);
}

size_t ely_value_to_chars(const ely_value* v, char* str, size_t len)
{
    return v->to_chars(str, len);
}

ely_value_kind ely_value_get_kind(const ely_value* v)
{
    return v->kind;
}

ely_type ely_value_type(const ely_value* v)
{
    return v->type();
}