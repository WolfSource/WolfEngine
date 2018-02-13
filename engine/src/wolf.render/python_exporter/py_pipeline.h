/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : py_pipeline.h
	Description      : The python exporter for w_pipeline class
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_PIPELINE_H__
#define __PY_PIPELINE_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pywolf
{
	using namespace wolf::graphics;

	static void py_pipeline_export()
	{
		using namespace boost::python;
		using namespace wolf::graphics;

		//export w_pipeline class
		class_<w_pipeline, boost::noncopyable>("w_pipeline")
			//.def("load", &w_pipeline::py_load, "load pipeline")
			//.def("load_compute", &w_pipeline::load_compute, "load compute pipeline")
			//.def("bind", &w_pipeline::bind, "bind to pipeline")
			//.def("release", &w_pipeline::release, "get shader stages")
			//.def("create_pipeline_cache", w_pipeline::create_pipeline_cache, "create pipeline cache")
			//.staticmethod("create_pipeline_cache")
			//.def("release_all_pipeline_caches", w_pipeline::release_all_pipeline_caches, "release all pipeline caches")
			//.staticmethod("release_all_pipeline_caches")
			;
	}
}

#endif//__PY_SHADER_H__

#endif//__PYTHON__
