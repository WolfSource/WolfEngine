/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : py_frame_buffer.h
	Description      : The python exporter for w_frame_buffer class
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_FRAME_BUFFER_H__
#define __PY_FRAME_BUFFER_H__

#include <python_exporter/w_boost_python_helper.h>
#include "w_graphics/w_render_pass.h"

namespace pywolf
{
	//using namespace wolf::graphics;

	//class py_frame_buffer : public w_frame_buffer
	//{
	//public:
	//	py_frame_buffer() :
	//		_graphics_device_index(-1)
	//	{
	//	}

	//	bool py_load(_In_ const uint32_t& pGDeviceIndex, _In_ const py_render_pass& pRenderPass)
	//	{
	//		return true;
	//	}

	//private:
	//	short    _graphics_device_index;
	//};

	//static void py_frame_buffer_export()
	//{
	//	using namespace boost::python;
	//	//export w_frame_buffer class
	//	class_<py_frame_buffer, boost::noncopyable>("w_frame_buffer")
	//		.def("load", &py_frame_buffer::py_load, "load frame buffer")
	//		.def("release", &py_frame_buffer::release, "release resources of semaphore")
	//		;
	//}
}

#endif//__PY_SEMAPHORE_H__

#endif//__PYTHON__
