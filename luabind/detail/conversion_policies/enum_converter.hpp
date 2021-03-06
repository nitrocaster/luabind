// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <type_traits>
#include <luabind/detail/type_traits.hpp>
#include <luabind/detail/conversion_policies/conversion_base.hpp>

namespace luabind::detail
{
    struct enum_converter
    {
        using type = enum_converter;
        using is_native = std::false_type;

        static constexpr int consumed_args = 1;

        template <class T>
        void to_lua(lua_State* L, T val)
        {
            using integral_t = std::underlying_type_t<T>;
            lua_pushnumber(L, static_cast<integral_t>(val));
        }

        template <class T>
        T to_cpp(lua_State* L, by_value<T>, int index)
        {
            using integral_t = std::underlying_type_t<T>;
            return static_cast<T>(static_cast<integral_t>(lua_tonumber(L, index)));
        }

        template <class T>
        int match(lua_State* L, by_value<T>, int index)
        {
            if (lua_isnumber(L, index))
                return 0;
            return no_match;
        }

        template <class T>
        T to_cpp(lua_State* L, by_const_reference<T>, int index)
        {
            return static_cast<T>(static_cast<int>(lua_tonumber(L, index)));
        }

        template <class T>
        int match(lua_State* L, by_const_reference<T>, int index)
        {
            if (lua_isnumber(L, index))
                return 0;
            return no_match;
        }

        template <class T>
        void converter_postcall(lua_State*, T, int) {}
    };
} // namespace luabind::detail
