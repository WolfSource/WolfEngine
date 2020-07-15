#include "w_lua.h"
#include <apr-1/apr_strings.h>
#include <io/w_io.h>

lua_State*		s_lua;
char*		    s_last_error;
char*		    s_function_name;
unsigned char	s_function_number_input_parameters;

void _VL(_In_ int pHR)
{
    if (pHR)
    {
        apr_snprintf(s_last_error,
            W_MAX_BUFFER_SIZE,
            "lua: %s",
            lua_tostring(s_lua, -1));
        lua_pop(s_lua, 1);
    }
}

W_RESULT _lua_get_value(_In_    int pIndex,
                        _Inout_ void* pValue,
                        _Inout_ char* pExpectedType)
{
    int _type = lua_type(s_lua, pIndex);
    switch (_type)
    {
        default:
            apr_snprintf(pExpectedType, W_PATH_MAX, "unknown");
            return W_FAILURE;
        case LUA_TNIL:
        {
            pValue = NULL;
            apr_snprintf(pExpectedType, W_PATH_MAX, "NULL");
            return W_SUCCESS;
        }
        case LUA_TBOOLEAN:
        {
            int* _v = w_malloc(sizeof(int), "_lua_get_value::LUA_TBOOLEAN");
            if (!_v)
            {
                return W_FAILURE;
            }
            
            *_v = lua_toboolean(s_lua, pIndex);
            pValue = (void*)_v;
            apr_snprintf(pExpectedType, W_PATH_MAX, "BOOLEAN");
            return W_SUCCESS;
        }
        case LUA_TNUMBER:
        {
            double* _v = w_malloc(sizeof(double), "_lua_get_value::LUA_TNUMBER");
            if (!_v)
            {
                return W_FAILURE;
            }
            
            *_v = lua_tonumber(s_lua, pIndex);
            pValue = (void*)_v;
            apr_snprintf(pExpectedType, W_PATH_MAX, "NUMBER");
            return W_SUCCESS;
        }
        case LUA_TSTRING:
        {
            pValue = (void*)lua_tostring(s_lua, pIndex);
            apr_snprintf(pExpectedType, W_PATH_MAX, "STRING");
            return W_SUCCESS;
        }
        case LUA_TTABLE:
        {
            apr_snprintf(pExpectedType, W_PATH_MAX, "TTABLE");
            return W_SUCCESS;
        }
        case LUA_TUSERDATA:
        {
            apr_snprintf(pExpectedType, W_PATH_MAX, "USERDATA");
            return W_SUCCESS;
        }
    }
}

W_RESULT w_lua_init(void)
{
    s_last_error = (char*)w_malloc(W_MAX_BUFFER_SIZE, "w_lua_init");
    s_function_name = (char*)w_malloc(W_PATH_MAX, "w_lua_init");
    s_function_number_input_parameters = 0;
    
    if (!s_function_name || !s_last_error)
    {
        return W_FAILURE;
    }
    return W_SUCCESS;
}

W_RESULT w_lua_load_file(_In_z_ const char* pPath)
{
    if (!pPath)
    {
        W_ASSERT(false, "pPath is NULL. trace info: w_lua_load_file");
        return W_FAILURE;
    }
    
	s_last_error[0] = '\0';

    W_RESULT _is_exists = w_io_file_check_is_file(pPath);
    if (_is_exists == W_FAILURE)
    {
        apr_snprintf(s_last_error,
            W_MAX_BUFFER_SIZE,
            "lua: lua file not exists on following path: %s", pPath);
        return W_FAILURE;
    }

	//initialize lua
	s_lua = luaL_newstate();
	if (!s_lua)
	{
		_VL(1);
		return W_FAILURE;
	}
	luaL_openlibs(s_lua);

	//load the lua file
	int _ret = luaL_loadfile(s_lua, pPath);
	if (_ret)
	{
		_VL(_ret);
		return W_FAILURE;
	}
	return W_SUCCESS;
}

