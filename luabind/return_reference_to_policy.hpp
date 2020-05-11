// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2003 The Luabind Authors

#pragma once

#include <luabind/detail/policy.hpp>    // for index_map, policy_cons, etc
#include <luabind/lua_include.hpp>      // for lua_State, lua_pushnil, etc

namespace luabind {
	namespace detail {

		struct cpp_to_lua;

		template<class T>
		struct return_reference_to_converter;

		template<>
		struct return_reference_to_converter<cpp_to_lua>
		{
			template<class T>
			void to_lua(lua_State* L, const T&)
			{
				lua_pushnil(L);
			}
		};

		template< unsigned int N >
		struct return_reference_to_policy : detail::converter_policy_has_postcall_tag
		{
			template<typename StackIndexList>
			static void postcall(lua_State* L, int results, StackIndexList)
			{
				lua_pushvalue(L, (meta::get<StackIndexList, N>::value));
				lua_replace(L, (meta::get<StackIndexList, 0>::value + results));
			}

			template<class T, class Direction>
			struct specialize
			{
				using type = return_reference_to_converter<Direction>;
			};
		};

	}

	namespace policy
	{
		template<unsigned int N>
		using return_reference_to = converter_policy_injector<0, detail::return_reference_to_policy<N>>;
	}
}
