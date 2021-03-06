// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/config.hpp>
#include <luabind/detail/primitives.hpp>
#include <luabind/detail/decorate_type.hpp>

#include <typeinfo>
#include <type_traits>
#include <string>
#include <memory>

#include <luabind/detail/meta.hpp>

namespace luabind
{
    template <typename... T>
    using policy_list = meta::type_list<T...>;
    using no_policies = policy_list<>;

    namespace detail
    {
        struct has_postcall_tag {};
    } // namespace detail

    // A converter injector instructs the call mechanism to use a
    // certain converter policy for an element of a function call signature
    // that is denoted by the parameter Index. 0 stands for the return value,
    // while 1 might denote an implicit "this" argument or the first
    // actual argument of the function call
    template <uint32_t Index, typename T>
    struct converter_injector : policy_list<converter_injector<Index, T>>
    {
        static constexpr bool has_postcall = std::is_convertible_v<T, detail::has_postcall_tag>;
    };

    // A postcall injector instructs the call mechanism to call certain
    // static function "postcall" on type T after having executed a call
    template <typename T>
    struct postcall_injector : policy_list<postcall_injector<T>>
    {};

    template <typename T, typename Enable = void>
    struct default_converter;
} // namespace luabind

namespace luabind::detail
{
    struct lua_to_cpp {};
    struct cpp_to_lua {};

    struct default_policy
    {
        template <class T, class Direction>
        struct specialize
        {
            using type = default_converter<T>;
        };
    };

    template <class T>
    struct is_primitive : default_converter<T>::is_native
    {};

    template <class T>
    constexpr bool is_primitive_v = is_primitive<T>::value;

    namespace policy_detail
    {
        template <uint32_t Index, typename PoliciesList>
        struct get_converter_policy;

        template <uint32_t Index, typename Injector0, typename... Injectors>
        struct get_converter_policy<Index, meta::type_list<Injector0, Injectors...>>
        {
            using type = typename get_converter_policy<Index, meta::type_list<Injectors...>>::type;
        };

        template <uint32_t Index, typename ConverterPolicy, typename... Injectors>
        struct get_converter_policy<Index, meta::type_list<converter_injector<Index, ConverterPolicy>, Injectors...>>
        {
            using type = ConverterPolicy;
        };

        template <uint32_t Index, typename ConverterPolicy>
        struct get_converter_policy<Index, converter_injector<Index, ConverterPolicy>>
        {
            using type = ConverterPolicy;
        };

        template <uint32_t Index, typename ConverterPolicy, uint32_t Index2>
        struct get_converter_policy<Index, converter_injector<Index2, ConverterPolicy>>
        {
            using type = default_policy;
        };

        template <uint32_t Index>
        struct get_converter_policy<Index, meta::type_list<>>
        {
            using type = default_policy;
        };
    }

    // Fetches converter policy for Signature element [Index] from policy list [PolicyList]
    template <uint32_t Index, typename PolicyList>
    using fetched_converter_policy = typename policy_detail::get_converter_policy<Index, PolicyList>::type;

    // Specializes converter policy [ConverterPolicy] for type [Type] in direction [Direction]
    template <typename ConverterPolicy, typename Type, typename Direction>
    using specialized_converter_policy = typename ConverterPolicy::template specialize<Type, Direction>::type;

    // Fetches the converter policy for Signature element [Index] from the policy list [PolicyList] and specializes it
    // for the concrete type [T] with [Direction] being either "lua_to_cpp" or "cpp_to_lua".
    template <uint32_t Index, typename PolicyList, typename T, typename Direction>
    using specialized_converter_policy_n = specialized_converter_policy<fetched_converter_policy<Index, PolicyList>, T, Direction>;

    template <typename List, class Sought>
    constexpr bool has_call_policy_v = meta::contains_v<List, postcall_injector<Sought>>;
} // namespace luabind::detail

namespace luabind
{
    constexpr meta::index<0> return_value;
    constexpr meta::index<0> result;
    constexpr meta::index<1> _1;
    constexpr meta::index<2> _2;
    constexpr meta::index<3> _3;
    constexpr meta::index<4> _4;
    constexpr meta::index<5> _5;
    constexpr meta::index<6> _6;
    constexpr meta::index<7> _7;
    constexpr meta::index<8> _8;
    constexpr meta::index<9> _9;
} // namespace luabind
