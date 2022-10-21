#include "ely/runtime.h"

#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <cstdio>

#include <algorithm>
#include <charconv>

#include "node.hpp"

#include "ely/string.h"
#include "ely/type.h"

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

    ely_type_kind type() const
    {
        switch (kind)
        {
        case ELY_VALUE_POISON:
            return ELY_TYPE_POISON;
        case ELY_VALUE_U64:
            return ELY_TYPE_U64;
        case ELY_VALUE_U32:
            return ELY_TYPE_U32;
        case ELY_VALUE_S64:
            return ELY_TYPE_S64;
        case ELY_VALUE_S32:
            return ELY_TYPE_S32;
        case ELY_VALUE_F32:
            return ELY_TYPE_F32;
        case ELY_VALUE_F64:
            return ELY_TYPE_F64;
        case ELY_VALUE_STRING_LIT:
            return ELY_TYPE_STRING_LIT;
        case ELY_VALUE_INT_LIT:
            return ELY_TYPE_INT_LIT;
        case ELY_VALUE_DEC_LIT:
            return ELY_TYPE_DEC_LIT;
        default:
            assert(0 && "unreachable");
            return ELY_TYPE_POISON;
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

bool primitive_valid_arg_len(ely_prim_kind kind, size_t args)
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

bool values_same_type(const ely_value* const* vals, size_t len)
{
    if (len == 0)
    {
        return true;
    }

    ely_value_kind expected = vals[0]->kind;

    for (size_t i = 1; i != len; ++i)
    {
        const ely_value* val = vals[i];
        if (val->kind != expected)
        {
            return false;
        }
    }

    return true;
}

bool binary_math_type(ely_value_kind kind)
{
    switch (kind)
    {
    case ELY_VALUE_F32:
    case ELY_VALUE_F64:
    case ELY_VALUE_S64:
    case ELY_VALUE_S32:
    case ELY_VALUE_U64:
    case ELY_VALUE_U32:
        return true;
    default:
        return false;
    }
}
} // namespace

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
        case ELY_PRIM_ADD:
            return eval_add(args, args_len);
        default:
            assert(0 && "unexpected case");
            break;
        }

        return nullptr;
    }

    ely_value* eval_add(ely_value** args, size_t args_len)
    {
        assert(values_same_type(args, args_len));
        assert(args_len >= 1);
        assert(binary_math_type(args[0]->kind));

        switch (args[0]->kind)
        {}

        return nullptr;
    }

    ely_value* eval_sub(ely_value** args, size_t args_len)
    {
        return nullptr;
    }

    ely_value* eval_mul(ely_value** args, size_t args_len)
    {
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

ely_type_kind ely_value_type(const ely_value* v)
{
    return v->type();
}