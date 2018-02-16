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

namespace pywolf
{
	static void py_mesh_export()
	{
		using namespace boost::python;
		using namespace wolf::graphics;

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
			.def_readwrite("declaration", &w_vertex_binding_attributes::declaration, "declaration")
			.add_property("binding_attributes", &w_vertex_binding_attributes::py_get_binding_attributes, &w_vertex_binding_attributes::py_set_binding_attributes, "binding attributes")
			;
	}
}

#endif//__PY_MESH_H__

#endif//__PYTHON__
