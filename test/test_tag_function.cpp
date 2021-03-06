// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/tag_function.hpp>
#include <functional>

int f(int x, int y)
{
    return x + y;
}

struct X
{
    int f(int x, int y)
    {
        return x + y;
    }
};

TEST_CASE("tag_function")
{
    using namespace luabind;
    
    module(L) [
        def("f", tag_function<int(int)>(std::bind(&f, 5, std::placeholders::_1))),

        class_<X>("X")
            .def(constructor<>())
            .def("f", tag_function<int(X&, int)>(std::bind(&X::f, std::placeholders::_1, 10, std::placeholders::_2)))
    ];

    DOSTRING(L,
        "assert(f(0) == 5)\n"
        "assert(f(1) == 6)\n"
        "assert(f(5) == 10)\n"
    );

    DOSTRING(L,
        "x = X()\n"
        "assert(x:f(0) == 10)\n"
        "assert(x:f(1) == 11)\n"
        "assert(x:f(5) == 15)\n"
    );

}

