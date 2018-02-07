/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : py_fences.h
	Description      : The python exporter for w_fences class
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_FENCES_H__
#define __PY_FENCES_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pywolf
{
	using namespace wolf::graphics;

	static void py_fences_export()
	{
		using namespace boost::python;
		using namespace wolf::graphics;

		//export w_fences class
		class_<w_fences, boost::noncopyable>("w_fences")
			.def("initialize", &w_fences::py_initialize, "initialize fence")
			.def("wait_for", &w_fences::py_wait_for, "wait for all fence for the timeout period in units of nanoseconds. e.g for arguman \'numpy.iinfo(numpy.uint64).max\'")
			.def("wait", &w_fences::py_wait, "wait for all fence with maximum timeout")
			.def("reset", &w_fences::py_reset, "reset all fences")
			.def("get_count", &w_fences::get_count, "get number of fences")
			.def("release", &w_fences::release, "release resources of all fences")
			;
	}
}

#endif//__PY_FENCES_H__

#endif//__PYTHON__
