/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : w_texture_py.h
	Description      : The python exporter for w_texture class
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __W_TEXTURE_PY_H__
#define __W_TEXTURE_PY_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pywolf
{
	using namespace wolf::graphics;

	class py_texture : public w_texture
	{
	public:
		py_texture() :
			_graphics_device_index(-1)
		{
		}

		bool py_initialize(
			_In_ const uint32_t& pGDeviceIndex,
			_In_ const uint32_t& pWidth,
			_In_ const uint32_t& pHeight,
			_In_ const bool& pGenerateMipMapsLevels,
			_In_ const w_memory_property_flags pMemoryPropertyFlags)
		{
			if (pGDeviceIndex >= pywolf::py_graphics_devices.size()) return false;
			this->_graphics_device_index = pGDeviceIndex;
			return this->initialize(
				pywolf::py_graphics_devices[pGDeviceIndex],
				pWidth,
				pHeight,
				pGenerateMipMapsLevels,
				pMemoryPropertyFlags) == S_OK;
		}

		bool py_load_texture_from_memory_rgba(_In_ boost::python::list pRGBAData)
		{
			auto _len = len(pRGBAData);
			if (!_len) return false;

			auto _data = new uint8_t[_len];
			for (size_t i = 0; i < _len; ++i)
			{
				_data[i] = boost::python::extract<uint8_t>(pRGBAData[i]);
			}

			auto _hr = this->load_texture_from_memory_rgba(&_data[0]);
			SAFE_DELETE_ARRAY(_data);
			return _hr;
		}

		bool py_load_texture_from_memory_rgb(_In_ boost::python::list  pRGBData)
		{
			auto _len = len(pRGBData);
			if (!_len) return false;

			auto _data = new uint8_t[_len];
			for (size_t i = 0; i < _len; ++i)
			{
				_data[i] = boost::python::extract<uint8_t>(pRGBData[i]);
			}

			auto _hr = this->load_texture_from_memory_rgb(&_data[0]);
			SAFE_DELETE_ARRAY(_data);
			return _hr;
		}

		bool py_copy_data_to_texture_2D(_In_ boost::python::list  pRGBData)
		{
			auto _len = len(pRGBData);
			if (!_len) return false;

			auto _data = new uint8_t[_len];
			for (size_t i = 0; i < _len; ++i)
			{
				_data[i] = boost::python::extract<uint8_t>(pRGBData[i]);
			}

			auto _hr = this->copy_data_to_texture_2D(&_data[0]);
			SAFE_DELETE_ARRAY(_data);
			return _hr;
		}

		boost::python::list py_read_data_of_texture()
		{
			boost::python::list _list;
			uint8_t* _data = (uint8_t*)this->read_data_of_texture();
			auto _w = this->get_width();
			auto _h = this->get_height();

			for (size_t i = 0; i < _w * _h; i += 4)
			{
				_list.append(_data[i]);
				_list.append(_data[i + 1]);
				_list.append(_data[i + 2]);
				_list.append(_data[i + 3]);
			}
			return _list;
		}

		bool py_flush_staging_data()
		{
			return this->flush_staging_data() == S_OK;
		}

		//const VkImageUsageFlags get_usage() const;
		//VkSampler get_sampler(_In_ w_sampler_type pSamplerType = w_sampler_type::NO_MIPMAP_AND_NO_ANISOTROPY) const;
		//w_image_view get_image_view() const;
		//VkImageType get_image_type() const;
		//VkImageViewType get_image_view_type() const;
		//const VkDescriptorImageInfo get_descriptor_info(_In_ w_sampler_type pSamplerType = w_sampler_type::NO_MIPMAP_AND_NO_ANISOTROPY) const;


#pragma region static methods

		static bool py_load_to_shared_textures(
			_In_ size_t pGDeviceIndex,
			_In_z_ std::wstring pPath)
		{
			//py_texture* _texture = nullptr;
			w_texture* _texture = nullptr;
			if (pGDeviceIndex >= pywolf::py_graphics_devices.size()) return false;
			auto _hr = load_to_shared_textures(
				pywolf::py_graphics_devices[pGDeviceIndex],
				pPath,
				&_texture);
			if (_hr != S_OK)
			{
				SAFE_DELETE(_texture);
				return false;
			}
			return true;
		}

		static bool py_save_png_to_file(
			_In_z_ std::string pFilePath,
			_In_ uint32_t pWidth,
			_In_ uint32_t pHeight,
			_In_ boost::python::list pData,
			_In_ int pCompCount,
			_In_ int pStrideInBytes)
		{
			auto _len = len(pData);
			if (pFilePath.empty() || pWidth == 0 || pHeight == 0 || _len == 0) return false;

			auto _data = new uint8_t[_len];
			for (size_t i = 0; i < _len; ++i)
			{
				_data[i] = boost::python::extract<uint8_t>(pData[i]);
			}
			auto _hr = save_png_to_file(pFilePath.c_str(), pWidth, pHeight, &_data[0], pCompCount, pStrideInBytes);
			SAFE_DELETE_ARRAY(_data);
			return _hr == S_OK;
		}

		static bool py_save_bmp_to_file(
			_In_z_ std::string pFilePath,
			_In_ uint32_t pWidth,
			_In_ uint32_t pHeight,
			_In_ boost::python::list pData,
			_In_ int pCompCount)
		{
			auto _len = len(pData);
			if (pFilePath.empty() || pWidth == 0 || pHeight == 0 || _len == 0) return false;

			auto _data = new uint8_t[_len];
			for (size_t i = 0; i < _len; ++i)
			{
				_data[i] = boost::python::extract<uint8_t>(pData[i]);
			}
			auto _hr = save_bmp_to_file(pFilePath.c_str(), pWidth, pHeight, &_data[0], pCompCount);
			SAFE_DELETE_ARRAY(_data);
			return _hr == S_OK;
		}

		static bool py_save_tga_to_file(
			_In_z_ std::string pFilePath,
			_In_ uint32_t pWidth,
			_In_ uint32_t pHeight,
			_In_ boost::python::list pData,
			_In_ int pCompCount)
		{
			auto _len = len(pData);
			if (pFilePath.empty() || pWidth == 0 || pHeight == 0 || _len == 0) return false;

			auto _data = new uint8_t[_len];
			for (size_t i = 0; i < _len; ++i)
			{
				_data[i] = boost::python::extract<uint8_t>(pData[i]);
			}
			auto _hr = save_tga_to_file(pFilePath.c_str(), pWidth, pHeight, &_data[0], pCompCount);
			SAFE_DELETE_ARRAY(_data);
			return _hr == S_OK;
		}

		static bool py_save_hdr_to_file(
			_In_z_ std::string pFilePath,
			_In_ uint32_t pWidth,
			_In_ uint32_t pHeight,
			_In_ boost::python::list pData,
			_In_ int pCompCount)
		{
			auto _len = len(pData);
			if (pFilePath.empty() || pWidth == 0 || pHeight == 0 || _len == 0) return false;

			auto _data = new float[_len];
			for (size_t i = 0; i < _len; ++i)
			{
				_data[i] = boost::python::extract<float>(pData[i]);
			}
			auto _hr = save_hdr_to_file(pFilePath.c_str(), pWidth, pHeight, &_data[0], pCompCount);
			SAFE_DELETE_ARRAY(_data);
			return _hr == S_OK;
		}

		static bool py_save_jpg_to_file(
			_In_z_ std::string pFilePath,
			_In_ uint32_t pWidth,
			_In_ uint32_t pHeight,
			_In_ boost::python::list pData,
			_In_ int pCompCount,
			_In_ int pQuality)
		{
			auto _len = len(pData);
			if (pFilePath.empty() || pWidth == 0 || pHeight == 0 || _len == 0) return false;

			auto _data = new float[_len];
			for (size_t i = 0; i < _len; ++i)
			{
				_data[i] = boost::python::extract<uint8_t>(pData[i]);
			}
			auto _hr = save_jpg_to_file(pFilePath.c_str(), pWidth, pHeight, &_data[0], pCompCount, pQuality);
			SAFE_DELETE_ARRAY(_data);
			return _hr == S_OK;
		}

#pragma endregion

	private:
		short    _graphics_device_index;
	};

	static void w_texture_py_export()
	{
		using namespace boost::python;
		using namespace wolf::graphics;

		//define w_texture_buffer_type enum
		enum_<w_texture_buffer_type>("w_texture_buffer_type")
			.value("W_TEXTURE_COLOR_BUFFER", w_texture_buffer_type::W_TEXTURE_COLOR_BUFFER)
			.value("W_TEXTURE_DEPTH_BUFFER", w_texture_buffer_type::W_TEXTURE_DEPTH_BUFFER)
			.value("W_TEXTURE_STENCIL_BUFFER", w_texture_buffer_type::W_TEXTURE_STENCIL_BUFFER)
			.export_values()
			;

		//define w_texture_view_type enum
		enum_<w_texture_view_type>("w_texture_buffer_type")
			.value("W_TEXTURE_VIEW_TYPE_1D", w_texture_view_type::W_TEXTURE_VIEW_TYPE_1D)
			.value("W_TEXTURE_VIEW_TYPE_2D", w_texture_view_type::W_TEXTURE_VIEW_TYPE_2D)
			.value("W_TEXTURE_VIEW_TYPE_3D", w_texture_view_type::W_TEXTURE_VIEW_TYPE_3D)
			.value("W_TEXTURE_VIEW_TYPE_CUBE", w_texture_view_type::W_TEXTURE_VIEW_TYPE_CUBE)
			.value("W_TEXTURE_VIEW_TYPE_1D_ARRAY", w_texture_view_type::W_TEXTURE_VIEW_TYPE_1D_ARRAY)
			.value("W_TEXTURE_VIEW_TYPE_2D_ARRAY", w_texture_view_type::W_TEXTURE_VIEW_TYPE_2D_ARRAY)
			.value("W_TEXTURE_VIEW_TYPE_CUBE_ARRAY", w_texture_view_type::W_TEXTURE_VIEW_TYPE_CUBE_ARRAY)
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
		class_<py_texture, boost::noncopyable>("w_texture")
			.def("initialize", &py_texture::py_initialize, "initialize texture")
			.def("load", &py_texture::load, "load texture")
			.def("load_texture_2D_from_file", &py_texture::load_texture_2D_from_file, "load texture2D from file")
			.def("load_texture_from_memory_rgba", &py_texture::py_load_texture_from_memory_rgba, "Load texture from memory in the format of RGBA")
			.def("load_texture_from_memory_rgb", &py_texture::py_load_texture_from_memory_rgb, "Load texture from memory in the format of RGB")
			.def("load_texture_from_memory_all_channels_same", &py_texture::load_texture_from_memory_all_channels_same, "Load texture from memory, all channels will have same byte")
			.def("load_texture_from_memory_color", &py_texture::load_texture_from_memory_color, "Load texture from w_color")
			.def("copy_data_to_texture_2D", &py_texture::py_copy_data_to_texture_2D, "copy data to texture, if this is a staging buffer, do not use this function because it will cause memory leaks, instead use \"get_pointer_to_staging_data\" function")
			.def("read_data_of_texture", &py_texture::py_read_data_of_texture, "read texture's data")
			.def("flush_staging_data", &py_texture::py_flush_staging_data, "flush staging buffer")
			.def("release", &py_texture::release, "release all resources")
			.def("get_width", &py_texture::get_width, "get width of image")
			.def("get_height", &py_texture::get_height, "get height of image")
			.def("get_mip_maps_level", &py_texture::get_mip_maps_level, "get number of mip maps levels")
			.def("get_format", &py_texture::get_format, "get image format")
			.def("load_to_shared_textures", &py_texture::py_load_to_shared_textures/*, return_value_policy<manage_new_object>()*/, "load texture and store it into the shared key-value mapped data, the key is the path of texture")
			.staticmethod("load_to_shared_textures")
			.def("save_png_to_file", &py_texture::py_save_png_to_file, "save png image file")
			.staticmethod("save_png_to_file")
			.def("save_bmp_to_file", &py_texture::py_save_bmp_to_file, "save bmp image file")
			.staticmethod("save_bmp_to_file")
			.def("save_tga_to_file", &py_texture::py_save_tga_to_file, "save tga image file")
			.staticmethod("save_tga_to_file")
			.def("save_jpg_to_file", &py_texture::py_save_jpg_to_file, "save jpg image file")
			.staticmethod("save_jpg_to_file")
			;
	}
}

#endif//__W_TEXTURE_PY_H__

#endif//__PYTHON__
