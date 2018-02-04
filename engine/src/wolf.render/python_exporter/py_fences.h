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

	class py_fences : public w_fences
	{
	public:
		W_EXP py_fences();
		W_EXP bool py_initialize(_In_ const uint32_t& pGDeviceIndex, _In_ const uint32_t pNumberOfFences);
		W_EXP bool py_wait(_In_ uint64_t pTimeOut);
		W_EXP bool py_reset();

	private:
		short    _graphics_device_index;
	};

	static void py_fences_export()
	{
		using namespace boost::python;
		using namespace wolf::graphics;

		//export w_fences class
		class_<py_fences, boost::noncopyable>("py_fences")
			.def("initialize", &py_fences::py_initialize, "initialize fence")
			.def("wait", &py_fences::py_wait, "wait for all fence")
			.def("reset", &py_fences::py_reset, "reset all fences")
			.def("get_count", &py_fences::get_count, "get number of fences")
			.def("release", &py_fences::release, "release resources of all fences")
			;
	}
}

#endif//__PY_FENCES_H__

#endif//__PYTHON__
