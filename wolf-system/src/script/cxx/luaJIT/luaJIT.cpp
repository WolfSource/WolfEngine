#include "luaJIT.hpp"
#include "wolf-system/src/script/lua.rs.h"
#include <string.h>

#ifdef DEBUG
#include <iostream>
#endif

constexpr auto MAX_STRING_LUA_SIZE = 4096;

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

int luaJIT::call(rust::Str p_name, rust::Slice<const LuaValue> p_params, rust::Vec<LuaValue> &p_results) const
{
    char _name[MAX_STRING_LUA_SIZE];
    auto _len = p_name.length();
    if (!_len || _len > MAX_STRING_LUA_SIZE - 1)
    {
        return -1;
    }
    memcpy(_name, p_name.data(), _len);
    _name[_len] = '\0';

    lua_getglobal(this->_lua, _name);
    auto _type = lua_type(this->_lua, -1);
    if (_type == LUA_TFUNCTION)
    {
        size_t _num_of_params = p_params.length();
        int _stack_size = lua_gettop(this->_lua);

        //set args
        for (size_t i = 0; i < _num_of_params; ++i)
        {
            _set_value(p_params[i]);
        }

        //call lua function function
        auto _ret = lua_pcall(this->_lua, _num_of_params, LUA_MULTRET, 0);
        if (_ret)
        {
            return _ret;
        }

        int _num_returns = lua_gettop(this->_lua) - _stack_size + 1;
        if (_num_returns > 0)
        {
            p_results.reserve(_num_returns);
            //get results
            for (int i = 0; i < _num_returns; ++i)
            {
                LuaValue _v = {};
                _get_value(i - _num_returns, _v);
                p_results.push_back(_v);
            }
        }
        return 0;
    }
    return -1;
}

int luaJIT::get_global(rust::Str p_name, LuaValue &p_value) const
{
    char _name[MAX_STRING_LUA_SIZE];
    auto _len = p_name.length();
    if (!_len || _len > MAX_STRING_LUA_SIZE - 1)
    {
        return -1;
    }
    memcpy(_name, p_name.data(), _len);
    _name[_len] = '\0';

    lua_getglobal(this->_lua, _name);
    return _get_value(-1, p_value);
}

int luaJIT::set_global(rust::Str p_name, const LuaValue &p_value) const
{
    char _name[MAX_STRING_LUA_SIZE];
    auto _len = p_name.length();
    if (!_len || _len > MAX_STRING_LUA_SIZE - 1)
    {
        return -1;
    }
    memcpy(_name, p_name.data(), _len);
    _name[_len] = '\0';

    lua_getglobal(this->_lua, _name);
    //set the value for global variable of lua
    if (_set_value(p_value) == 0)
    {
        lua_setglobal(this->_lua, _name);
        return 0;
    }
    return -1;
}

int luaJIT::set_lua_path(rust::Str p_env_path) const
{
    if (p_env_path.empty())
    {
        return -1;
    }

    char _dst[MAX_STRING_LUA_SIZE]; //MAX_PATH_SIZE

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

int luaJIT::_set_value(const LuaValue &p_value) const
{
    int _ret = 0;
    const auto _type = p_value.t;

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
        memcpy(&_v, p_value.v.data(), p_value.v.size());
        lua_pushboolean(this->_lua, _v);
        break;
    }
    case LuaType::L_NUMBER:
    {
        double _v = 0;
        memcpy(&_v, p_value.v.data(), p_value.v.size());
        lua_pushnumber(this->_lua, _v);
        break;
    }
    case LuaType::L_STRING:
    {
        char _str[MAX_STRING_LUA_SIZE];
        auto _len = p_value.v.size();
        if (!_len || _len > MAX_STRING_LUA_SIZE - 1)
        {
            return -1;
        }
        memcpy(_str, p_value.v.data(), _len);
        _str[_len] = '\0';

        lua_pushlstring(this->_lua, _str, _len);
        break;
    }
    }
    return _ret;
}

int luaJIT::_get_value(
    const int p_index,
    LuaValue &p_value) const
{
    int _ret = 0;
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
        auto b = (uint8_t)lua_toboolean(this->_lua, p_index);
        p_value.v.push_back(b);
        break;
    }
    case LUA_TNUMBER:
    {
        //get lua number
        auto d = lua_tonumber(this->_lua, p_index);
        p_value.t = LuaType::L_NUMBER;

        // get size of double
        auto _size = sizeof(d);
        //cast double to bytes
        auto _bytes = reinterpret_cast<uint8_t *>(&d);

        //copy data to vector provided by rust
        p_value.v.reserve(_size);
        copy(&_bytes[0], &_bytes[_size], std::back_inserter(p_value.v));

        break;
    }
    case LUA_TSTRING:
    {
        //get lua number
        auto d = lua_tostring(this->_lua, p_index);
        p_value.t = LuaType::L_STRING;

        // get size of double
        auto _size = strlen(d);
        p_value.v.reserve(_size);

        //copy data to vector provided by rust
        copy(&d[0], &d[_size], std::back_inserter(p_value.v));

        break;
    }
    }

    return _ret;
}

std::unique_ptr<luaJIT> New()
{
    return std::make_unique<luaJIT>();
}