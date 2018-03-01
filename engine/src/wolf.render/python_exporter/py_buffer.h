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

		//export w_queue class
		//class_<w_buffer, boost::noncopyable>("w_buffer")
		//	.add_property("index", &w_queue::index, "queue index")
		//	.def("release", &w_queue::release, "release resources of semaphore")
		//	;
	}
}

#endif//__PY_BUFFER_H__

#endif//__PYTHON__
