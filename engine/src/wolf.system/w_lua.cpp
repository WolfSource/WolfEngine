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
		V(S_FALSE, _last_error, "w_lua", 3);
		lua_pop(_lua, 1);
	}
}

void w_lua::_incompatible_type_for_variable(const char* pVariableName, const char* pRequestedType, int pOriginalType)
{
	char _msg[256];
	switch (pOriginalType)
	{
	case -1:
		sprintf(_msg, "lua: %s is type of none, not %s\n", pVariableName, pRequestedType);
		break;
	case 0:
		sprintf(_msg, "lua: %s is type of nil, not %s\n", pVariableName, pRequestedType);
		break;
	case 1:
		sprintf(_msg, "lua: %s is type of boolean, not %s\n", pVariableName, pRequestedType);
		break;
	case 2:
		sprintf(_msg, "lua: %s is type of lightuserdata, not %s\n", pVariableName, pRequestedType);
		break;
	case 3:
		sprintf(_msg, "lua: %s is type of number, not %s\n", pVariableName, pRequestedType);
		break;
	case 4:
		sprintf(_msg, "lua: %s is type of string, not %s\n", pVariableName, pRequestedType);
		break;
	case 5:
		sprintf(_msg, "lua: %s is type of table, not %s\n", pVariableName, pRequestedType);
		break;
	case 6:
		sprintf(_msg, "lua: %s is type of function, not %s\n", pVariableName, pRequestedType);
		break;
	case 7:
		sprintf(_msg, "lua: %s is type of userdata, not %s\n", pVariableName, pRequestedType);
		break;
	case 8:
		sprintf(_msg, "lua: %s is type of thread, not %s\n", pVariableName, pRequestedType);
		break;
	}

	_last_error = _msg;
	V(S_FALSE, _last_error, "w_lua", 2);
}

HRESULT w_lua::load_file(const wchar_t* pPath)
{
	_last_error.clear();

	auto _utf8_path = wolf::system::convert::to_utf8(pPath);
	auto _is_exists = wolf::system::io::get_is_file(pPath);

	//initialize lua
	_lua = luaL_newstate();
	if (!_lua)
	{
		_VL(1);
		return S_FALSE;
	}
	luaL_openlibs(_lua);

	//load the lua file
	int _hr = luaL_loadfile(_lua, _utf8_path.c_str());
	if (_hr)
	{
		_VL(_hr);
		return S_FALSE;
	}
	return S_OK;
}

HRESULT w_lua::run()
{
	int _hr = lua_pcall(_lua, 0, LUA_MULTRET, 0);
	if (_hr)
	{
		_VL(_hr);
		return S_FALSE;
	}
	return S_OK;
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
			char _msg[256];
			sprintf(_msg, "error function %s is null\n", pFunctionName);
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

	int _r = lua_tonumber(_lua, -1);
	OutputDebugStringA(std::to_string(_r).c_str());
}

ULONG w_lua::release()
{
	lua_pop(_lua, 1);
	lua_close(_lua);

	return 1;
}