W_RESULT w_lua_load_from_stream(const char* pBufferStream)
{
	if (!pBufferStream)
    {
        W_ASSERT(false, "pBuffer is NULL. trace info: w_lua_load_file");
        return W_FAILURE;
    }

	//initialize lua
	s_lua = luaL_newstate();
	if (!s_lua)
	{
		_VL(1);
		return W_FAILURE;
	}
	luaL_openlibs(s_lua);

	//load the lua buffer
	const size_t _buffer_size = strlen(pBufferStream);
	int _ret = luaL_loadbuffer(s_lua, pBufferStream, _buffer_size, pBufferStream);
	if (_ret)
	{
		_VL(_ret);
		return W_FAILURE;
	}
	return W_SUCCESS;
}

W_RESULT w_lua_run(void)
{
	int _hr = lua_pcall(s_lua, 0, LUA_MULTRET, 0);
	if (_hr)
	{
		_VL(_hr);
		return W_FAILURE;
	}
	return W_SUCCESS;
}

void w_lua_bind_to_cfunction(lua_CFunction pFunc, const char* pLuaFunctionName)
{
	lua_pushcfunction(s_lua, pFunc);
	lua_setglobal(s_lua, pLuaFunctionName);
}

void w_lua_prepare_function(const char* pFunctionName)
{
	s_function_number_input_parameters = 0;
    lua_getglobal(s_lua, pFunctionName);
    if (lua_isnil(s_lua, -1))
    {
        apr_snprintf(s_last_error,
            W_MAX_BUFFER_SIZE,
            "error function %s is null\n",
            pFunctionName);
        return;
    }
    const size_t _len = strlen(pFunctionName);
    apr_cpystrn(s_function_name, pFunctionName, _len);
}

void w_lua_execute_void_function()
{
    int _hr = lua_pcall(s_lua, s_function_number_input_parameters, 0, 0);
    _VL(_hr);
}

W_RESULT w_lua_execute_function_with_one_result(_In_ void** pResult)
{
    int _hr = lua_pcall(s_lua, s_function_number_input_parameters, 1, 0);
    _VL(_hr);

    char* _requested_type_error = NULL;
    if(_lua_get_value(-1, pResult, _requested_type_error) == W_FAILURE)
    {
        apr_snprintf(s_last_error, W_PATH_MAX, "lua: %s is type of %s\n",
                     s_function_name,
                     _requested_type_error);

        //w_log_error("%s. trace info: %s", s_last_error, "w_lua:w_lua_execute_function_with_one_result");
            
        return W_FAILURE;
    }
    
    return W_SUCCESS;
}

W_RESULT w_lua_execute_function_with_two_results(_In_ void** pResult1, _In_ void** pResult2)
{
    int _hr = lua_pcall(s_lua, s_function_number_input_parameters, 2, 0);
    _VL(_hr);

    char* _requested_type_error = NULL;
    if(_lua_get_value(-2, pResult1, _requested_type_error) == W_FAILURE)
    {
        apr_snprintf(s_last_error, W_PATH_MAX, "lua: %s is type of %s\n",
                     s_function_name,
                     _requested_type_error);
        return W_FAILURE;
    }
    
    if(_lua_get_value(-1, pResult2, _requested_type_error) == W_FAILURE)
    {
        apr_snprintf(s_last_error, W_PATH_MAX, "lua: %s is type of %s\n",
                     s_function_name,
                     _requested_type_error);
        return W_FAILURE;
    }
    
    return W_SUCCESS;
}

W_RESULT w_lua_execute_function_with_three_results(_In_ void** pResult1,
                                                   _In_ void** pResult2,
                                                   _In_ void** pResult3)
{
    int _hr = lua_pcall(s_lua, s_function_number_input_parameters, 3, 0);
    _VL(_hr);

    char* _requested_type_error = NULL;
    if(_lua_get_value(-3, pResult1, _requested_type_error) == W_FAILURE)
    {
        apr_snprintf(s_last_error, W_PATH_MAX, "lua: %s is type of %s\n",
                     s_function_name,
                     _requested_type_error);
        return W_FAILURE;
    }
    
    if(_lua_get_value(-2, pResult2, _requested_type_error) == W_FAILURE)
    {
        apr_snprintf(s_last_error, W_PATH_MAX, "lua: %s is type of %s\n",
                     s_function_name,
                     _requested_type_error);
        return W_FAILURE;
    }
    
    if(_lua_get_value(-1, pResult3, _requested_type_error) == W_FAILURE)
    {
        apr_snprintf(s_last_error, W_PATH_MAX, "lua: %s is type of %s\n",
                     s_function_name,
                     _requested_type_error);
        return W_FAILURE;
    }
    
    return W_SUCCESS;
}

