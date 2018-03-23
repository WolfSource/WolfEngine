/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : py_cpipeline_scene.h
	Description		 : The python exporter for w_cpipeline_scene classe
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_CPIPELINE_SCENE_H__
#define __PY_CPIPELINE_SCENE_H__

namespace pyWolf
{
	static void py_cpipeline_scene_export()
	{
		using namespace boost::python;
		using namespace wolf::content_pipeline;

		//export w_cpipeline_scene struct
		class_<w_cpipeline_scene>("w_cpipeline_scene", init<>())
			.def("add_model", &w_cpipeline_scene::py_add_model, "add model")
			.def("add_boundary", &w_cpipeline_scene::py_add_boundary, "add boundary")
			.def("add_camera", &w_cpipeline_scene::py_add_camera_1_arg, "add camera")
			.def("add_camera", &w_cpipeline_scene::py_add_camera_3_args, "add camera")
			.def("release", &w_cpipeline_scene::release, "release")
			.def("get_model_by_index", &w_cpipeline_scene::py_get_model_by_index, "get model by index")
			.def("get_models_by_id", &w_cpipeline_scene::py_get_models_by_id, "get model by id")
			/*.def("get_all_models", &w_cpipeline_scene::py_get_all_models, "get all models")
			.def("get_boundaries", &w_cpipeline_scene::py_get_boundaries, "get boundaries")
			.def("get_first_camera", &w_cpipeline_scene::py_get_first_camera, "get first camera")
			.def("get_cameras_by_id", &w_cpipeline_scene::py_get_cameras_by_id, "get cameras by id")
			.def("get_cameras_by_index", &w_cpipeline_scene::py_get_cameras_by_index, "get cameras by index")
			.def("get_z_up", &w_cpipeline_scene::py_get_z_up, "get is zUp")*/
			.def("get_name", &w_cpipeline_scene::get_name, "get name")
			.def("set_name", &w_cpipeline_scene::set_name, "set name")
			.def("set_coordiante_system", &w_cpipeline_scene::set_z_up, "set coordiante system")
			;
	}
}

#endif//__PY_CPIPELINE_SCENE_H__

#endif//__PYTHON__
