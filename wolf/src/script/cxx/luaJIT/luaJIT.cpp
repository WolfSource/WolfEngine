#include "luaJIT.hpp"
#include "wolf/src/script/luajit.rs.h"

luaJIT::luaJIT()
{
    //initialize lua
    this->_lua = luaL_newstate();
    luaL_openlibs(this->_lua);
}

luaJIT::~luaJIT()
{
}

int luaJIT::load(rust::string &p_module_name, rust::string &p_source_code) const
{
    return luaL_loadbuffer(this->_lua, p_source_code.c_str(), p_source_code.size(), p_module_name.c_str());
}

int luaJIT::execute() const
{
    return lua_pcall(this->_lua, 0, LUA_MULTRET, 0);
}

void luaJIT::bind(rust::string p_lua_function_name, lua_CFunction p_lua_function_callback) const
{
    lua_pushcfunction(this->_lua, p_lua_function_callback);
    lua_setglobal(this->_lua, p_lua_function_name.c_str());
}