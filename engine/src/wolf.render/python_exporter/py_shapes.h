/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : py_shapes.h
	Description      : The python exporter for w_shapes class
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_SHAPES_H__
#define __PY_SHAPES_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pyWolf
{
	using namespace boost::python;
	using namespace wolf::graphics;

	static void py_shapes_export()
	{
		//export w_shapes class
		class_<w_shapes, boost::noncopyable>("w_shapes")
			.def(init<boost::python::list, boost::python::list, const w_color&>())
			.def(init<boost::python::list, boost::python::list, boost::python::list, const w_color&>())
			.def(init<boost::python::list, const float&, const w_color&, const w_plane&, const uint32_t&>())

			//.def("load", &w_shader::py_load, w_shader_load_overloads())
			//.def("release", &w_shader::release, "release all resources")
			//.def("get_shader_binding_params", &w_shader::py_get_shader_binding_params, "get shader binding params")
			//.def("get_shader_stages", &w_shader::py_get_shader_stages, "get shader stages")
			//.def("get_compute_shader_stage", &w_shader::get_compute_shader_stage, "get compute shader stage")
			//.def("set_shader_binding_params", &w_shader::py_set_shader_binding_params, "set and update shader binding params")
			//.def("load_shader", w_shader::py_load_shader, w_shader_load_shader_overloads())
			//.staticmethod("load_shader")
			;
	}
}

#endif//__PY_SHADER_H__

#endif//__PYTHON__
