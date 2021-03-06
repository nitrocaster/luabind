// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2008 The Luabind Authors

#include "test.hpp"
#include <luabind/luabind.hpp>
#include <luabind/adopt_policy.hpp>
#include <iostream>

struct Base
{
    Base()
    {
        count++;
    }

    virtual ~Base()
    {
        count--;
    }

    static int count;
};

int Base::count = 0;

struct Base_wrap : Base, luabind::wrap_base
{};

void destroy(Base* p)
{
    luabind::luabind_delete(p);
}

Base* adopted = 0;

void take_ownership(Base* p)
{
    adopted = p;
}

void not_null(Base* p)
{
    CHECK(p != nullptr);
}

TEST_CASE("adopt")
{
    using namespace luabind;

    disable_super_deprecation();

    module(L) [
        class_<Base, no_bases, default_holder, Base_wrap>("Base")
            .def(constructor<>()),

        def("take_ownership", &take_ownership, policy::adopt<1>()),
        def("not_null", &not_null)
    ];

    DOSTRING(L,
        "x = Base()\n"
    );

    CHECK(Base::count == 1);

    DOSTRING(L,
        "x = nil\n"
        "collectgarbage('collect')\n"
    );

    CHECK(Base::count == 0);

    DOSTRING(L,
        "class 'Derived' (Base)\n"
        "  function Derived:__init()\n"
        "      super()\n"
        "      self.x = Base()\n"
        "  end\n"
    );

    DOSTRING(L,
        "x = Derived()\n"
    );

    CHECK(Base::count == 2);

    DOSTRING(L,
        "x = nil\n"
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    CHECK(Base::count == 0);

    DOSTRING(L,
        "x = nil\n"
        "collectgarbage('collect')\n"
    );

    CHECK(Base::count == 0);

    DOSTRING(L,
        "class 'Derived2' (Derived)\n"
        "  function Derived2:__init()\n"
        "      super()\n"
        "  end\n"
    );

    DOSTRING(L,
        "x = Derived2()\n"
    );

    CHECK(Base::count == 2);

    DOSTRING(L,
        "x = nil\n"
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    CHECK(Base::count == 0);

    DOSTRING(L,
        "x = Derived()\n"
    );

    CHECK(Base::count == 2);

    DOSTRING(L,
        "take_ownership(x)\n"
        "x = nil\n"
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    CHECK(Base::count == 2);

    luabind_delete(adopted);

    DOSTRING(L,
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    CHECK(Base::count == 0);

    DOSTRING(L,
        "x = Derived2()\n"
    );

    CHECK(Base::count == 2);

    DOSTRING(L,
        "take_ownership(x)\n"
        "x = nil\n"
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    CHECK(Base::count == 2);

    luabind_delete(adopted);

    DOSTRING(L,
        "collectgarbage('collect')\n"
        "collectgarbage('collect')\n"
    );

    CHECK(Base::count == 0);

    DOSTRING(L,
        "x = Derived()\n"
        "take_ownership(x)\n"
        "not_null(x)\n"
    );

    luabind_delete(adopted);

    DOSTRING(L,
        "x = nil\n"
        "collectgarbage('collect')\n"
    );

    CHECK(Base::count == 0);
}
