// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2009 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/class_info.hpp>

struct private_struct
{};

struct X
{
    void f()
    {}

    int x;
    int y;

    void take_private(private_struct)
    {}
};

struct unnamed
{};

unnamed make_unnamed()
{
    return unnamed();
}

TEST_CASE("class_info")
{
    using namespace luabind;

    bind_class_info(L);

    module(L) [
        class_<X>("X")
            .def(constructor<>())
            .def("f", &X::f)
            .def_readonly("x", &X::x)
            .def_readonly("y", &X::y)
            .def("take_private", &X::take_private),

        class_<unnamed>()
            .def(constructor<>()),
        
        def("make_unnamed", &make_unnamed)
    ];

    DOSTRING(L,
        "x = X()\n"
        "info = class_info(x)\n"
        "assert(info.name == 'X')\n"
        "assert(info.methods['f'] == x.f)\n"
        "assert(info.methods['__init'] == x.__init)\n"
        "attrs = info.attributes\n"
        "assert(attrs[1] == 'x' or attrs[2] == 'x')\n"
        "assert(attrs[1] == 'y' or attrs[2] == 'y')\n");

    DOSTRING(L,
        "info = class_info(2)\n"
        "assert(info.name == 'number')\n"
        "assert(#info.methods == 0)\n"
        "assert(#info.attributes == 0)\n");

    DOSTRING(L,
        "names = class_names()\n"
        "assert(type(names) == 'table')\n"
        "assert(#names == 2)\n"
        "assert(names[1] == 'X' or names[2] == 'X')\n"
        "assert(names[1] == 'class_info_data' or names[2] == 'class_info_data')\n");

    DOSTRING(L,
        "info = class_info(X)\n"
        "assert(info.name == 'X')\n"
        "assert(info.methods['f'] == X().f)\n"
        "assert(info.methods['__init'] == X().__init)\n"
        "assert(info.attributes[1] == 'y')\n"
        "assert(info.attributes[2] == 'x')\n"
    );
#ifdef _MSC_VER
    DOSTRING_EXPECTED(L,
        "x:take_private(nil)\n",
        "No matching overload found, candidates:\n"
        "void take_private(X&,custom [struct private_struct])\n"
        "Passed arguments [2]: userdata (X&), nil (nil)\n");

    DOSTRING(L,
        "u = make_unnamed()\n"
        "info = class_info(u)\n"
        "assert(info.name == 'unnamed [struct unnamed]')");
#elif defined(__GNUC__) && (defined(__GLIBCXX__) || defined(__GLIBCPP__))
    DOSTRING_EXPECTED(L,
        "x:take_private(nil)\n",
        "No matching overload found, candidates:\n"
        "void take_private(X&,custom [private_struct])\n"
        "Passed arguments [2]: userdata (X&), nil (nil)\n");
    DOSTRING(L,
        "u = make_unnamed()\n"
        "info = class_info(u)\n"
        "assert(info.name == 'unnamed [unnamed]')");
#else
    // skipping implementation dependent type name tests
    // XXX: emit warning?
#endif
    DOSTRING(L,
        "s = tostring(X)\n"
        "assert(s:match('^class X (.+)$'))"
    );
    DOSTRING(L,"print(X)");
}

