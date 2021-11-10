#pragma once

#include "rust/cxx.h"
#include <memory>
#include <luajit-2.1/lua.h>
#include "luajit-2.1/lualib.h"
#include "luajit-2.1/lauxlib.h"

class luaJIT
{
public:
    /**
	 * constructor of luaJIT
	*/
    luaJIT();

    /**
	 * destructor of luaJIT
	*/
    ~luaJIT();

    /**
	 * create a buffer from lua's source code
	 * @param p_module_name the name of lua's module
     * @param p_source_code the source code of lua
	 * @return 0 means no error
	*/
    int load(rust::string &p_module_name, rust::string &p_source_code) const;

    /**
	 * execute lua buffer 
	 * @return 0 means no error
	*/
    int execute() const;

    /**
	 * create a lua function and bind it to C callback function
	 * @param p_lua_function_name the name of function which will be created
     * @param p_lua_function_callback the callback
	 * @return 0 means no error
	*/
    void bind(rust::string p_lua_function_name, lua_CFunction p_lua_function_callback) const;

private:
    //private constructors for avoid copy constructor
    luaJIT(const luaJIT &) = delete;
    luaJIT &operator=(const luaJIT &) = delete;

    lua_State *_lua;
};
