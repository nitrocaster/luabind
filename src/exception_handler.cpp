// Boost Software License http://www.boost.org/LICENSE_1_0.txt
// Copyright (c) 2005 The Luabind Authors

#include <luabind/lua_include.hpp>
#include <luabind/config.hpp>

#ifndef LUABIND_NO_EXCEPTIONS

#include <luabind/error.hpp> // for error
#include <luabind/exception_handler.hpp> // for exception_handler_base
#include <exception> // for exception
#include <stdexcept> // for logic_error, runtime_error

namespace luabind::detail
{
    static exception_handler_base* handler_chain = nullptr;

    static void push_exception_string(lua_State* L, char const* exception, char const* what)
    {
        lua_pushstring(L, exception);
        lua_pushliteral(L, ": '");
        lua_pushstring(L, what);
        lua_pushliteral(L, "'");
        lua_concat(L, 4);
    }

    void exception_handler_base::try_next(lua_State* L) const
    {
        if (next)
            next->handle(L);
        else
            throw;
    }

    void handle_exception_aux(lua_State* L)
    {
        try
        {
            if (handler_chain)
                handler_chain->handle(L);
            else
                throw;
        }
        catch (error const&)
        {
            // is always thrown in the context where an error message was already pushed to the lua stack.
        }
        catch (std::logic_error const& e)
        {
            push_exception_string(L, "std::logic_error", e.what());
        }
        catch (std::runtime_error const& e)
        {
            push_exception_string(L, "std::runtime_error", e.what());
        }
        catch (std::exception const& e)
        {
            push_exception_string(L, "std::exception", e.what());
        }
        catch (char const* str)
        {
            push_exception_string(L, "c-string", str);
        }
        catch (...)
        {
            lua_pushliteral(L, "Unknown C++ exception");
        }
    }

    void register_exception_handler(exception_handler_base* handler)
    {
        if (!handler_chain)
        {
            handler_chain = handler;
            return;
        }
        exception_handler_base* p = handler_chain;
        for (; p->next; p = p->next);
        handler->next = nullptr;
        p->next = handler;
    }
} // namespace luabind::detail

#endif // LUABIND_NO_EXCEPTIONS
