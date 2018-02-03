/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : w_fences_py.h
	Description      : The python exporter for w_fences class
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __W_FENCES_PY_H__
#define __W_FENCES_PY_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pywolf
{
	using namespace wolf::graphics;

	class py_fences : public w_fences
	{
	public:
		py_fences() :
			_graphics_device_index(-1)
		{
		}

		bool py_initialize(_In_ const uint32_t& pGDeviceIndex, _In_ const uint32_t pNumberOfFences)
		{
			if (pGDeviceIndex >= pywolf::py_graphics_devices.size()) return false;
			this->_graphics_device_index = pGDeviceIndex;
			//return this->initialize(pywolf::py_graphics_devices[pGDeviceIndex], pNumberOfFences) == S_OK;
			return true;
		}

	private:
		short    _graphics_device_index;
	};

	static void w_fences_py_export()
	{
		using namespace boost::python;
		using namespace wolf::graphics;

		//export w_fences class
		class_<py_fences, boost::noncopyable>("py_fences")
			.def("initialize", &py_fences::py_initialize, "initialize fence")
			//.def("load", &py_texture::load, "load texture")
			//.def("load_texture_2D_from_file", &py_texture::load_texture_2D_from_file, "load texture2D from file")
			//.def("load_texture_from_memory_rgba", &py_texture::py_load_texture_from_memory_rgba, "Load texture from memory in the format of RGBA")
			//.def("load_texture_from_memory_rgb", &py_texture::py_load_texture_from_memory_rgb, "Load texture from memory in the format of RGB")
			//.def("load_texture_from_memory_all_channels_same", &py_texture::load_texture_from_memory_all_channels_same, "Load texture from memory, all channels will have same byte")
			//.def("load_texture_from_memory_color", &py_texture::load_texture_from_memory_color, "Load texture from w_color")
			//.def("copy_data_to_texture_2D", &py_texture::py_copy_data_to_texture_2D, "copy data to texture, if this is a staging buffer, do not use this function because it will cause memory leaks, instead use \"get_pointer_to_staging_data\" function")
			//.def("read_data_of_texture", &py_texture::py_read_data_of_texture, "read texture's data")
			//.def("flush_staging_data", &py_texture::py_flush_staging_data, "flush staging buffer")
			//.def("release", &py_texture::release, "release all resources")
			//.def("get_width", &py_texture::get_width, "get width of image")
			//.def("get_height", &py_texture::get_height, "get height of image")
			//.def("get_mip_maps_level", &py_texture::get_mip_maps_level, "get number of mip maps levels")
			//.def("get_format", &py_texture::get_format, "get image format")
			//.def("load_to_shared_textures", &py_texture::py_load_to_shared_textures/*, return_value_policy<manage_new_object>()*/, "load texture and store it into the shared key-value mapped data, the key is the path of texture")
			//.staticmethod("load_to_shared_textures")
			//.def("save_png_to_file", &py_texture::py_save_png_to_file, "save png image file")
			//.staticmethod("save_png_to_file")
			//.def("save_bmp_to_file", &py_texture::py_save_bmp_to_file, "save bmp image file")
			//.staticmethod("save_bmp_to_file")
			//.def("save_tga_to_file", &py_texture::py_save_tga_to_file, "save tga image file")
			//.staticmethod("save_tga_to_file")
			//.def("save_jpg_to_file", &py_texture::py_save_jpg_to_file, "save jpg image file")
			//.staticmethod("save_jpg_to_file")
			;
	}
}

#endif//__W_FENCES_PY_H__

#endif//__PYTHON__
