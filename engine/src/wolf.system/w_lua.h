#ifndef __W_LUA_H__
#define __W_LUA_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef WIN32
#pragma comment(lib, "libluajit.lib")

#include <Windows.h>
#endif

#include "w_system_export.h"
#include <lua.hpp>
#include <string>

namespace wolf
{
	namespace system
	{
		class w_lua
		{
		public:
			WSYS_EXP static HRESULT			load_file(const wchar_t* pPath);
			WSYS_EXP static HRESULT			run();
			/*
				lua_CFunction		: the c function binder for lua
				pLuaFunctionName	: how function will be named in Lua
			*/
			WSYS_EXP static void			bind_to_cfunction(lua_CFunction pFunc, const char* pLuaFunctionName);
			WSYS_EXP static ULONG			release();

			WSYS_EXP static void			prepare_function(const char* pFunctionName);
			
			//make sure call run and prepare_function before calling this function
			template<typename T>
			static void						set_parameter_function(const T pParam);

			WSYS_EXP static void			execute_function();

			template<typename T>
			static void						execute_function(T& pResult);

			template<typename T1, typename T2>
			static void						execute_function(T1& pResult1, T2& pResult2);

			template<typename T1, typename T2, typename T3>
			static void						execute_function(T1& pResult1, T2& pResult2, T3& pResult3);

#pragma region Getters

			//return the last error
			WSYS_EXP static const char*		get_last_error() 				{ return _last_error.c_str(); };

			//get the global variable in lua script
			template <typename T>
			static HRESULT					get_global_variable(const char* pVariableName, T& pValue);

#pragma endregion

#pragma region Setters

			//set the global variable in lua script
			template <typename T>
			static HRESULT					set_global_variable(const char* pVariableName, const T pValue);

#pragma endregion

		private:
			/*
				make sure define these private function as __declspec(dllexport) because we need them in the template functions
			*/

			//validate lua result
			WSYS_EXP static void				_VL(int pHR);
			//log the error for incompatible requsted type for lua variable
			WSYS_EXP static void				_incompatible_type_for_variable(const char* pVariableName, const char* pRequestedType, int pOriginalType);

			//the lua state
			WSYS_EXP static lua_State*		_lua;
			//the last error
			WSYS_EXP static std::string		_last_error;
			//name of lua function
			WSYS_EXP static std::string		_function_name;
			//number of input parameters for lua function
			WSYS_EXP static unsigned char	_function_number_input_parameters;
		};

#pragma region Templates
		
#pragma region Getters

		/*
			compile time template functions for getting value of lua variable based on their types
		*/
		template<typename T>
		auto get_value(lua_State* pLua, int pIndex, _Inout_ T& pValue, int& pVarType, std::string& pRequestedTypeError) -> typename std::enable_if<std::is_same<T, bool>::value, void>::type
		{
			pVarType = lua_type(pLua, pIndex);
			if (pVarType == LUA_TBOOLEAN)
			{
				pValue = lua_toboolean(pLua, pIndex);
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "bool";
			}
		}

		template<typename T>
		auto get_value(lua_State* pLua, int pIndex, _Inout_ T& pValue, int& pVarType, std::string& pRequestedTypeError) -> typename std::enable_if<std::is_same<T, int>::value, void>::type
		{
			pVarType = lua_type(pLua, pIndex);
			if (pVarType == LUA_TNUMBER)
			{
				pValue = lua_tointeger(pLua, pIndex);
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "integer";
			}
		}

		template<typename T>
		auto get_value(lua_State* pLua, int pIndex, _Inout_ T& pValue, _Inout_ int& pVarType, _Inout_ std::string& pRequestedTypeError) -> typename std::enable_if<std::is_floating_point<T>::value, void>::type
		{
			pVarType = lua_type(pLua, pIndex);
			if (pVarType == LUA_TNUMBER)
			{
				pValue = lua_tonumber(pLua, pIndex);
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "floating point";
			}
		}

		template<typename T >
		auto get_value(lua_State* pLua, int pIndex, _Inout_ T& pValue, int& pVarType, std::string& pRequestedTypeError) -> typename std::enable_if<std::is_same<T, std::wstring>::value, void>::type
		{
			pVarType = lua_type(pLua, pIndex);
			if (pVarType == LUA_TSTRING)
			{
				auto _str = lua_tostring(pLua, pIndex);
				pValue = wolf::system::convert::from_utf8(_str);
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "wstring";
			}
		}

		template<typename T >
		auto get_value(lua_State* pLua, int pIndex, _Inout_ T& pValue, int& pVarType, std::string& pRequestedTypeError) -> typename std::enable_if<std::is_same<T, std::string>::value, void>::type
		{
			pVarType = lua_type(pLua, pIndex);
			if (pVarType == LUA_TSTRING)
			{
				pValue = lua_tostring(pLua, pIndex);
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "string";
			}
		}

