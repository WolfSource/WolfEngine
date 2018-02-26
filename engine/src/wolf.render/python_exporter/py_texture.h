/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : py_texture.h
	Description      : The python exporter for w_texture class
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_TEXTURE_H__
#define __PY_TEXTURE_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pywolf
{
	static void py_texture_export()
	{
		using namespace boost::python;
		using namespace wolf::graphics;

		//define w_texture_buffer_type enum
		enum_<w_texture_buffer_type>("w_texture_buffer_type")
			.value("COLOR", w_texture_buffer_type::COLOR)
			.value("DEPTH", w_texture_buffer_type::DEPTH)
			.value("STENCIL", w_texture_buffer_type::STENCIL)
			.export_values()
			;

		//define w_sampler_type enum
		enum_<w_sampler_type>("w_sampler_type")
			.value("NO_MIPMAP_AND_NO_ANISOTROPY", w_sampler_type::NO_MIPMAP_AND_NO_ANISOTROPY)
			.value("NO_MIPMAP_AND_ANISOTROPY", w_sampler_type::NO_MIPMAP_AND_ANISOTROPY)
			.value("MIPMAP_AND_NO_ANISOTROPY", w_sampler_type::MIPMAP_AND_NO_ANISOTROPY)
			.value("MIPMAP_AND_ANISOTROPY", w_sampler_type::MIPMAP_AND_ANISOTROPY)
			.export_values()
			;

		//export w_texture class
		class_<w_texture, boost::noncopyable>("w_texture")
			.def("initialize", &w_texture::py_initialize_simple, "initialize texture")
			.def("initialize", &w_texture::py_initialize_advanced, "initialize texture")
			.def("load", &w_texture::load, "load texture")
			.def("load_texture_2D_from_file", &w_texture::load_texture_2D_from_file, "load texture2D from file")
			.def("load_texture_from_memory_rgba", &w_texture::py_load_texture_from_memory_rgba, "Load texture from memory in the format of RGBA")
			.def("load_texture_from_memory_rgb", &w_texture::py_load_texture_from_memory_rgb, "Load texture from memory in the format of RGB")
			.def("load_texture_from_memory_all_channels_same", &w_texture::load_texture_from_memory_all_channels_same, "Load texture from memory, all channels will have same byte")
			.def("load_texture_from_memory_color", &w_texture::load_texture_from_memory_color, "Load texture from w_color")
			.def("copy_data_to_texture_2D", &w_texture::py_copy_data_to_texture_2D, "copy data to texture, if this is a staging buffer, do not use this function because it will cause memory leaks, instead use \"get_pointer_to_staging_data\" function")
			.def("read_data_of_texture", &w_texture::py_read_data_of_texture, "read texture's data")
			.def("flush_staging_data", &w_texture::py_flush_staging_data, "flush staging buffer")
			.def("release", &w_texture::release, "release all resources")
			.def("get_width", &w_texture::get_width, "get width of image")
			.def("get_height", &w_texture::get_height, "get height of image")
			.def("get_usage_flags", &w_texture::get_usage_flags, "get usage flags")
			.def("get_sampler", &w_texture::py_get_sampler, "get sampler")
			.def("get_image_view", &w_texture::get_image_view, "get image view")
			.def("get_image_type", &w_texture::get_image_type, "get image type")
			.def("get_image_type", &w_texture::get_image_view_type, "get image view type")
			.def("get_format", &w_texture::get_format, "get image format")
			.def("get_descriptor_info", &w_texture::py_get_descriptor_info, "get descriptor_info")
			.def("get_mip_maps_level", &w_texture::get_mip_maps_level, "get number of mip maps levels")
			.def("set_format", &w_texture::set_format, "set format")
			.def("set_usage_flags", &w_texture::set_usage_flags, "set usage flags")
			.def("set_buffer_type", &w_texture::set_buffer_type, "set buffer type")
			.def("set_view_type", &w_texture::set_view_type, "set view type")
			.def("load_to_shared_textures", &w_texture::py_load_to_shared_textures, "load texture and store it into the shared key-value mapped data, the key is the path of texture")
			.staticmethod("load_to_shared_textures")
			.def("release_shared_textures", &w_texture::release_shared_textures, "release all shared textures")
			.staticmethod("release_shared_textures")
			.def("save_png_to_file", &w_texture::py_save_png_to_file, "save png image file")
			.staticmethod("save_png_to_file")
			.def("save_bmp_to_file", &w_texture::py_save_bmp_to_file, "save bmp image file")
			.staticmethod("save_bmp_to_file")
			.def("save_tga_to_file", &w_texture::py_save_tga_to_file, "save tga image file")
			.staticmethod("save_tga_to_file")
			.def("save_jpg_to_file", &w_texture::py_save_jpg_to_file, "save jpg image file")
			.staticmethod("save_jpg_to_file")
			;
	}
}

#endif//__PY_TEXTURE_H__

#endif//__PYTHON__
