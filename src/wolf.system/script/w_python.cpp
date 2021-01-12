#include "w_python.h"

#ifdef WOLF_ENABLE_PYTHON3

#ifndef BOOST_PYTHON_SOURCE
#define BOOST_PYTHON_STATIC_LIB
#endif
//undef it because of curl
#ifdef ssize_t
#undef ssize_t __int64
#endif
#include <boost/python.hpp>

using namespace std;
using namespace boost::python;

static const std::string s_fetch_error()
{
	PyObject* _type;
	PyObject* _value;
	PyObject* _traceback;

	std::string _error_msg;
	try
	{
		PyErr_Fetch(&_type, &_value, &_traceback);

		handle<> _handle_type(_type);
		object extype(_handle_type);

		handle<> _handle_traceback(_traceback);
		object traceback(_handle_traceback);

		//Extract error message
		wstring _str_error_message = extract<wstring>(_value);

		//	Extract line number (top entry of call stack)
		//	if you want to extract another levels of call stack
		//	also process traceback.attr("tb_next") recurently
		long _line_number = extract<long>(traceback.attr("tb_lineno"));
		wstring _func_name = extract<wstring>(traceback.attr("tb_frame").attr("f_code").attr("co_name"));
		wstring _file_name = extract<wstring>(traceback.attr("tb_frame").attr("f_code").attr("co_filename"));

		auto _error_msg_w = L"\r\nTraceback(most recent call last) :\r\n"
			"File \"" + _file_name + L"\", line " + std::to_wstring(_line_number) + L", in " + _func_name + L"\r\nNameError : " + _str_error_message;

		Py_DECREF(_type);
		Py_DECREF(_value);
		Py_DECREF(_traceback);

		_error_msg = std::string(_error_msg_w.begin(), _error_msg_w.end());
	}
	catch (...)
	{
		_error_msg = "something unexpected happended";
	}

	return _error_msg;
}

template <typename T>
static W_RESULT s_set_python_obj(
	w_mem_pool pMemPool, 
	w_python_object* pPyObj, 
	T pValue,
	std::string& pErrorMsg)
{
	if (!pPyObj  || !*pPyObj)
	{
		return W_BAD_ARG;
	}

	auto _size = sizeof(T);
	//allocate memory for data
	auto _obj = *pPyObj;
	_obj->data = w_malloc(pMemPool, _size);
	if (_obj->data)
	{
		//copy memory
		memcpy(_obj->data, (void*)&(pValue), _size);
		*pPyObj = _obj;
		return W_SUCCESS;
	}
	return W_FAILURE;
}

void w_python::init()
{
	Py_Initialize();
}

W_RESULT w_python::execute(
	_In_ w_mem_pool pMemPool,
	_In_z_ const char* pScriptText,
	_In_opt_ w_hash pKeyValueParameters,
	_Inout_opt_ w_string* pErrorMsg)
{
	std::string _error_msg;

	if (!pScriptText || pScriptText[0] == '\0')
	{
		return W_FAILURE;
	}

	auto _ret = W_SUCCESS;
	//create main python module
	object _main(import("__main__"));
	//create main namespace as dictionary
	object _global = _main.attr("__dict__");

	try
	{
		//execute script
		auto _obj = exec(pScriptText, _global, _global);
		//fetch results
		if (pKeyValueParameters)
		{
			for (auto _hash = w_hash_first(pMemPool, pKeyValueParameters); _hash; _hash = w_hash_next(_hash))
			{
				const char* _key = NULL;
				size_t _key_len = 0;
				w_python_object _value = NULL;
				
				w_hash_this(_hash, (const void**)&_key, &_key_len, (void**)&_value);
				if (_key && _value)
				{
					//fetch the value from python and cast it
					switch (_value->type)
					{
					case w_python_object_type::W_PYTHON_BOOLEAN://BOOLEAN
					{
						const auto _size = sizeof(bool);
						bool _python_value = extract<bool>(_global[_key]);
						if (s_set_python_obj<bool>(
							pMemPool, 
							&_value, 
							_python_value,
							_error_msg) == W_SUCCESS)
						{
							//update hash
							w_hash_set(pKeyValueParameters,
								_key,
								_key_len,
								(const void*)_value);
						}
						else
						{
							_error_msg +=
								"could not allocate memory for python data of key:" +
								std::string(_key) + "\r\n";
						}
						break;
					}
					case w_python_object_type::W_PYTHON_NUMBER_INT://INT
					{
						const auto _size = sizeof(int);
						int _python_value = extract<int>(_global[_key]);
						if (s_set_python_obj<int>(
							pMemPool, 
							&_value, 
							_python_value,
							_error_msg) == W_SUCCESS)
						{
							//update hash
							w_hash_set(pKeyValueParameters,
								_key,
								_key_len,
								(const void*)_value);
						}
						else
						{
							_error_msg +=
								"could not allocate memory for python data of key:" +
								std::string(_key) + "\r\n";
						}
						break;
					}
					case w_python_object_type::W_PYTHON_NUMBER_DOUBLE://DOUBLE
					{
						const auto _size = sizeof(double);
						double _python_value = extract<double>(_global[_key]);
						if (s_set_python_obj<double>(
							pMemPool,
							&_value,
							_python_value,
							_error_msg) == W_SUCCESS)
						{
							//update hash
							w_hash_set(pKeyValueParameters,
								_key,
								_key_len,
								(const void*)_value);
						}
						else
						{
							_error_msg +=
								"could not allocate memory for python data of key:" +
								std::string(_key) + "\r\n";
						}
						break;
					}
					case w_python_object_type::W_PYTHON_STRING://STRING
					{
						std::string _python_value = extract<std::string>(_global[_key]);
						const auto _size = _python_value.size() + 1; //with NULL
						auto _str = static_cast<char*>(w_malloc(pMemPool, _size));
						if (_str)
						{
							//copy memory
							strcpy_s(_str, _size, _python_value.c_str());

							_value->data = &_str[0];
							//update hash
							w_hash_set(pKeyValueParameters,
								_key,
								_key_len,
								(const void*)_value);
						}
						else
						{
							_error_msg +=
								"could not allocate memory for python data of key:" +
								std::string(_key) + "\r\n";
						}
						break;
					}
				}
				}
				else
				{
				std:string _k;
					if (_key)
					{
						_k = _key;
					}
					else
					{
						_k = "NULL";
					}
					_error_msg += "missing w_python_object memory for key:" + _k + "\r\n";
					_ret = W_BAD_ARG;
				}
			}
		}
	}
	catch (...)
	{
		//on any unexpected error
		_error_msg += "python exception: " + s_fetch_error() + "\r\n";
		_ret = W_FAILURE;
	}

	if (pErrorMsg && *pErrorMsg)
	{
		_error_msg += "caught memory leaks on pErrorMsg\r\n";
	}
	if (!_error_msg.empty())
	{
		auto _ret_str = w_string_init(pMemPool, pErrorMsg, _error_msg.c_str());
		return _ret || _ret_str;
	}

	return _ret;
}

void w_python::fini()
{
	Py_Finalize();
}

#endif