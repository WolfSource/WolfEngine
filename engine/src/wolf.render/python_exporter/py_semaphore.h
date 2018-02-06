/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : py_semaphore.h
	Description      : The python exporter for w_semaphore class
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_SEMAPHORE_H__
#define __PY_SEMAPHORE_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pywolf
{
	class py_semaphore : public wolf::graphics::w_semaphore
	{
	public:
		W_EXP py_semaphore();
		W_EXP bool py_initialize(_In_ boost::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
	};

	static void py_semaphore_export()
	{
		using namespace boost::python;
		using namespace wolf::graphics;

		//export w_semaphore class
		class_<py_semaphore, boost::noncopyable>("w_semaphore")
			.def("initialize", &py_semaphore::py_initialize, "initialize semaphore")
			.def("release", &py_semaphore::release, "release resources of semaphore")
			;

	}
}

#endif//__PY_SEMAPHORE_H__

#endif//__PYTHON__
