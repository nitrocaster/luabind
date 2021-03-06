// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#pragma once

#include <luabind/error.hpp>
#include <luabind/lua_include.hpp>
#include <string>
#include <iostream>
#include <doctest/doctest.h>

#define DOSTRING_EXPECTED(state, str, expected) \
    CHECK_THROWS_WITH((dostring(state, str)), expected)
#define DOSTRING(state, str) dostring(state, str)

struct lua_state
{
    ~lua_state();
    void initialize();
    operator lua_State*() const { return m_state; }
    void check() const;

private:
    lua_State* m_state = nullptr;
    int m_top = 0;
};

inline lua_State *L = nullptr;

std::string demangle(std::type_info const& id);

#define CAT2(a,b) a##b
#define CAT(a,b) CAT2(a,b)

template <class T>
struct counted_type
{
    inline static int count = 0;

    counted_type() { ++count; }
    counted_type(counted_type const&) { ++count; }
    ~counted_type()
    { CHECK_MESSAGE(--count >= 0, demangle(typeid(T)) << ": double destruction"); }
};

template <typename T>
struct counter_guard
{
    ~counter_guard()
    {
        if (!counted_type<T>::count)
            return;
        std::cout << demangle(typeid(T)) + ": live object\n";
        exit(1);
    }
};

#define COUNTER_GUARD(type) counter_guard<type> CAT(type, _guard)

void dostring(lua_State* L, char const* str);
