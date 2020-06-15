/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_lua.h
	Description		 : lua script manager
	Comment          :
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"
#include <script/lua/lua.h>

/**
 * initialize lua resources
 * @return result
*/
W_RESULT w_lua_init(void);

/**
 * load lua script from file
 * @param pFilePath file path
 * @return result
*/
W_RESULT w_lua_load_file(_In_z_ const char* pFilePath);

/**
 * load from buffer
 * @param pBuffer buffer
 * @return result
*/
W_RESULT w_lua_load_from_buffer(_In_z_ const char* pBuffer);

/**
 * run lua
 * @return result
*/
W_RESULT w_lua_run(void);

/**
 * bind to the c function
 * @param pFunc lua function
 * @param pLuaFunctionName lua function name
*/
void w_lua_bind_to_cfunction(_In_ lua_CFunction pFunc, _In_z_ const char* pLuaFunctionName);

/**
 * prepare lua function
 * @param pFunctionName name of function
*/
void w_lua_prepare_function(const char* pFunctionName);

/**
 * execute lua function
*/
void w_lua_execute_void_function(void);

/**
 * release lua resources
 * @param pResult is result of the function
 * @return result
*/
W_RESULT w_lua_execute_function_with_one_result(_In_ void** pResult);

/**
 * release lua resources
 * @param pResult1 is the first result of the function
 * @param pResult2 is the second result of the function
 * @return result
*/
W_RESULT w_lua_execute_function_with_two_results(_In_ void** pResult1, _In_ void** pResult2);

/**
 * release lua resources
 * @param pResult1 is the first result of the function
 * @param pResult2 is the second result of the function
 * @param pResult3 is the third result of the function
 * @return result
*/
W_RESULT w_lua_execute_function_with_three_results(_In_ void** pResult1,
                                                   _In_ void** pResult2,
                                                   _In_ void** pResult3);

/**
 * return the lua state for custom operation
 * @return pointer to lua_State
*/
lua_State*  w_lua_get_state(void);

/**
 * return the last error
 * @return last error
*/
const char* w_lua_get_last_error(void);

/**
 * get global variable
 * @param pVariableName name of variable
 * @param pValue value of variable
 * @return result
*/
W_RESULT    w_lua_get_global_variable(_In_z_ const char* pVariableName, _Inout_ void* pValue);

/**
 * set function parameter
 * @param pValue parameter
 * @param pValueType type of param
 *          LUA_TNIL            0
            LUA_TBOOLEAN        1
            LUA_TLIGHTUSERDATA  2
            LUA_TNUMBER         3
            LUA_TSTRING         4
            LUA_TTABLE          5
            LUA_TFUNCTION       6
            LUA_TUSERDATA       7
            LUA_TTHREAD         8
            LUA_INTEGER         9
 * @return result
*/
W_RESULT w_lua_set_parameter_function(_In_ const void* pValue, _In_ const int pValueType);

/**
 *  set global variable
 * @param pVariableName name of variable
 * @param pVariableType type of variable
            LUA_TNIL            0
            LUA_TBOOLEAN        1
            LUA_TLIGHTUSERDATA  2
            LUA_TNUMBER         3
            LUA_TSTRING         4
            LUA_TTABLE          5
            LUA_TFUNCTION       6
            LUA_TUSERDATA       7
            LUA_TTHREAD         8
            LUA_INTEGER         9
 * @param pValue value of variable
 * @return result
*/
W_RESULT w_lua_set_global_variable(_In_z_   const char* pVariableName,
                                   _In_     const int   pVariableType,
                                   _In_     const void* pValue);

/**
 * set lua path
 * @param pPath path
 * @return result
*/
W_RESULT w_lua_set_lua_path(_In_z_ const char* pPath);

/**
 * release lua resources
 * @return result
*/
W_RESULT w_lua_free(void);


