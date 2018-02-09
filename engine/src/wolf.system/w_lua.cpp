#include "w_system_pch.h"

#include "w_lua.h"

using namespace wolf::system;

lua_State*		w_lua::_lua;
std::string		w_lua::_last_error;
std::string		w_lua::_function_name;
unsigned char	w_lua::_function_number_input_parameters = 0;

void w_lua::_VL(int pHR)
{
	if (pHR)
	{
		_last_error = "lua: " + std::string(lua_tostring(_lua, -1));
		V(W_FALSE, _last_error, "w_lua", 3);
		lua_pop(_lua, 1);
	}
}

void w_lua::_incompatible_type_for_variable(const char* pVariableName, const char* pRequestedType, int pOriginalType)
{
    const int _length = 256;
	char _msg[_length];
	switch (pOriginalType)
	{
	case -1:
		w_sprintf(_msg, _length, "lua: %s is type of none, not %s\n", pVariableName, pRequestedType);
		break;
	case 0:
        w_sprintf(_msg, _length, "lua: %s is type of nil, not %s\n", pVariableName, pRequestedType);
		break;
	case 1:
        w_sprintf(_msg, _length, "lua: %s is type of boolean, not %s\n", pVariableName, pRequestedType);
		break;
	case 2:
        w_sprintf(_msg, _length, "lua: %s is type of lightuserdata, not %s\n", pVariableName, pRequestedType);
		break;
	case 3:
        w_sprintf(_msg, _length, "lua: %s is type of number, not %s\n", pVariableName, pRequestedType);
		break;
	case 4:
        w_sprintf(_msg, _length, "lua: %s is type of string, not %s\n", pVariableName, pRequestedType);
		break;
	case 5:
        w_sprintf(_msg, _length, "lua: %s is type of table, not %s\n", pVariableName, pRequestedType);
		break;
	case 6:
        w_sprintf(_msg, _length, "lua: %s is type of function, not %s\n", pVariableName, pRequestedType);
		break;
	case 7:
        w_sprintf(_msg, _length, "lua: %s is type of userdata, not %s\n", pVariableName, pRequestedType);
		break;
	case 8:
        w_sprintf(_msg, _length, "lua: %s is type of thread, not %s\n", pVariableName, pRequestedType);
		break;
	}

	_last_error = _msg;
	V(W_FALSE, _last_error, "w_lua", 2);
}

W_RESULT w_lua::load_file(const wchar_t* pPath)
{
	_last_error.clear();

    auto _utf8_path = wolf::system::convert::to_utf8(pPath);
#if defined(__WIN32) || defined(__UWP)
    auto _is_exists = wolf::system::io::get_is_file(pPath);
#else
   auto _c_str = wolf::system::convert::wstring_to_string(pPath).c_str();
    auto _is_exists = wolf::system::io::get_is_file(_c_str);
#endif
    
    if (_is_exists == W_FALSE)
    {
        _last_error = "lua: lua file not exists on following path: " + _utf8_path;
        return W_FALSE;
    }
    
	//initialize lua
	_lua = luaL_newstate();
	if (!_lua)
	{
		_VL(1);
		return W_FALSE;
	}
	luaL_openlibs(_lua);

	//load the lua file
	int _hr = luaL_loadfile(_lua, _utf8_path.c_str());
	if (_hr)
	{
		_VL(_hr);
		return W_FALSE;
	}
	return W_OK;
}

W_RESULT w_lua::run()
{
	int _hr = lua_pcall(_lua, 0, LUA_MULTRET, 0);
	if (_hr)
	{
		_VL(_hr);
		return W_FALSE;
	}
	return W_OK;
}

void w_lua::bind_to_cfunction(lua_CFunction pFunc, const char* pLuaFunctionName)
{
	lua_pushcfunction(_lua, pFunc);
	lua_setglobal(_lua, pLuaFunctionName);
}

void w_lua::prepare_function(const char* pFunctionName)
{
	_function_number_input_parameters = 0;
	_function_name.clear();
	{
		lua_getglobal(_lua, pFunctionName);
		if (lua_isnil(_lua, -1))
		{
            const int _length = 256;
			char _msg[_length];
            w_sprintf(_msg, _length, "error function %s is null\n", pFunctionName);
			_last_error = _msg;
			return;
		}
	}
	_function_name = pFunctionName;
}

void w_lua::execute_function()
{
	auto _hr = lua_pcall(_lua, _function_number_input_parameters, 0, 0);
	_VL(_hr);
}

ULONG w_lua::release()
{
	lua_pop(_lua, 1);
	lua_close(_lua);

	return 0;
}

W_RESULT w_lua::set_lua_path(_In_z_ const char* pPath)
{
	lua_getglobal(_lua, "package");
	lua_getfield(_lua, -1, "path"); // get field "path" from table at top of stack (-1)
	std::string cur_path = lua_tostring(_lua, -1); // grab path string from top of stack
	cur_path.append(";"); // do your path magic here
	cur_path.append(pPath);
	lua_pop(_lua, 1); // get rid of the string on the stack we just pushed on line 5
	lua_pushstring(_lua, cur_path.c_str()); // push the new one
	lua_setfield(_lua, -2, "path"); // set the field "path" in table at -2 with value at top of stack
	lua_pop(_lua, 1); // get rid of package table from top of stack
	return W_OK;
}
