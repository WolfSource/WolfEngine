/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : py_cpipeline_model.h
	Description		 : The python exporter for w_cpipeline_model classes
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_CPIPELINE_MODEL_H__
#define __PY_CPIPELINE_MODEL_H__

namespace pyWolf
{
	static void py_cpipeline_model_export()
	{
		using namespace boost::python;
		using namespace wolf::content_pipeline;

		//export w_vertex_struct struct
		class_<w_vertex_struct>("w_vertex_struct", init<>())
			.add_property("position", &w_vertex_struct::py_get_position, &w_vertex_struct::py_set_position, "get or set position")
			.add_property("normal", &w_vertex_struct::py_get_normal, &w_vertex_struct::py_set_normal, "get or set normal")
			.add_property("uv", &w_vertex_struct::py_get_uv, &w_vertex_struct::py_set_uv, "get or set uv")
			.add_property("blend_weight", &w_vertex_struct::py_get_blend_weight, &w_vertex_struct::py_set_blend_weight, "get or set blend_weight")
			.add_property("blend_indices", &w_vertex_struct::py_get_blend_indices, &w_vertex_struct::py_set_blend_indices, "get or set blend_indices")
			.add_property("blend_tangent", &w_vertex_struct::py_get_tangent, &w_vertex_struct::py_set_tangent, "get or set tangent")
			.add_property("blend_binormal", &w_vertex_struct::py_get_binormal, &w_vertex_struct::py_set_binormal, "get or set binormal")
			.add_property("blend_color", &w_vertex_struct::py_get_color, &w_vertex_struct::py_set_color, "get or set color")
			.def_readwrite("vertex_index", &w_vertex_struct::vertex_index, "get or set vertex_index")
			;

		//export w_vertex_index struct
		class_<w_vertex_index>("w_vertex_index", init<>())
			.def_readwrite("vertex_index", &w_vertex_index::vertex_index, "index of vertex")
			.def_readwrite("vertex", &w_vertex_index::vertex, "points of vertex")
			.def_readwrite("uv", &w_vertex_index::uv, "uv of vertex")
			.def_readwrite("normal", &w_vertex_index::normal, "normal of vertex")
			.def("release", &w_vertex_index::release, "release resources of vertex")
			;

		//export w_transform_info struct
		class_<w_transform_info>("w_transform_info", init<>())
			.add_property("position", &w_transform_info::py_get_position, &w_transform_info::py_set_position, "get or set position")
			.add_property("rotation", &w_transform_info::py_get_rotation, &w_transform_info::py_set_rotation, "get or set rotation")
			.add_property("scale", &w_transform_info::py_get_scale, &w_transform_info::py_set_scale, "get or set scale")
			.add_property("transform", &w_transform_info::py_get_transform, &w_transform_info::py_set_transform, "get or set transform")
			;

		//export w_instance_info struct
		class_<w_instance_info>("w_instance_info", init<>())
			.def_readwrite("name", &w_instance_info::name, "get or set name")
			.add_property("position", &w_instance_info::py_get_position, &w_instance_info::py_set_position, "get or set position")
			.add_property("rotation", &w_instance_info::py_get_rotation, &w_instance_info::py_set_rotation, "get or set rotation")
			.add_property("scale", &w_instance_info::py_get_scale, &w_instance_info::py_set_scale, "get or set scale")
			.def_readwrite("texture_sampler_index", &w_instance_info::texture_sampler_index, "get or set texture sampler index")
			;

		//export w_cpipeline_mesh struct
		class_<w_cpipeline_mesh>("w_cpipeline_mesh", init<>())
			.add_property("vertices", &w_cpipeline_mesh::py_get_vertices, &w_cpipeline_mesh::py_set_vertices, "vertices of mesh")
			.def_readwrite("indices", &w_cpipeline_mesh::indices, "indices of mesh")
			.def_readwrite("textures_path", &w_cpipeline_mesh::textures_path, "path of texture")
			.def_readwrite("bounding_box", &w_cpipeline_mesh::bounding_box, "bounding_box of mesh")
			.def("release", &w_cpipeline_mesh::release, "release the resources of mesh")
			;


		//export w_cpipeline_model struct
		class_<w_cpipeline_model>("w_cpipeline_model", init<>())
			.def("add_instance", &w_cpipeline_model::add_instance, "add instance")
			.def("update_world", &w_cpipeline_model::update_world, "update world matrix")
			.def("release", &w_cpipeline_model::release, "release")
			.def("get_name", &w_cpipeline_model::get_name, "get name")
			.def("get_is_all_sub_meshes_use_same_texture", &w_cpipeline_model::get_is_all_sub_meshes_use_same_texture, "get is all sub meshes use same texture")
			.def("get_instance_geometry_name", &w_cpipeline_model::get_instance_geometry_name, "get instance geometry name")
			.def("get_transform", &w_cpipeline_model::get_transform, "get transform")
			.def("get_instances_count", &w_cpipeline_model::get_instances_count, "get instances count")
			.def("get_instances", &w_cpipeline_model::py_get_instances, "get instances")
			.def("get_meshes", &w_cpipeline_model::py_get_meshes, "get meshes")
			.def("get_meshes_count", &w_cpipeline_model::get_meshes_count, "get meshes count")
			.def("get_lods", &w_cpipeline_model::py_get_lods, "get lods")
			.def("get_convex_hulls", &w_cpipeline_model::py_get_convex_hulls, "get convex hulls")
			.def("get_bounding_box", &w_cpipeline_model::py_get_bounding_box, "get bounding box")
			.def("get_lods_count", &w_cpipeline_model::get_lods_count, "get lods count")
			.def("get_convex_hulls_count", &w_cpipeline_model::get_convex_hulls_count, "get convex hulls count")
			.def("set_name", &w_cpipeline_model::set_name, "set name")
			.def("set_instance_geometry_name", &w_cpipeline_model::set_instance_geometry_name, "set instance geometry name")
			.def("set_transform", &w_cpipeline_model::set_transform, "set transform info")
			;
	}
}

#endif//__PY_CPIPELINE_SCENE_H__

#endif//__PYTHON__