		/*
			get value from lua variable
		*/
		template <typename T>
		HRESULT w_lua::get_global_variable(const char* pVariableName, _Inout_ T& pValue)
		{
			lua_getglobal(_lua, pVariableName);
			if (lua_isnil(_lua, -1))
			{
				char _msg[256];
				sprintf(_msg, "%s is null\n", pVariableName);
				_last_error = _msg;

				return S_FALSE;
			}
			else
			{
				std::string _requested_type_error;
				int _original_lua_type = LUA_TNONE;
				get_value(_lua, -1, pValue, _original_lua_type, _requested_type_error);
				if (!_requested_type_error.empty())
				{
					_incompatible_type_for_variable(pVariableName, _requested_type_error.c_str(), _original_lua_type);
				}
			}

			return S_OK;
		}

#pragma endregion

#pragma region Setters

		/*
			compile time template functions for setting value of lua variable based on their types
		*/
		template<typename T>
		auto set_value(lua_State* pLua, const T pValue, int& pVarType, std::string& pRequestedTypeError) -> typename std::enable_if<std::is_same<T, bool>::value, void>::type
		{
			pVarType = lua_type(pLua, -1);
			if (pVarType == LUA_TBOOLEAN)
			{
				lua_pushinteger(pLua, pValue);
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "bool";
			}
		}

		template<typename T>
		auto set_value(lua_State* pLua, const T pValue, int& pVarType, std::string& pRequestedTypeError) -> typename std::enable_if<std::is_same<T, int>::value, void>::type
		{
			pVarType = lua_type(pLua, -1);
			if (pVarType == LUA_TNUMBER)
			{
				lua_pushinteger(pLua, pValue);
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "integer";
			}
		}

		template<typename T>
		auto set_value(lua_State* pLua, const T pValue, _Inout_ int& pVarType, _Inout_ std::string& pRequestedTypeError) -> typename std::enable_if<std::is_floating_point<T>::value, void>::type
		{
			pVarType = lua_type(pLua, -1);
			if (pVarType == LUA_TNUMBER)
			{
				lua_pushnumber(pLua, pValue);
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "floating point";
			}
		}

		template<typename T >
		auto set_value(lua_State* pLua, const T pValue, int& pVarType, std::string& pRequestedTypeError) -> typename std::enable_if<std::is_same<T, const wchar_t*>::value, void>::type
		{
			pVarType = lua_type(pLua, -1);
			if (pVarType == LUA_TSTRING)
			{
				auto _str = wolf::system::convert::to_utf8(std::wstring(pValue));
				lua_pushstring(pLua, _str.c_str());
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "wstring";
			}
		}

		template<typename T >
		auto set_value(lua_State* pLua, const T pValue, int& pVarType, std::string& pRequestedTypeError) -> typename std::enable_if<std::is_same<T, std::wstring>::value, void>::type
		{
			pVarType = lua_type(pLua, -1);
			if (pVarType == LUA_TSTRING)
			{
				auto _str = wolf::system::convert::wstring_to_utf8_string(pValue);
				lua_pushstring(pLua, _str.c_str());
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "wstring";
			}
		}

		/*
			set value of lua variable
		*/
		template<typename T >
		auto set_value(lua_State* pLua, const T pValue, int& pVarType, std::string& pRequestedTypeError) -> typename std::enable_if<std::is_same<T, const char*>::value, void>::type
		{
			pVarType = lua_type(pLua, -1);
			if (pVarType == LUA_TSTRING)
			{
				lua_pushstring(pLua, pValue);
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "string";
			}
		}

		template<typename T >
		auto set_value(lua_State* pLua, const T pValue, int& pVarType, std::string& pRequestedTypeError) -> typename std::enable_if<std::is_same<T, std::string>::value, void>::type
		{
			pVarType = lua_type(pLua, -1);
			if (pVarType == LUA_TSTRING)
			{
				lua_pushstring(pLua, pValue.c_str());
				pRequestedTypeError.clear();
			}
			else
			{
				pRequestedTypeError = "string";
			}
		}

		template <typename T>
		HRESULT	w_lua::set_global_variable(const char* pVariableName, const T pValue)
		{
			lua_getglobal(_lua, pVariableName);
			if (lua_isnil(_lua, -1))
			{
				char _msg[256];
				sprintf(_msg, "%s is null\n", pVariableName);
				_last_error = _msg;

				return S_FALSE;
			}
			else
			{
				std::string _requested_type_error;
				int _original_lua_type = LUA_TNONE;

				//set the value for global variable of lua
				set_value(_lua, pValue, _original_lua_type, _requested_type_error);
				
				if (_requested_type_error.empty())
				{
					lua_setglobal(_lua, pVariableName);
				}
				else
				{
					_incompatible_type_for_variable(pVariableName, _requested_type_error.c_str(), _original_lua_type);
				}
			}
		}

		/*
			compile time template functions for setting parameter of lua function based on their types
		*/
		template<typename T>
		auto set_parameter(lua_State* pLua, const T pValue) -> typename std::enable_if<std::is_same<T, bool>::value, void>::type
		{
			lua_pushboolean(pLua, pValue);
		}

		template<typename T>
		auto set_parameter(lua_State* pLua, const T pValue) -> typename std::enable_if<std::is_same<T, int>::value, void>::type
		{
			lua_pushinteger(pLua, pValue);
		}

