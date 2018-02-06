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

namespace pywolf
{
	using namespace wolf::graphics;

	class py_render_pass : public w_render_pass
	{
	public:
		py_render_pass() :
			_graphics_device_index(-1)
		{
		}

		//bool py_load(
		//	_In_ const uint32_t& pGDeviceIndex,
		//	_In_ const w_viewport& pViewPort,
		//	_In_ const w_viewport_scissor& pViewPortScissor,
		//	_In_ const std::vector<w_attachment_buffer_desc>& pAttachmentsBuffersDescriptions)
		//{

		//}

        bool py_load()
        {
            return true;
        }
        
		bool py_begin()
		{
            return true;
		}

		bool py_end()
		{
            return true;
		}

	private:
		short    _graphics_device_index;
	};

	static void w_render_pass_py_export()
	{
		using namespace boost::python;

		////define w_texture_buffer_type enum
		//enum_<w_texture_buffer_type>("w_texture_buffer_type")
		//    .value("W_TEXTURE_COLOR_BUFFER", w_texture_buffer_type::W_TEXTURE_COLOR_BUFFER)
		//    .value("W_TEXTURE_DEPTH_BUFFER", w_texture_buffer_type::W_TEXTURE_DEPTH_BUFFER)
		//    .value("W_TEXTURE_STENCIL_BUFFER", w_texture_buffer_type::W_TEXTURE_STENCIL_BUFFER)
		//    .export_values()
		//    ;

		////define w_texture_view_type enum
		//enum_<w_texture_view_type>("w_texture_buffer_type")
		//    .value("W_TEXTURE_VIEW_TYPE_1D", w_texture_view_type::W_TEXTURE_VIEW_TYPE_1D)
		//    .value("W_TEXTURE_VIEW_TYPE_2D", w_texture_view_type::W_TEXTURE_VIEW_TYPE_2D)
		//    .value("W_TEXTURE_VIEW_TYPE_3D", w_texture_view_type::W_TEXTURE_VIEW_TYPE_3D)
		//    .value("W_TEXTURE_VIEW_TYPE_CUBE", w_texture_view_type::W_TEXTURE_VIEW_TYPE_CUBE)
		//    .value("W_TEXTURE_VIEW_TYPE_1D_ARRAY", w_texture_view_type::W_TEXTURE_VIEW_TYPE_1D_ARRAY)
		//    .value("W_TEXTURE_VIEW_TYPE_2D_ARRAY", w_texture_view_type::W_TEXTURE_VIEW_TYPE_2D_ARRAY)
		//    .value("W_TEXTURE_VIEW_TYPE_CUBE_ARRAY", w_texture_view_type::W_TEXTURE_VIEW_TYPE_CUBE_ARRAY)
		//    .export_values()
		//    ;

		////define w_sampler_type enum
		//enum_<w_sampler_type>("w_sampler_type")
		//    .value("NO_MIPMAP_AND_NO_ANISOTROPY", w_sampler_type::NO_MIPMAP_AND_NO_ANISOTROPY)
		//    .value("NO_MIPMAP_AND_ANISOTROPY", w_sampler_type::NO_MIPMAP_AND_ANISOTROPY)
		//    .value("MIPMAP_AND_NO_ANISOTROPY", w_sampler_type::MIPMAP_AND_NO_ANISOTROPY)
		//    .value("MIPMAP_AND_ANISOTROPY", w_sampler_type::MIPMAP_AND_ANISOTROPY)
		//    .export_values()
		//    ;

		//export w_texture class
		class_<py_render_pass, boost::noncopyable>("w_render_pass")
			.def("load", &py_render_pass::py_load, "load render pass")
			.def("release", &py_render_pass::release, "release")
			;
	}
}

#endif//__PY_RENDER_PASS_H__

#endif//__PYTHON__

