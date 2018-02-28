/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : py_uniform.h
	Description      : The python exporter for w_uniform template
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_UNIFORM_H__
#define __PY_UNIFORM_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pyWolf
{
	using namespace boost::python;
	using namespace wolf::graphics;

	class py_uniform
	{
		py_uniform() :
			_host_visible(false)
		{
		}

		~py_uniform()
		{
			//release();
		}

		/*
			Load the uniform buffer
			_In_ pGDevice : Graphics Device
			_In_ pHostVisible : True means host memory of uniform's buffer in DRAM, otherwise host memory in VRAM
		*/
		//W_RESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
		//	_In_ const bool& pHostVisible = false)
		//{
		//	const std::string _trace = this->name + "update";

		//	this->_host_visible = pHostVisible;

		//	//store the shared graphics device pointer
		//	auto _buffer_size = static_cast<uint32_t>(sizeof(T));

		//	this->_gDevice = pGDevice;

		//	//create uniform buffer
		//	W_RESULT _hr = W_PASSED;

		//	if (this->_host_visible)
		//	{
		//		_hr = this->_buffer.load(pGDevice,
		//			_buffer_size,
		//			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		//			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		//		if (_hr == W_FAILED)
		//		{
		//			V(_hr, "loading host visible buffer " +
		//				_gDevice->get_info(),
		//				_trace,
		//				3);
		//			return _hr;
		//		}
		//	}
		//	else
		//	{
		//		_hr = this->_buffer.load(pGDevice,
		//			_buffer_size,
		//			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		//			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		//		if (_hr == W_FAILED)
		//		{
		//			V(_hr, "loading device buffer " +
		//				_gDevice->get_info(),
		//				_trace,
		//				3);
		//			return _hr;
		//		}

		//		//create staging buffer
		//		_hr = this->_staging_buffer.load(pGDevice,
		//			_buffer_size,
		//			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		//			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
		//		if (_hr == W_FAILED)
		//		{
		//			V(_hr, "loading staging buffer " +
		//				_gDevice->get_info(),
		//				_trace,
		//				3);
		//			return _hr;
		//		}

		//		_hr = this->_staging_buffer.bind();
		//		if (_hr == W_FAILED)
		//		{
		//			V(_hr, "binding device buffer " +
		//				_gDevice->get_info(),
		//				_trace,
		//				3);
		//			return _hr;
		//		}
		//	}

		//	_hr = this->_buffer.bind();
		//	if (_hr == W_FAILED)
		//	{
		//		V(_hr, "binding buffer " +
		//			_gDevice->get_info(),
		//			_trace,
		//			3);
		//		return _hr;
		//	}

		//	return _hr;
		//}

		//W_RESULT update()
		//{
		//	const std::string _trace = this->name + "update";

		//	W_RESULT _hr = W_PASSED;

		//	if (this->_host_visible)
		//	{
		//		_hr = this->_buffer.set_data(&this->data);
		//		V(_hr, "setting to host visible buffer " +
		//			_gDevice->get_info(),
		//			_trace,
		//			3);
		//	}
		//	else
		//	{
		//		auto _size = this->_staging_buffer.get_size();
		//		auto _mapped = this->_staging_buffer.map();
		//		if (_mapped)
		//		{
		//			memcpy(_mapped, &this->data, _size);
		//			this->_staging_buffer.flush();
		//		}
		//		else
		//		{
		//			_hr = W_FAILED;
		//			V(_hr, "begining command buffer " +
		//				_gDevice->get_info(),
		//				_trace,
		//				3);
		//		}
		//		this->_staging_buffer.unmap();

		//		if (_hr == W_FAILED) return _hr;

		//		_hr = this->_staging_buffer.copy_to(this->_buffer);
		//		V(_hr, "copy staging buffer to device buffer " +
		//			_gDevice->get_info(),
		//			_trace,
		//			3);
		//	}

		//	return _hr;
		//}

		//const w_descriptor_buffer_info get_descriptor_info() const
		//{
		//	return this->_buffer.get_descriptor_info();
		//}

		////Release resources
		//ULONG release()
		//{
		//	if (this->get_is_released()) return 1;

		//	this->_buffer.release();
		//	this->_staging_buffer.release();
		//	this->_gDevice = nullptr;

		//	return _super::release();
		//}

		//uint32_t get_size()
		//{
		//	return this->_buffer.get_size();
		//}

		//VkBuffer get_handle()
		//{
		//	return this->_buffer.get_handle();
		//}

	private:
		/*std::shared_ptr<w_graphics_device>   _gDevice;
		w_buffer                             _buffer;
		w_buffer                             _staging_buffer;*/
		bool                                 _host_visible;
	};

	static void py_uniform_export()
	{
		//export py_uniform class
		//class_<w_texture, boost::noncopyable>("w_texture")
		//	.def("initialize", &w_texture::py_initialize_simple, "initialize texture")
		//	.def("initialize", &w_texture::py_initialize_advanced, "initialize texture")
		//	.def("load", &w_texture::load, "load texture")
		//	.def("load_texture_2D_from_file", &w_texture::load_texture_2D_from_file, "load texture2D from file")
		//	.def("load_texture_from_memory_rgba", &w_texture::py_load_texture_from_memory_rgba, "Load texture from memory in the format of RGBA")
		//	.def("load_texture_from_memory_rgb", &w_texture::py_load_texture_from_memory_rgb, "Load texture from memory in the format of RGB")
		//	.def("load_texture_from_memory_all_channels_same", &w_texture::load_texture_from_memory_all_channels_same, "Load texture from memory, all channels will have same byte")
		//	.def("load_texture_from_memory_color", &w_texture::load_texture_from_memory_color, "Load texture from w_color")
		//	.def("copy_data_to_texture_2D", &w_texture::py_copy_data_to_texture_2D, "copy data to texture, if this is a staging buffer, do not use this function because it will cause memory leaks, instead use \"get_pointer_to_staging_data\" function")
		//	.def("read_data_of_texture", &w_texture::py_read_data_of_texture, "read texture's data")
		//	.def("flush_staging_data", &w_texture::py_flush_staging_data, "flush staging buffer")
		//	.def("release", &w_texture::release, "release all resources")
		//	.def("get_width", &w_texture::get_width, "get width of image")
		//	.def("get_height", &w_texture::get_height, "get height of image")
		//	.def("get_usage_flags", &w_texture::get_usage_flags, "get usage flags")
		//	.def("get_sampler", &w_texture::py_get_sampler, "get sampler")
		//	.def("get_image_view", &w_texture::get_image_view, "get image view")
		//	.def("get_image_type", &w_texture::get_image_type, "get image type")
		//	.def("get_image_type", &w_texture::get_image_view_type, "get image view type")
		//	.def("get_format", &w_texture::get_format, "get image format")
		//	.def("get_descriptor_info", &w_texture::py_get_descriptor_info, "get descriptor_info")
		//	.def("get_mip_maps_level", &w_texture::get_mip_maps_level, "get number of mip maps levels")
		//	.def("set_format", &w_texture::set_format, "set format")
		//	.def("set_usage_flags", &w_texture::set_usage_flags, "set usage flags")
		//	.def("set_buffer_type", &w_texture::set_buffer_type, "set buffer type")
		//	.def("set_view_type", &w_texture::set_view_type, "set view type")
		//	.def("load_to_shared_textures", &w_texture::py_load_to_shared_textures, "load texture and store it into the shared key-value mapped data, the key is the path of texture")
		//	.staticmethod("load_to_shared_textures")
		//	.def("release_shared_textures", &w_texture::release_shared_textures, "release all shared textures")
		//	.staticmethod("release_shared_textures")
		//	.def("save_png_to_file", &w_texture::py_save_png_to_file, "save png image file")
		//	.staticmethod("save_png_to_file")
		//	.def("save_bmp_to_file", &w_texture::py_save_bmp_to_file, "save bmp image file")
		//	.staticmethod("save_bmp_to_file")
		//	.def("save_tga_to_file", &w_texture::py_save_tga_to_file, "save tga image file")
		//	.staticmethod("save_tga_to_file")
		//	.def("save_jpg_to_file", &w_texture::py_save_jpg_to_file, "save jpg image file")
		//	.staticmethod("save_jpg_to_file")
		//	;
	}
}

#endif//__PY_UNIFORM_H__

#endif//__PYTHON__
