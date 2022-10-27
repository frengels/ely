#include "primitive.hpp"

#include <array>
#include <tuple>
#include <vector>

#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/return_type.hpp>

#include "ely/node.h"
#include "ely/runtime.h"
#include "ely/value.h"
#include "elypp/type.hpp"
#include "elypp/value.hpp"

namespace
{
template<typename Tuple>
struct apply_to_tuple;

template<typename... Ts>
struct apply_to_tuple<std::tuple<Ts...>>
{
    template<template<typename...> typename F>
    using apply = F<Ts...>;
};

template<typename... Ts>
struct tuple_to_types_impl
{
    static constexpr auto value =
        std::array<ely_type, sizeof...(Ts)>{ely::get_type<Ts>()...};
};

template<typename Tuple>
struct tuple_to_types
{
    static constexpr auto value =
        apply_to_tuple<Tuple>::template apply<tuple_to_types_impl>::value;
};

template<auto Fn, typename Seq>
struct adapt_impl;

template<auto Fn, std::size_t... Indices>
struct adapt_impl<Fn, std::index_sequence<Indices...>>
{
    using fn_t   = decltype(Fn);
    using args_t = boost::callable_traits::args_t<fn_t>;
    using ret_t  = boost::callable_traits::return_type_t<fn_t>;

    ely_value operator()(ely_value* args)
    {
        auto val = Fn(ely::get_value<std::tuple_element_t<Indices, args_t>>(
            ely::value(args[Indices]))...);
        return ely::create_value(val);
    }
};

template<auto Fn>
struct adapt_fn
{
    using args_t = boost::callable_traits::args_t<decltype(Fn)>;

    static constexpr auto fn = [](ely_value* args) -> ely_value {
        return adapt_impl<
            Fn,
            std::make_index_sequence<std::tuple_size_v<args_t>>>{}(args);
    };
};

template<auto Fn>
struct pinfo_maker
{
    using Fn_t   = decltype(Fn);
    using args_t = boost::callable_traits::args_t<Fn_t>;
    static constexpr auto           args_tys = tuple_to_types<args_t>::value;
    static constexpr primitive_info pinfo{
        adapt_fn<Fn>::fn,
        ely::get_type<boost::callable_traits::return_type_t<Fn_t>>(),
        args_tys.data(),
        args_tys.size()};
};

template<auto... Fns>
struct prim_overload_set
{
    static constexpr auto pinfos =
        std::array<primitive_info, sizeof...(Fns)>{pinfo_maker<Fns>::pinfo...};
};

template<typename Tag, typename Res, typename... Args>
primitive_info make_pinfo(Res (*fn)(Args...))
{
    void*         void_fn = static_cast<void*>(fn);
    ely_type_kind res_ty  = ely::get_type<Res>();

    static constexpr std::array<ely_type, sizeof...(Args)> args_tys = {
        ely::get_type<Args>()...};

    return {void_fn, res_ty, args_tys.data(), args_tys.size()};
}

template<auto... Fns>
constexpr primitive_overload_set make_set()
{
    using maker_t = prim_overload_set<Fns...>;
    return {maker_t::pinfos.data(), maker_t::pinfos.size()};
}

template<typename To, typename From>
To cvt(From val)
{
    return static_cast<To>(val);
}

constexpr primitive_overload_set primitive_sets[] = {
    [ELY_PRIM_F32] = make_set<cvt<float, double>>(),
    [ELY_PRIM_F64] = make_set<cvt<double, float>>(),
    [ELY_PRIM_U64] = {},
    [ELY_PRIM_U32] = {},
    [ELY_PRIM_S64] = {},
    [ELY_PRIM_S32] = make_set<cvt<std::int32_t, std::int64_t>,
                              cvt<std::int32_t, std::uint32_t>>(),
};
} // namespace

const std::size_t             primitives_len = std::size(primitive_sets);
const primitive_overload_set* primitives     = std::data(primitive_sets);

ely_fn_ptr_t select_primitive_overload(ely_prim_kind   kind,
                                       ely_type        ret_ty,
                                       const ely_type* args_ty,
                                       std::size_t     args_len)
{
    assert(static_cast<std::size_t>(kind) < primitives_len &&
           "Unknown primitive (out of range)");

    const primitive_overload_set& candidates = primitives[kind];

    std::vector<const primitive_info*> matches;
    for (const primitive_info& info : candidates)
    {
        if (info.matches(ret_ty, args_ty, args_len))
        {
            matches.push_back(std::addressof(info));
        }
    }

    assert(matches.size() != 0 && "No match found");
    assert(matches.size() == 1 && "ambiguous matches found");
    return matches[0]->fn;
}