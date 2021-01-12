/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_lua.h
	Description		 : lua script manager
	Comment          :
*/

#pragma once

#ifdef WOLF_ENABLE_LUA

#ifdef __cplusplus
extern "C" {
#endif

#include "wolf.h"
#include <script/lua/lua.h>

    /**
     * initialize lua resources
     * @param pMemPool The pool to allocate out of
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_lua_init(_Inout_ w_mem_pool pMemPool);

    /**
     * load lua script from file
     * @param pFilePath file path
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_lua_load_file(_In_z_ const char* pPath);

    /**
     * load from buffer
     * @param pBuffer buffer
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_lua_load_from_stream(_In_z_ const char* pBufferStream);

    /**
     * run lua
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_lua_run(void);

    /**
     * bind to the c function
     * @param pFunc lua function
     * @param pLuaFunctionName lua function name
    */
    W_SYSTEM_EXPORT
        void w_lua_bind_to_cfunction(
            _In_ lua_CFunction pFunc,
            _In_z_ const char* pLuaFunctionName);

    /**
     * prepare lua function
     * @param pFunctionName name of function
    */
    W_SYSTEM_EXPORT
        void w_lua_prepare_function(_In_z_ const char* pFunctionName);

    /**
     * execute lua function
    */
    W_SYSTEM_EXPORT
        void w_lua_execute_void_function(void);

    /**
     * release lua resources
     * @param pMemPool The pool to allocate out of
     * @param pResult is result of the function
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_lua_execute_function_with_one_result(
            _Inout_ w_mem_pool pMemPool,
            _In_ void** pResult);

    /**
     * release lua resources
     * @param pMemPool The pool to allocate out of
     * @param pResult1 is the first result of the function
     * @param pResult2 is the second result of the function
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_lua_execute_function_with_two_results(
            _Inout_ w_mem_pool pMemPool,
            _In_ void** pResult1,
            _In_ void** pResult2);

    /**
     * release lua resources
     * @param pMemPool The pool to allocate out of
     * @param pResult1 is the first result of the function
     * @param pResult2 is the second result of the function
     * @param pResult3 is the third result of the function
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_lua_execute_function_with_three_results(
            _Inout_ w_mem_pool pMemPool,
            _In_ void** pResult1,
            _In_ void** pResult2,
            _In_ void** pResult3);

    /**
     * return the lua state for custom operation
     * @return pointer to lua_State
    */
    W_SYSTEM_EXPORT
        lua_State* w_lua_get_state(void);

    /**
     * return the last error
     * @return last error
    */
    W_SYSTEM_EXPORT
        const char* w_lua_get_last_error(void);

    /**
     * get global variable
     * @param pMemPool The pool to allocate out of
     * @param pVariableName name of variable
     * @param pValue value of variable
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_lua_get_global_variable(
            _Inout_ w_mem_pool pMemPool,
            _In_z_ const char* pVariableName,
            _Inout_ void** pValue);

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
    W_SYSTEM_EXPORT
        W_RESULT w_lua_set_parameter_function(
            _In_ void* pValue,
            _In_ int pValueType);

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
    W_SYSTEM_EXPORT
        W_RESULT w_lua_set_global_variable(
            _In_z_   const char* pVariableName,
            _In_     int         pVariableType,
            _In_     void*       pValue);

    /**
     * set lua path
     * @param pMemPool The pool to allocate out of
     * @param pPath path
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_lua_set_lua_path(
            _Inout_ w_mem_pool pMemPool,
            _In_z_ const char* pPath);

    /**
     * release lua resources
     * @return result
    */
    W_SYSTEM_EXPORT
        W_RESULT w_lua_fini(void);

#ifdef __cplusplus
}
#endif

#endif
