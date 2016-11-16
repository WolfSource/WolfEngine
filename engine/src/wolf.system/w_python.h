/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_python.h
	Description		 : This class responsible to bind python and c++ functions callbacks 
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __W_PYTHON_H__
#define __W_PYTHON_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef WIN32

#ifdef _DEBUG

#pragma comment(lib, "python35.lib")
#pragma comment(lib, "libboost_python-vc140-mt-gd-1_61.lib")
#pragma comment(lib, "libboost_python3-vc140-mt-gd-1_61.lib")

#else

#pragma comment(lib, "python35.lib")
#pragma comment(lib, "libboost_python-vc140-mt-1_61.lib")
#pragma comment(lib, "libboost_python3-vc140-mt-1_61.lib")

#endif

#endif

#include "w_system_dll.h"
#include "w_object.h"

#ifndef BOOST_PYTHON_SOURCE
#define BOOST_PYTHON_STATIC_LIB
#endif

#include <boost/python.hpp>

namespace wolf
{
	namespace system
	{
		template<typename T>
		class w_python_result
		{
			std::string name = "";
			T value;
		};

		class w_python : public wolf::system::w_object
		{
		public:
			SYS_EXP w_python();
			SYS_EXP virtual ~w_python();
			SYS_EXP HRESULT run(std::string pScriptText);
			SYS_EXP ULONG release() override;
			
			template <class T>
			HRESULT register_class_methods(std::string pName, boost::python::class_<T> pClass)
			{
				auto _hr = S_OK;
				try
				{
					this->_main_namespace[pName] = pClass;
				}
				catch (boost::python::error_already_set&)
				{
					_hr = S_FALSE;
					V(_hr, _fetch_error(), this->name, 3);
				}
				return _hr;
			}

			template <class T>
			HRESULT register_pointer_class(std::string pName, boost::python::pointer_wrapper<T> pPtr)
			{
				auto _hr = S_OK;
				try
				{
					this->_main_namespace[pName] = pPtr;
				}
				catch (boost::python::error_already_set&)
				{
					_hr = S_FALSE;
					V(_hr, _fetch_error(), this->name, 3);
				}

				return _hr;
			}

			template <typename T>
			T get_result(std::string pName)
			{
				T _value;
				try
				{
					_value = boost::python::extract<T>(this->_main_namespace[pName]);
				}
				catch (boost::python::error_already_set&)
				{
					logger.error("error on casting python result for : " + pName);
				}
				return _value;
			}

#pragma region Setters

			

#pragma endregion

#pragma region Getters

#pragma endregion

		private:
			typedef w_object _super;

			WSYS_EXP const std::wstring  _fetch_error();
			
			std::wstring _log;
			boost::python::object _main_namespace;
		};
	}
}

#endif //__W_PYTHON_H__

#endif//__PYTHON__