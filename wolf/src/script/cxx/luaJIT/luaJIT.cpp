#include "luaJIT.hpp"
#include "wolf/src/script/lua.rs.h"
#include <string.h>

#ifdef DEBUG
#include <iostream>
#endif

luaJIT::luaJIT()
{
    //initialize lua
    this->_lua = luaL_newstate();
    luaL_openlibs(this->_lua);
}

luaJIT::~luaJIT()
{
    if (this->_lua)
    {
        lua_pop(this->_lua, 1);
        lua_close(this->_lua);
        this->_lua = nullptr;
    }
}

int luaJIT::load(rust::Str p_module_name, rust::Str p_source_code) const
{
    auto _src_code_size = p_source_code.length();
    auto _module_name_size = p_module_name.length();

    if (!_src_code_size || !_module_name_size)
    {
        return -1;
    }
    auto _module_name = p_module_name.data();
    auto _src_code = p_source_code.data();

    return luaL_loadbuffer(this->_lua, _src_code, _src_code_size, _module_name);
}

int luaJIT::exec() const
{
    return lua_pcall(this->_lua, 0, LUA_MULTRET, 0);
}

int luaJIT::bind(rust::Str p_lua_fn_name, lua_CFunction p_lua_function_callback) const
{
    if (!p_lua_fn_name.length())
    {
        return -1;
    }
    auto _name = p_lua_fn_name.data();

    lua_pushcfunction(this->_lua, p_lua_function_callback);
    lua_setglobal(this->_lua, _name);
    return 0;
}

int luaJIT::call(rust::Str p_lua_fn_name, rust::Slice<const lua_in_value> p_params, rust::Vec<lua_in_value> &p_results) const
{
    if (!p_lua_fn_name.length())
    {
        return -1;
    }
    auto _name = p_lua_fn_name.data();

    lua_getglobal(this->_lua, _name);
    if (lua_isnil(this->_lua, -1))
    {
        return -1;
    }

    for (const auto &iter : p_params)
    {
        _set_value(iter);
    }

    return 0;
}

int luaJIT::get_global_var(const rust::string &p_name, lua_inout_value &p_value) const
{
    if (!p_name.length())
    {
        return -1;
    }
    auto _name = p_name.data();

    lua_getglobal(this->_lua, _name);
    if (lua_isnil(this->_lua, -1))
    {
        return -1;
    }
    else
    {
        return _get_value(-1, p_value);
    }
}

int luaJIT::get_global_str(const rust::string &p_name, rust::string &p_value) const
{
    if (!p_name.length())
    {
        return -1;
    }
    auto _name = p_name.data();

    lua_getglobal(this->_lua, _name);
    if (lua_isnil(this->_lua, -1))
    {
        return -1;
    }
    else
    {
        auto _index = -1;
        auto _type = lua_type(this->_lua, _index);
        if (_type == LUA_TSTRING)
        {
            p_value = lua_tostring(this->_lua, _index);
            return 0;
        }
        else
        {
            return -1;
        }
    }
}

int luaJIT::set_global_var(rust::Str p_name, const lua_in_value &p_value) const
{
    if (!p_name.length())
    {
        return -1;
    }

    auto _name = p_name.data();
    lua_getglobal(this->_lua, _name);
    if (lua_isnil(this->_lua, -1))
    {
        return -1;
    }
    else
    {
        //set the value for global variable of lua
        if (_set_value(p_value) == 0)
        {
            lua_setglobal(this->_lua, _name);
            return 0;
        }
        return -1;
    }
}

int luaJIT::set_lua_path(rust::Str p_env_path) const
{
    if (p_env_path.empty())
    {
        return -1;
    }

    constexpr auto MAX_PATH_SIZE = 256;
    char _dst[MAX_PATH_SIZE]; //MAX_PATH_SIZE

    lua_getglobal(this->_lua, "package");
    lua_getfield(this->_lua, -1, "path"); // get field "path" from table at top of stack (-1)

    auto _current_lua_path = lua_tostring(this->_lua, -1); // grab path string from top of stack

    strcpy(_dst, _current_lua_path);
    strcat(_dst, ";");
    strcat(_dst, p_env_path.data());

    lua_pop(this->_lua, 1);               // get rid of the string on the stack we just pushed on line 5
    lua_pushstring(this->_lua, _dst);     // push the new one
    lua_setfield(this->_lua, -2, "path"); // set the field "path" in table at -2 with value at top of stack
    lua_pop(this->_lua, 1);               // get rid of package table from top of stack

    return 0;
}

int luaJIT::_set_value(const lua_in_value &p_value) const
{
    int _ret = 1;
    const auto _type = p_value.t;
    auto *_val = p_value.v.data();

    switch (_type)
    {
    default:
        _ret = -1;
        break;
    case LuaType::L_NIL:
    {
        lua_pushnil(this->_lua);
        break;
    }
    case LuaType::L_BOOLEAN:
    {
        bool _v = false;
        memcpy(&_v, _val, sizeof(bool));
        lua_pushboolean(this->_lua, _v);
        break;
    }
    case LuaType::L_DOUBLE:
    {
        double _v = 0;
        memcpy(&_v, _val, sizeof(double));
        lua_pushnumber(this->_lua, _v);
        break;
    }
    case LuaType::L_STRING:
    {
        lua_pushstring(this->_lua, (const char *)_val);
        break;
    }
    case LuaType::L_INTEGER:
    {
        int _v = 0;
        memcpy(&_v, _val, sizeof(int));
        lua_pushinteger(this->_lua, _v);
        break;
    }
    }
    return _ret;
}

int luaJIT::_get_value(
    const int p_index,
    lua_inout_value &p_value) const
{
    int _ret = 0;
    auto v = p_value.v.data();
    auto _type = lua_type(this->_lua, p_index);
    switch (_type)
    {
    default:
    {
        _ret = -1;
        break;
    }
    case LUA_TNIL:
    {
        p_value.t = LuaType::L_NIL;
        break;
    }
    case LUA_TBOOLEAN:
    {
        p_value.t = LuaType::L_BOOLEAN;
        auto b = (bool)lua_toboolean(this->_lua, p_index);
        memset(v, b, sizeof(b));
        break;
    }
    case LUA_TNUMBER:
    {
        p_value.t = LuaType::L_DOUBLE;
        auto d = lua_tonumber(this->_lua, p_index);
        memcpy(v, &d, sizeof(d));
        break;
    }
    case LUA_TSTRING:
    {
        p_value.t = LuaType::L_STRING;
        auto s = lua_tostring(this->_lua, p_index);
        strcpy((char *)v, s);
        break;
    }
    }

    return _ret;
}

std::unique_ptr<luaJIT> New()
{
    return std::make_unique<luaJIT>();
}
