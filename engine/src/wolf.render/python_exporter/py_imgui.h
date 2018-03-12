/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : py_imgui.h
	Description      : The python exporter for w_imgui class
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_IMGUI_H__
#define __PY_IMGUI_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pyWolf
{
	using namespace boost::python;
	using namespace wolf::graphics;

	BOOST_PYTHON_FUNCTION_OVERLOADS(w_imgui_py_load_overloads, w_imgui::py_load, 5, 7)

	static void py_imgui_export()
	{
		//export w_texture class
		class_<w_imgui, boost::noncopyable>("w_imgui")
			.def("load", &w_imgui::py_load, w_imgui_py_load_overloads())
			.staticmethod("load")
			.def("new_frame", &w_imgui::new_frame, "make new gui frame")
			.staticmethod("new_frame")
			.def("render", &w_imgui::render, "render gui frame")
			.staticmethod("render")
			.def("release", &w_imgui::release, "release all resources")
			.staticmethod("release")
			.def("get_width", &w_imgui::get_width, "get width")
			.staticmethod("get_width")
			.def("get_height", &w_imgui::get_height, "get height")
			.staticmethod("get_height")
			.def("get_command_buffer_at", &w_imgui::get_command_buffer_at, "get command_buffer")
			.staticmethod("get_command_buffer_at")
			.def("set_width", &w_imgui::set_width, "set width")
			.staticmethod("set_width")
			.def("set_height", &w_imgui::set_height, "set height")
			.staticmethod("set_height")
			;
	}
}

#endif//__PY_TEXTURE_H__

#endif//__PYTHON__
