#include "w_python_pch.h"
#include "w_python.h"

#ifndef BOOST_PYTHON_SOURCE
#define BOOST_PYTHON_STATIC_LIB
#endif
#include <boost/python.hpp>

using namespace std;
using namespace wolf::system;
using namespace boost::python;

void w_python::initialize()
{
	Py_Initialize();
}

HRESULT w_python::run(std::string pScriptText)
{
	//create main python module
	object main_module(handle<>(borrowed(PyImport_AddModule("__main__"))));
	//create main namespace as dictionary
	boost::python::object _main_namespace = main_module.attr("__dict__");

	auto _hr = S_OK;
	handle<>* _handle_script = nullptr;

	try
	{
		_handle_script = new handle<>((PyRun_String(
			pScriptText.c_str(),
			Py_file_input,
			_main_namespace.ptr(),
			_main_namespace.ptr())));
	}
	catch (error_already_set&)
	{
		_hr = S_FALSE;
		V(_hr, _fetch_error(), this->name, 3);
	}
	if (_handle_script)
	{
		_handle_script->release();
		_handle_script = nullptr;
	}

	return _hr;
}

const std::wstring w_python::_fetch_error()
{
	PyObject* _type;
	PyObject* _value;
	PyObject* _traceback;

	PyErr_Fetch(&_type, &_value, &_traceback);

	handle<> _handle_type(_type);
	object extype(_handle_type);

	if (!_traceback)
	{
		auto _error_msg = L"Unknown";
		this->_log += _error_msg;
		return _error_msg;
	}
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

	auto _error_msg = L"\r\nTraceback(most recent call last) :\r\n"
		"File \"" + _file_name + L"\", line " + std::to_wstring(_line_number) + L", in " + _func_name + L"\r\nNameError : " + _str_error_message;
	this->_log += _error_msg;

	logger.error(_error_msg);

	Py_DECREF(_type);
	Py_DECREF(_value);
	Py_DECREF(_traceback);

	return _error_msg;
}

ULONG w_python::release()
{
	if (_super::get_is_released()) return 0;

	_log.clear();
	Py_Finalize();

	return w_object::release();
}
