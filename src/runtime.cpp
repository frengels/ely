#include "ely/runtime.h"

#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <cstdio>

#include <algorithm>
#include <charconv>

#include "ast.hpp"

namespace
{
struct primitive_info
{
    uint32_t required_args : 31;
    bool     variadic : 1;
};

primitive_info primitives[] = {
    [ELY_PRIM_F32] = {1, false},
    [ELY_PRIM_F64] = {1, false},
    [ELY_PRIM_U64] = {1, false},
    [ELY_PRIM_U32] = {1, false},
    [ELY_PRIM_U16] = {1, false},
    [ELY_PRIM_U8]  = {1, false},
    [ELY_PRIM_I64] = {1, false},
    [ELY_PRIM_I32] = {1, false},
    [ELY_PRIM_I16] = {1, false},
    [ELY_PRIM_I8]  = {1, false},
};

constexpr bool primitive_valid_arg_len(ely_prim_kind kind, size_t args)
{
    primitive_info info = primitives[kind];
    if (args < info.required_args)
    {
        return false;
    }
    else if (args > info.required_args)
    {
        return info.variadic;
    }

    return true;
}
} // namespace

struct ely_value
{
    ely_value_kind kind;
    union
    {
        char     nothing;
        uint64_t uval;
        int64_t  sval;
        float    f;
        double   d;
        struct
        {
            char*  str;
            size_t len;
        } lit;
    } as;

    ely_value(int64_t val) : kind(ELY_VALUE_SINT64)
    {
        as.sval = val;
    }

    ely_value(int32_t val) : kind(ELY_VALUE_SINT32)
    {
        as.sval = val;
    }

    ely_value(int16_t val) : kind(ELY_VALUE_SINT16)
    {
        as.sval = val;
    }

    ely_value(int8_t val) : kind(ELY_VALUE_SINT8)
    {
        as.sval = val;
    }

    ely_value(uint64_t val) : kind(ELY_VALUE_UINT64)
    {
        as.uval = val;
    }

    ely_value(uint32_t val) : kind(ELY_VALUE_UINT32)
    {
        as.uval = val;
    }

    ely_value(uint16_t val) : kind(ELY_VALUE_UINT16)
    {
        as.uval = val;
    }

    ely_value(uint8_t val) : kind(ELY_VALUE_UINT8)
    {
        as.uval = val;
    }

    ely_value(float f) : kind(ELY_VALUE_FLOAT)
    {
        as.f = f;
    }

    ely_value(double val) : kind(ELY_VALUE_DOUBLE)
    {
        as.d = val;
    }

    ely_value(ely_value_kind kind, const char* str, size_t len) : kind(kind)
    {
        as.lit.str = new char[len + 1];
        std::copy(str, str + len, as.lit.str);
        as.lit.str[len] = '\0';
        as.lit.len      = len;
    }

    ~ely_value()
    {
        switch (kind)
        {
        case ELY_VALUE_STRING_LIT:
        case ELY_VALUE_INT_LIT:
        case ELY_VALUE_DEC_LIT:
            delete[] as.lit.str;
            break;
        default:
            break;
        }
    }

    void print(FILE* f) const
    {
        switch (kind)
        {
        case ELY_VALUE_FLOAT:
            fprintf(f, "%f", as.f);
            break;
        case ELY_VALUE_DOUBLE:
            fprintf(f, "%f", as.d);
            break;
        case ELY_VALUE_SINT64:
            fprintf(f, PRIi64, as.sval);
            break;
        case ELY_VALUE_SINT32:
            fprintf(f, PRIi32, static_cast<int16_t>(as.sval));
            break;
        case ELY_VALUE_SINT16:
            fprintf(f, PRIi16, static_cast<int16_t>(as.sval));
            break;
        case ELY_VALUE_SINT8:
            fprintf(f, PRIi8, static_cast<int8_t>(as.sval));
            break;
        case ELY_VALUE_UINT64:
            fprintf(f, PRIu64, as.uval);
            break;
        case ELY_VALUE_UINT32:
            fprintf(f, PRIu32, static_cast<uint32_t>(as.uval));
            break;
        case ELY_VALUE_UINT16:
            fprintf(f, PRIu16, static_cast<uint16_t>(as.uval));
            break;
        case ELY_VALUE_UINT8:
            fprintf(f, PRIu8, static_cast<uint8_t>(as.uval));
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
        case ELY_VALUE_FLOAT:
            res = std::to_chars(
                buf, buf + buf_len, as.f, std::chars_format::fixed);
            break;
        case ELY_VALUE_DOUBLE:
            res = std::to_chars(
                buf, buf + buf_len, as.d, std::chars_format::fixed);
            break;
        case ELY_VALUE_SINT64:
        case ELY_VALUE_SINT32:
        case ELY_VALUE_SINT16:
        case ELY_VALUE_SINT8:
            res = std::to_chars(buf, buf + buf_len, as.uval);
            break;
        case ELY_VALUE_UINT64:
        case ELY_VALUE_UINT32:
        case ELY_VALUE_UINT16:
        case ELY_VALUE_UINT8:
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
        assert(primitive_valid_arg_len(kind, args_len));
        switch (kind)
        {
        case ELY_PRIM_F32:
            return do_from_chars<float>(args[0]);
        case ELY_PRIM_F64:
            return do_from_chars<double>(args[0]);
        case ELY_PRIM_U64:
            return do_from_chars<uint64_t>(args[0]);
        case ELY_PRIM_U32:
            return do_from_chars<uint32_t>(args[0]);
        case ELY_PRIM_U16:
            return do_from_chars<uint16_t>(args[0]);
        case ELY_PRIM_U8:
            return do_from_chars<uint8_t>(args[0]);
        case ELY_PRIM_I64:
            return do_from_chars<int64_t>(args[0]);
        case ELY_PRIM_I32:
            return do_from_chars<int32_t>(args[0]);
        case ELY_PRIM_I16:
            return do_from_chars<int16_t>(args[0]);
        case ELY_PRIM_I8:
            return do_from_chars<int8_t>(args[0]);
        default:
            assert(0 && "unexpected case");
            break;
        }

        return nullptr;
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