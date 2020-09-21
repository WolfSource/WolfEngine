/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : py_render_target.h
    Description      : The python exporter for w_render_target class
    Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_RENDER_TARGET_H__
#define __PY_RENDER_TARGET_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pyWolf
{
	using namespace wolf::graphics;

	BOOST_PYTHON_FUNCTION_OVERLOADS(render_target_record_overloads, w_render_target::py_record_command_buffer, 3, 6)

	static void py_render_target_export()
	{
		using namespace boost::python;

		//export w_render_target class
		class_<w_render_target, boost::noncopyable>("w_render_target")
			.def("load", &w_render_target::py_load, "load render pass which contains frame buffers")
			.def("record_command_buffer", &w_render_target::py_record_command_buffer, render_target_record_overloads())
			.def("release", &w_render_target::release, "release render target")
			.def("get_width", &w_render_target::get_width, "get width of render target")
			.def("get_height", &w_render_target::get_height, "get height of render target")
			.def("get_sampler", &w_render_target::get_sampler, "get sampler of render target")
			.def("get_image_view", &w_render_target::get_image_view, "get image view of render target")
			.def("get_image_type", &w_render_target::get_image_type, "get image type of render target")
			.def("get_image_view_type", &w_render_target::get_image_view_type, "get image viee type of render target")
			.def("get_attachment_format", &w_render_target::get_attachment_format, "get attachment format of render target")
			.def("get_attachment_descriptor_info", &w_render_target::get_attachment_descriptor_info, "get attachment descriptor info of render target")
			;
	}
}

#endif//__PY_RENDER_TARGET_H__

#endif//__PYTHON__

