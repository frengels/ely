#include "ely/runtime.h"

#include <cassert>
#include <cstdarg>
#include <cstdint>

#include <algorithm>
#include <charconv>

#include "node.hpp"
#include "primitive.hpp"

#include "ely/string.h"
#include "ely/type.h"
#include "ely/value.h"
#include "elypp/type.hpp"
#include "elypp/value.hpp"

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

    ely_value eval(ely_expr* e)
    {
        switch (e->base.kind)
        {
        case ELY_NODE_LIT_INT: {
            auto* ilit = static_cast<ely_int_literal*>(e);
            return ely_value_create_int_literal(ilit->str.s, ilit->str.len);
        }
        break;
        case ELY_NODE_LIT_DEC: {
            auto* dlit = static_cast<ely_dec_literal*>(e);
            return ely_value_create_dec_literal(dlit->str.s, dlit->str.len);
        }
        break;
        case ELY_NODE_LIT_STRING: {
            ely_string_literal* strlit = static_cast<ely_string_literal*>(e);
            return ely_value_create_string_literal(strlit->text.s,
                                                   strlit->text.len);
        }
        break;
        case ELY_NODE_LIT_S32: {
            ely_s32_literal* lit = static_cast<ely_s32_literal*>(e);
            return ely_value_create_s32(lit->val);
        }
        break;
        case ELY_NODE_LIT_S64: {
            ely_s64_literal* lit = static_cast<ely_s64_literal*>(e);
            return ely_value_create_s64(lit->val);
        }
        break;
        case ELY_NODE_PRIM_CALL: {
            ely_prim_call*         pcall = static_cast<ely_prim_call*>(e);
            std::vector<ely_value> vals;

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

        return ely_value_create_poison();
    }

    ely_value eval_prim(ely_prim_kind kind, ely_value* args, size_t args_len)
    {
        std::vector<ely_type> types;
        types.reserve(args_len);
        for (size_t i = 0; i != args_len; ++i)
        {
            types.push_back(ely_value_type(&args[i]));
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

ely_value ely_runtime_eval(ely_runtime* rt, ely_expr* e)
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
