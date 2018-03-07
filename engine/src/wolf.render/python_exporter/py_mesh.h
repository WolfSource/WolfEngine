/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : py_mesh.h
	Description      : The python exporter for w_mesh class
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_MESH_H__
#define __PY_MESH_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pyWolf
{
	using namespace boost::python;
	using namespace wolf::graphics;

	BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(w_mesh_py_load_overloads, w_mesh::py_load, 3, 4)
	BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(w_mesh_py_draw_overloads, w_mesh::py_draw, 4, 5)
	
	static void py_mesh_export()
	{
		//define w_vertex_declaration enum
		enum_<w_vertex_declaration>("w_vertex_declaration")
			.value("NOT_DEFINED", w_vertex_declaration::NOT_DEFINED)
			.value("USER_DEFINED", w_vertex_declaration::USER_DEFINED)
			.value("VERTEX_POSITION", w_vertex_declaration::VERTEX_POSITION)
			.value("VERTEX_POSITION_COLOR", w_vertex_declaration::VERTEX_POSITION_COLOR)
			.value("VERTEX_POSITION_UV", w_vertex_declaration::VERTEX_POSITION_UV)
			.value("VERTEX_POSITION_UV_INDEX", w_vertex_declaration::VERTEX_POSITION_UV_INDEX)
			.value("VERTEX_POSITION_UV_COLOR", w_vertex_declaration::VERTEX_POSITION_UV_COLOR)
			.value("VERTEX_POSITION_UV_INDEX_COLOR", w_vertex_declaration::VERTEX_POSITION_UV_INDEX_COLOR)
			.value("VERTEX_POSITION_NORMAL_COLOR", w_vertex_declaration::VERTEX_POSITION_NORMAL_COLOR)
			.value("VERTEX_POSITION_NORMAL_UV", w_vertex_declaration::VERTEX_POSITION_NORMAL_UV)
			.value("VERTEX_POSITION_NORMAL_UV_INDEX", w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX)
			.value("VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL", w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL)
			.value("VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL", w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL)
			.value("VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL_BLEND_WEIGHT_BLEND_INDICES", w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL_BLEND_WEIGHT_BLEND_INDICES)
			.value("VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL_BLEND_WEIGHT_BLEND_INDICES", w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL_BLEND_WEIGHT_BLEND_INDICES)
			.export_values()
			;

		//define w_vertex_attribute enum
		enum_<w_vertex_attribute>("w_vertex_attribute")
			.value("Float", w_vertex_attribute::Float)
			.value("Vec2", w_vertex_attribute::Vec2)
			.value("Vec3", w_vertex_attribute::Vec3)
			.value("Vec4", w_vertex_attribute::Vec4)
			.export_values()
			;

		//export w_vertex_binding_attributes struct
		class_<w_vertex_binding_attributes, boost::noncopyable>("w_vertex_binding_attributes")
			.def(init<const w_vertex_declaration&>())
			.def(init<boost::python::dict>())
			.def_readwrite("declaration", &w_vertex_binding_attributes::declaration, "declaration")
			.add_property("binding_attributes", &w_vertex_binding_attributes::py_get_binding_attributes, &w_vertex_binding_attributes::py_set_binding_attributes, "binding attributes")
			;

		//export w_mesh struct
		class_<w_mesh, boost::noncopyable>("w_mesh")
			.def("load", &w_mesh::py_load, w_mesh_py_load_overloads())
			.def("draw", &w_mesh::py_draw, w_mesh_py_draw_overloads())
			.def("release", &w_mesh::release, "release")
			.def("get_vertices_count", &w_mesh::get_vertices_count, "get vertices count")
			.def("get_indices_count", &w_mesh::get_vertices_count, "get indices count")
			.def("get_texture", &w_mesh::get_texture, return_value_policy<manage_new_object>())
			.def("get_vertex_binding_attributes", &w_mesh::get_vertex_binding_attributes, "get vertex binding attributes")
			.def("set_vertex_binding_attributes", &w_mesh::set_vertex_binding_attributes, "set vertex binding attributes")
			.def("set_texture", &w_mesh::py_set_texture, "set texture")
			;
	}
}

#endif//__PY_MESH_H__

#endif//__PYTHON__
