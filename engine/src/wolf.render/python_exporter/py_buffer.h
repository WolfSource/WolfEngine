/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : py_buffer.h
	Description      : The python exporter for w_buffer class
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_BUFFER_H__
#define __PY_BUFFER_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pyWolf
{
	static void py_buffer_export()
	{
		using namespace boost::python;
		using namespace wolf::graphics;

		//export w_buffer_handle struct
		class_<w_buffer_handle, boost::noncopyable>("w_buffer_handle", init<>());

		//export w_buffer class
		class_<w_buffer, boost::noncopyable>("w_buffer")
			.def("release", &w_buffer::release, "release resources of buffer")
			.def("get_size", &w_buffer::get_size, "get size of buffer")
			//.def("get_usage_flags", &w_buffer::get_usage_flags, "get buffer usage")
			//.def("get_memory_flags", &w_buffer::get_memory_flags, "get memory flags")
			//.def("get_handle", &w_buffer::get_handle, "get handle of buffer")
			//.def("get_memory", &w_buffer::get_memory, "get memory")
			//.def("get_descriptor_info", &w_buffer::get_descriptor_info, "get descriptor info")
			.def("set_data", &w_buffer::py_set_data, "set data")
			;
	}
}

#endif//__PY_BUFFER_H__

#endif//__PYTHON__
