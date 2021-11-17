#pragma once

#include "rust/cxx.h"
#include <memory>

extern "C"
{
#include <luajit-2.1/lua.h>
#include <luajit-2.1/lualib.h>
#include <luajit-2.1/lauxlib.h>
}

struct LuaValue;
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
	int load(rust::Str p_module_name, rust::Str p_source_code) const;

	/**
	 * exec lua buffer 
	 * @return 0 means no error
	*/
	int exec() const;

	/**
	 * create a lua function and bind it to C callback function
	 * @param p_lua_fn_name the name of function which will be created
     * @param p_lua_function_callback the callback
	 * @return void
	*/
	int bind(rust::Str p_lua_fn_name, lua_CFunction p_lua_function_callback) const;

	/**
	 * call a lua function 
	 * @param p_name the name of function which will be called
     * @param p_params parameters 
	 * @param p_results the return results
	 * @return 0 means no error
	*/
	int call(rust::Str p_name, rust::Slice<const LuaValue> p_params, rust::Vec<LuaValue> &p_results) const;

	/**
	 * get data from a global variable
	 * @param p_name the name of global variable
     * @param p_value the value which will be filled by global variable
	 * @return 0 means no error
	*/
	int get_global(rust::Str p_name, LuaValue &p_value) const;

	/**
	 * set data to a global variable
	 * @param p_name the name of global variable
     * @param p_value the value which will be assigned to global variable 
	 * @return 0 means no error
	*/
	int set_global(rust::Str p_name, const LuaValue &p_value) const;

	/**
	 * set lua enviroment path
	 * @param p_env_path the enviroment path for lua
	 * @return 0 means no error
	*/
	int set_lua_path(rust::Str p_env_path) const;

private:
	//private constructors for avoid copy constructor
	luaJIT(const luaJIT &) = delete;
	luaJIT &operator=(const luaJIT &) = delete;

	int _set_value(const LuaValue &p_value) const;
	int _get_value(
		const int p_index,
		LuaValue &p_value) const;

	lua_State *_lua;
};

std::unique_ptr<luaJIT> New();
