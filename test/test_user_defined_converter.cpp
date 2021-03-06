// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>

struct X
{
    X(int value)
        : value(value)
    {}

    int value;
};

namespace luabind {


    /*
        This is the only piece of code that hat non-static compute_score. Fixed it to be static.
    */

    template <>
    struct default_converter<X>
        : native_converter_base<X>
    {
        static int compute_score(lua_State* L, int index)
        {
            return cv.compute_score(L, index);
        }

        X to_cpp_deferred(lua_State* L, int index)
        {
            return X((int)lua_tonumber(L, index));
        }

        void to_lua_deferred(lua_State* L, X const& x)
        {
            lua_pushinteger(L, x.value);
        }

        static default_converter<int> cv;
    };

} // namespace luabind

int take(X x)
{
    return x.value;
}

X get(int value)
{
    return X(value);
}

TEST_CASE("user_defined_converter")
{
    using namespace luabind;

    module(L)[
        def("take", &take),
            def("get", &get)
    ];

    DOSTRING(L,
        "assert(take(1) == 1)\n"
        "assert(take(2) == 2)\n"
    );

    DOSTRING(L,
        "assert(get(1) == 1)\n"
        "assert(get(2) == 2)\n"
    );
}

