/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_texture.h
	Description		 : create and handle 2D texture resource
	Comment          : Wolf used encoding texture with two channel based on http://devkk.net/index.php?tag=articles&id=24
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_TEXTURE_2D_H__
#define __W_TEXTURE_2D_H__

#include "w_graphics_device_manager.h"
#include <w_logger.h>
#include <w_io.h>

namespace wolf
{
	namespace graphics
	{
		enum w_texture_buffer_type
		{
			COLOR = 1,
			DEPTH = 2,
			STENCIL = 4,
		};
			
		typedef enum w_sampler_type
		{
			NO_MIPMAP_AND_NO_ANISOTROPY = 0,
			NO_MIPMAP_AND_ANISOTROPY,
			MIPMAP_AND_NO_ANISOTROPY,
			MIPMAP_AND_ANISOTROPY
		} w_sampler_type;

        class w_texture_pimp;
		class w_texture : public system::w_object
		{
		public:
			W_EXP w_texture();
			W_EXP virtual ~w_texture();

            W_EXP W_RESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const uint32_t& pWidth = 32,
                _In_ const uint32_t& pHeight = 32,
				_In_ const bool& pGenerateMipMapsLevels = false,
				_In_ const bool& pIsStaging = false);
            
            W_EXP W_RESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const uint32_t& pWidth,
                _In_ const uint32_t& pHeight,
				_In_ const bool& pGenerateMipMapsLevels,
				_In_ const w_memory_property_flags pMemoryPropertyFlags);
            
			//Load texture
			W_EXP W_RESULT load();

			//Load texture from file
			W_EXP W_RESULT load_texture_2D_from_file(_In_ std::wstring pPath, _In_ bool pIsAbsolutePath = false);
            //Load texture from memory in the format of RGBA
            W_EXP W_RESULT load_texture_from_memory_rgba(_In_ uint8_t* pRGBAData);
            //Load texture from memory in the format of RGB
            W_EXP W_RESULT load_texture_from_memory_rgb(_In_ uint8_t* pRGBData);
            //Load texture from memory, all channels will have same byte
            W_EXP W_RESULT load_texture_from_memory_all_channels_same(_In_ uint8_t pData);
            //Load texture from w_color
            W_EXP W_RESULT load_texture_from_memory_color(_In_ w_color pColor);
            /*
                copy data to texture
                if this is a staging buffer, do not use this function because it will cause memory leaks,
                instead use "get_pointer_to_staging_data" function
            */
            W_EXP W_RESULT copy_data_to_texture_2D(_In_ const uint8_t* pRGBA);
			
			//read texture's data
			W_EXP void* read_data_of_texture();

            //flush staging buffer
            W_EXP W_RESULT flush_staging_data();

            //release all resources
            W_EXP virtual ULONG release() override;

            //load texture and store it into the shared
            W_EXP static W_RESULT load_to_shared_textures(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_z_ std::wstring pPath,
                _Inout_ w_texture** pPointerToTexture);
        
			/*
				save png image file
				@param pFilePath, path of file
				@param pSize, size of file (width, height)
				@param pData, pointer to rgba data
				@param pCompCount, number of channels(RGBA = 4, RGB = 3)
				@param pStrideInBytes, stride of pixel's structure inf bytes
			*/
			W_EXP static W_RESULT save_png_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount, _In_ int pStrideInBytes = 4 * sizeof(uint8_t));
			/*
				save bmp image file
				@param pFilePath, path of file
				@param pSize, size of file (width, height)
				@param pData, pointer to rgba data
				@param pCompCount, number of channels(RGBA = 4, RGB = 3)
			*/
			W_EXP static W_RESULT save_bmp_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData,_In_ int pCompCount);
			/*
				save tga image file
				@param pFilePath, path of file
				@param pSize, size of file (width, height)
				@param pData, pointer to rgba data
				@param pCompCount, number of channels(RGBA = 4, RGB = 3)
				@param pQuality, Quality(1 - 100)
			*/
			W_EXP static W_RESULT save_tga_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData,_In_ int pCompCount);
			/*
				save hdr image file
				@param pFilePath, path of file
				@param pSize, size of file (width, height)
				@param pData, pointer to rgba data
				@param pCompCount, number of channels(RGBA = 4, RGB = 3)
			*/
			W_EXP static W_RESULT save_hdr_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const float* pData ,_In_ int pCompCount);
			/*
				save jpg image file
				@param pFilePath, path of file
				@param pSize, size of file (width, height)
				@param pData, pointer to rgba data
				@param pCompCount, number of channels(RGBA = 4, RGB = 3)
				@param pQuality, Quality(1 - 100)
			*/
			W_EXP static W_RESULT save_jpg_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData,_In_ int pCompCount, _In_ int pQuality);

            //release all shared textures
            W_EXP ULONG static release_shared_textures();

