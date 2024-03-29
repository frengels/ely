#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <string_view>

#include <ely/context.h>
#include <ely/node.h>
#include <ely/runtime.h>

std::string_view value{"45.1343"};

TEST_CASE("eval")
{
    SUBCASE("decimal_literal")
    {
        constexpr auto buf_len = 32;
        char           buf[buf_len];

        auto* ctx = ely_context_create();
        auto* rt  = ely_runtime_create();
        auto* e   = ely_dec_literal_create(ctx, value.data(), value.size());
        auto  v   = ely_runtime_eval(rt, reinterpret_cast<ely_expr*>(e));
        auto  len = ely_value_to_chars(&v, buf, buf_len);
        CHECK_NE(len, size_t(-1));
        CHECK_EQ(std::string_view{buf, len}, value);

        ely_runtime_destroy(rt);
        ely_context_release(ctx);
    }

    SUBCASE("primitive")
    {
        auto* ctx = ely_context_create();
        auto* rt  = ely_runtime_create();

        SUBCASE("convert")
        {
            constexpr auto i = std::numeric_limits<std::int64_t>::max();

            auto* p = ely_prim_call_create(ctx, ELY_PRIM_S32);
            auto* e = ely_s64_literal_create(ctx, i);
            ely_prim_call_push_operand(p, reinterpret_cast<ely_expr*>(e));

            auto val = ely_runtime_eval(rt, reinterpret_cast<ely_expr*>(p));
            CHECK_EQ(val.kind, ELY_VALUE_S32);
        }

        SUBCASE("lit convert")
        {
            std::string_view ival{"654"};

            auto* p = ely_prim_call_create(ctx, ELY_PRIM_S32);
            auto* e = ely_int_literal_create(ctx, ival.data(), ival.size());
            ely_prim_call_push_operand(p, reinterpret_cast<ely_expr*>(e));

            auto val = ely_runtime_eval(rt, reinterpret_cast<ely_expr*>(p));
            CHECK_EQ(val.kind, ELY_VALUE_S32);
        }

        ely_runtime_destroy(rt);
        ely_context_release(ctx);
    }
}