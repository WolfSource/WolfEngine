/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : py_render_pass.h
    Description      : The python exporter for w_render_pass class
    Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_RENDER_PASS_H__
#define __PY_RENDER_PASS_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pyWolf
{
	using namespace wolf::graphics;

	BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(render_pass_begin_overloads, w_render_pass::begin, 3, 5)

	static void py_render_pass_export()
	{
		using namespace boost::python;

		//export w_render_pass class
		class_<w_render_pass, boost::noncopyable>("w_render_pass")
			.def("load", &w_render_pass::py_load, "load render pass which contains frame buffers")
			.def("begin", &w_render_pass::begin, render_pass_begin_overloads())
			.def("end", &w_render_pass::end, "end render pass")
			.def("release", &w_render_pass::release, "release")
			.def("viewport", &w_render_pass::get_viewport, "get viewport")
			.def("viewport_scissor", &w_render_pass::get_viewport_scissor, "get viewport scissor")
			.def("get_number_of_frame_buffers", &w_render_pass::get_number_of_frame_buffers, "get number of frame buffers")
			.def("set_viewport", &w_render_pass::set_viewport, "set viewport")
			.def("set_viewport_scissor", &w_render_pass::set_viewport_scissor, "set viewport scissor")
			;
	}
}

#endif//__PY_RENDER_PASS_H__

#endif//__PYTHON__