#pragma region Getters

            //get width of image
            W_EXP const uint32_t get_width() const;
            //get height of image
            W_EXP const uint32_t get_height() const;
			//get image usage
			W_EXP const w_image_usage_flags get_usage_flags() const;
			//get sampler of image
            W_EXP w_sampler get_sampler(_In_ const w_sampler_type& pSamplerType = w_sampler_type::NO_MIPMAP_AND_NO_ANISOTROPY) const;
            //get image and view resources
            W_EXP w_image_view get_image_view() const;
            //get image type
            W_EXP w_image_type get_image_type() const;
            //get image view type
            W_EXP w_image_view_type get_image_view_type() const;
            //get image format
            W_EXP w_format get_format() const;
            //get write descriptor image info
            W_EXP const w_descriptor_image_info get_descriptor_info(_In_ const w_sampler_type& pSamplerType = w_sampler_type::NO_MIPMAP_AND_NO_ANISOTROPY) const;
			//get number of mip maps levels
			W_EXP const uint32_t get_mip_maps_level() const;

#pragma endregion

#pragma region Setters

			//set image format
			W_EXP void set_format(_In_ w_format pFormat);
			//set image usage
			W_EXP void set_usage_flags(_In_ w_image_usage_flags pUsage);
			//set buffer type
			W_EXP void set_buffer_type(_In_ w_texture_buffer_type pBufferType);
			//set image view type
			W_EXP void set_view_type(_In_ w_image_view_type pViewType);

#pragma region

            W_EXP static w_texture*                               default_texture;
            
#ifdef __PYTHON__

			bool py_initialize_simple(
				_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const uint32_t& pWidth,
				_In_ const uint32_t& pHeight,
				_In_ const bool& pGenerateMipMapsLevels,
				_In_ const bool& pIsStaging)
			{
				if (!pGDevice.get()) return false;
				auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

				auto _hr = initialize(_gDevice, pWidth, pHeight, pGenerateMipMapsLevels, pIsStaging);

				_gDevice.reset();
				return _hr == W_PASSED;
			}

			bool py_initialize_advanced(
				_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const uint32_t& pWidth,
				_In_ const uint32_t& pHeight,
				_In_ const bool& pGenerateMipMapsLevels,
				_In_ const w_memory_property_flags pMemoryPropertyFlags)
			{
				if (!pGDevice.get()) return false;
				auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

				auto _hr = initialize(_gDevice, pWidth, pHeight, pGenerateMipMapsLevels, pMemoryPropertyFlags);

				_gDevice.reset();
				return _hr == W_PASSED;
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

				auto _hr = load_texture_from_memory_rgba(&_data[0]);
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

				auto _hr = load_texture_from_memory_rgb(&_data[0]);
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

				auto _hr = copy_data_to_texture_2D(&_data[0]);
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
				return flush_staging_data() == W_PASSED;
			}

			w_sampler py_get_sampler(_In_ const w_sampler_type& pSamplerType)
			{
				return get_sampler(pSamplerType);
			}

			w_descriptor_image_info py_get_descriptor_info(_In_ const w_sampler_type& pSamplerType)
			{
				return get_descriptor_info(pSamplerType);
			}

			static bool py_load_to_shared_textures(
				_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
				_In_z_ std::wstring pPath)
			{
				w_texture* _texture = nullptr;

				if (!pGDevice.get()) return false;
				auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

				auto _hr = load_to_shared_textures(
					_gDevice,
					pPath,
					&_texture);

				_gDevice.reset();

				if (_hr != W_PASSED)
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
				return _hr == W_PASSED;
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
				return _hr == W_PASSED;
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
				return _hr == W_PASSED;
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
				return _hr == W_PASSED;
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
				return _hr == W_PASSED;
			}
#endif

		private:
			typedef system::w_object						_super;
            w_texture_pimp*                                 _pimp;

            static std::map<std::wstring, w_texture*>       _shared;
        };
    }
}

#include "python_exporter/py_texture.h"

#endif//__W_TEXTURE_2D_H__