		template<typename T>
		auto set_parameter(lua_State* pLua, const T pValue) -> typename std::enable_if<std::is_floating_point<T>::value, void>::type
		{
			lua_pushnumber(pLua, pValue);
		}

		template<typename T >
		auto set_parameter(lua_State* pLua, const T pValue) -> typename std::enable_if<std::is_same<T, const wchar_t*>::value, void>::type
		{
			auto _str = wolf::system::convert::wstring_to_utf8_string(std::wstring(pValue));
			lua_pushstring(pLua, _str.c_str());
		}

		template<typename T >
		auto set_parameter(lua_State* pLua, const T pValue) -> typename std::enable_if<std::is_same<T, std::wstring>::value, void>::type
		{
			auto _str = wolf::system::convert::wstring_to_utf8_string(pValue);
			lua_pushstring(pLua, _str.c_str());
		}

		template<typename T >
		auto set_parameter(lua_State* pLua, const T pValue) -> typename std::enable_if<std::is_same<T, const char*>::value, void>::type
		{
			lua_pushstring(pLua, pValue);
		}

		template<typename T >
		auto set_parameter(lua_State* pLua, const T pValue) -> typename std::enable_if<std::is_same<T, std::string>::value, void>::type
		{
			lua_pushstring(pLua, pValue.c_str());
		}

		template<typename T>
		void w_lua::set_parameter_function(const T pValue)
		{
			_function_number_input_parameters++;
			set_parameter(_lua, pValue);
		}

#pragma endregion

#pragma region Executes
		
		/*
			execute function of lua with one return result
		*/
		template<typename T>
		void w_lua::execute_function(T& pResult)
		{
			auto _hr = lua_pcall(_lua, _function_number_input_parameters, 1, 0);
			_VL(_hr);

			std::string _requested_type_error;
			int _original_lua_type = LUA_TNONE;
			get_value(_lua, -1, pResult, _original_lua_type, _requested_type_error);
			if (!_requested_type_error.empty())
			{
				std::string _function_name_str = std::string("first parameter of function: ") + _function_name;

				_incompatible_type_for_variable(
					_function_name_str.c_str(),
					_requested_type_error.c_str(), 
					_original_lua_type);
			}
		}

		/*
			execute function of lua with two return results
		*/
		template<typename T1, typename T2>
		void w_lua::execute_function(T1& pResult1, T2& pResult2)
		{
			auto _hr = lua_pcall(_lua, _function_number_input_parameters, 2, 0);
			_VL(_hr);

			//get first return
			std::string _requested_type_error;
			int _original_lua_type = LUA_TNONE;
			get_value(_lua, -2, pResult1, _original_lua_type, _requested_type_error);
			if (!_requested_type_error.empty())
			{
				std::string _function_name_str = std::string("first parameter of function: ") + _function_name;

				_incompatible_type_for_variable(
					_function_name_str.c_str(),
					_requested_type_error.c_str(),
					_original_lua_type);
			}

			_requested_type_error.clear();
			_original_lua_type = LUA_TNONE;
			get_value(_lua, -1, pResult2, _original_lua_type, _requested_type_error);
			if (!_requested_type_error.empty())
			{
				std::string _function_name_str = std::string("second parameter of function: ") + _function_name;

				_incompatible_type_for_variable(
					_function_name_str.c_str(),
					_requested_type_error.c_str(),
					_original_lua_type);
			}
		}

		/*
			execute function of lua with three return results
		*/
		template<typename T1, typename T2, typename T3>
		void w_lua::execute_function(T1& pResult1, T2& pResult2, T3& pResult3)
		{
			auto _hr = lua_pcall(_lua, _function_number_input_parameters, 3, 0);
			_VL(_hr);

			std::string _requested_type_error;
			int _original_lua_type = LUA_TNONE;
			get_value(_lua, -3, pResult1, _original_lua_type, _requested_type_error);
			if (!_requested_type_error.empty())
			{
				std::string _function_name_str = std::string("first parameter of function: ") + _function_name;

				_incompatible_type_for_variable(
					_function_name_str.c_str(),
					_requested_type_error.c_str(),
					_original_lua_type);
			}

			_requested_type_error.clear();
			_original_lua_type = LUA_TNONE;
			get_value(_lua, -2, pResult2, _original_lua_type, _requested_type_error);
			if (!_requested_type_error.empty())
			{
				std::string _function_name_str = std::string("second parameter of function: ") + _function_name;

				_incompatible_type_for_variable(
					_function_name_str.c_str(),
					_requested_type_error.c_str(),
					_original_lua_type);
			}

			_requested_type_error.clear();
			_original_lua_type = LUA_TNONE;
			get_value(_lua, -1, pResult3, _original_lua_type, _requested_type_error);
			if (!_requested_type_error.empty())
			{
				std::string _function_name_str = std::string("third parameter of function: ") + _function_name;

				_incompatible_type_for_variable(
					_function_name_str.c_str(),
					_requested_type_error.c_str(),
					_original_lua_type);
			}
		}

#pragma endregion

#pragma endregion

	}
}

#endif //__W_LUA_H__