lua_State* w_lua_get_state(void)
{
    return s_lua;
}

const char* w_lua_get_last_error()
{
    return s_last_error;
}

W_RESULT w_lua_get_global_variable(_In_z_ const char* pVariableName, _Inout_ void* pValue)
{
    lua_getglobal(s_lua, pVariableName);
    if (lua_isnil(s_lua, -1))
    {
        apr_snprintf(s_last_error, W_PATH_MAX, "%s is null\n", pVariableName);
        return W_FAILURE;
    }
    else
    {
        char* _requested_type_error = NULL;
        if(_lua_get_value(-1, pValue, _requested_type_error) == W_FAILURE)
        {
            apr_snprintf(s_last_error, W_PATH_MAX, "lua: %s is type of %s\n",
                         pVariableName,
                         _requested_type_error);
            return W_FAILURE;
        }
    }

    return W_SUCCESS;
}

W_RESULT _lua_set_value(_Inout_ void* pValue, _In_ int pValueType)
{
    switch (pValueType)
    {
        default:
            return W_FAILURE;
        case LUA_TNIL:
        {
            lua_pushnil(s_lua);
            return W_SUCCESS;
        }
        case LUA_TBOOLEAN:
        {
            bool _v = false;
            memcpy(&_v, pValue, sizeof(bool));
            lua_pushboolean(s_lua, _v);
            return W_SUCCESS;
        }
        case LUA_TNUMBER:
        {
            double _v = 0;
            memcpy(&_v, pValue, sizeof(double));
            lua_pushnumber(s_lua, _v);
            return W_SUCCESS;
        }
        case LUA_TSTRING:
        {
            lua_pushstring(s_lua, pValue);
            return W_SUCCESS;
        }
        case LUA_TTABLE:
        {
            return W_SUCCESS;
        }
        case LUA_TUSERDATA:
        {
            return W_SUCCESS;
        }
        case 9://integer
        {
            int _v = 0;
            memcpy(&_v, pValue, sizeof(int));
            lua_pushinteger(s_lua, _v);
            return W_SUCCESS;
        }
    }
}

W_RESULT w_lua_set_parameter_function(_In_ const void* pValue, _In_ int pValueType)
{
    s_function_number_input_parameters++;
    return _lua_set_value(pValue, pValueType);
}

W_RESULT w_lua_set_global_variable(_In_z_ const char*   pVariableName,
                                   _In_   int           pVariableType,
                                   _In_   const void*   pValue)
{
    lua_getglobal(s_lua, pVariableName);
    if (lua_isnil(s_lua, -1))
    {
        apr_snprintf(s_last_error, W_PATH_MAX, "%s is null\n", pVariableName);
        return W_FAILURE;
    }
    else
    {
        //set the value for global variable of lua
        if(_lua_set_value(pValue, pVariableType) == W_SUCCESS)
        {
            lua_setglobal(s_lua, pVariableName);
            return W_SUCCESS;
        }
        return W_FAILURE;
    }
}

W_RESULT w_lua_set_lua_path(_In_z_ const char* pPath)
{
    w_mem_pool _pool = w_get_default_memory_pool();
    if(!_pool)
    {
        W_ASSERT(false, "could not get default memory. trace info: w_string_create");
        return W_FAILURE;
    }
    
    lua_getglobal(s_lua, "package");
    lua_getfield(s_lua, -1, "path"); // get field "path" from table at top of stack (-1)
    const char* _cur_path = lua_tostring(s_lua, -1); // grab path string from top of stack
    _cur_path = apr_pstrcat(_pool, _cur_path, ";", pPath, NULL);
    lua_pop(s_lua, 1); // get rid of the string on the stack we just pushed on line 5
    lua_pushstring(s_lua, _cur_path); // push the new one
    lua_setfield(s_lua, -2, "path"); // set the field "path" in table at -2 with value at top of stack
    lua_pop(s_lua, 1); // get rid of package table from top of stack
    return W_SUCCESS;
}

W_RESULT w_lua_free()
{
    if (s_lua)
    {
        lua_pop(s_lua, 1);
        lua_close(s_lua);
    }

    w_free(s_last_error);
    w_free(s_function_name);
    
    return W_SUCCESS;
}
