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
			.def(init<glm::w_vec3, glm::w_vec3, const w_color&>())
			.def(init<glm::w_vec3, glm::w_vec3, glm::w_vec3, const w_color&>())
			.def(init<glm::w_vec3, const float&, const w_color&, const w_plane&, const uint32_t&>())
			.def(init<const wolf::system::w_bounding_box&, const w_color&>())
			.def(init<const wolf::system::w_bounding_sphere&, const w_color&, const uint32_t&>())
			.def(init<const w_color&>())
			.def("load", &w_shapes::py_load, "load shape")
			.def("update", &w_shapes::py_update, "update shape")
			.def("draw", &w_shapes::draw, "draw shape")
			.def("release", &w_shapes::release, "release resources of shape")
			;
	}
}

#endif//__PY_SHADER_H__

#endif//__PYTHON__
