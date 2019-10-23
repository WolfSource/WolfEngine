/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
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

#ifdef __PYTHON__
#include "w_std.h"
#include "w_thread_pool.h"
#endif

namespace wolf
{
	namespace render
	{
		namespace vulkan
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
				W_VK_EXP w_texture();
				W_VK_EXP virtual ~w_texture();

				W_VK_EXP W_RESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const uint32_t& pWidth = 32,
					_In_ const uint32_t& pHeight = 32,
					_In_ const bool& pGenerateMipMapsLevels = false,
					_In_ const bool& pHasStagingBuffer = false,
					_In_ const uint32_t pMemoryPropertyFlags = w_memory_property_flag_bits::DEVICE_LOCAL_BIT);

				//Load texture
				W_VK_EXP W_RESULT load();

				//Load texture from file
				W_VK_EXP W_RESULT load_texture_2D_from_file(_In_z_ std::wstring pPath, _In_ bool pIsAbsolutePath = false);
				//Load texture from memory in the format of RGBA
				W_VK_EXP W_RESULT load_texture_from_memory_rgba(_In_ uint8_t* pRGBAData);
				//Load texture from memory in the format of RGB
				W_VK_EXP W_RESULT load_texture_from_memory_rgb(_In_ uint8_t* pRGBData);
				//Load texture from memory, all channels will have same byte
				W_VK_EXP W_RESULT load_texture_from_memory_all_channels_same(_In_ uint8_t pData);
				//Load texture from w_color
				W_VK_EXP W_RESULT load_texture_from_memory_color(_In_ w_color pColor);
				/*
					copy data to texture
					if this is a staging buffer, do not use this function because it will cause memory leaks,
					instead use "get_pointer_to_staging_data" function
				*/
				W_VK_EXP W_RESULT copy_data_to_texture_2D(_In_ const uint8_t* pRGBA);

				//read texture's data
				W_VK_EXP void* read_data_of_texture();

				//flush staging buffer
				W_VK_EXP W_RESULT flush_staging_data();

				//release all resources
				W_VK_EXP virtual ULONG release() override;

				//load texture and store it into the shared
				W_VK_EXP static W_RESULT load_to_shared_textures(
					_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_z_ const std::wstring& pPath,
					_In_z_ const bool& pGenerateMipMaps,
					_Inout_ w_texture** pPointerToTexture);

				/*
					save png image file
					@param pFilePath, path of file
					@param pSize, size of file (width, height)
					@param pData, pointer to rgba data
					@param pCompCount, number of channels(RGBA = 4, RGB = 3)
					@param pStrideInBytes, stride of pixel's structure inf bytes
				*/
				W_VK_EXP static W_RESULT save_png_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount, _In_ int pStrideInBytes = 4 * sizeof(uint8_t));
				/*
					save bmp image file
					@param pFilePath, path of file
					@param pSize, size of file (width, height)
					@param pData, pointer to rgba data
					@param pCompCount, number of channels(RGBA = 4, RGB = 3)
				*/
				W_VK_EXP static W_RESULT save_bmp_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount);
				/*
					save tga image file
					@param pFilePath, path of file
					@param pSize, size of file (width, height)
					@param pData, pointer to rgba data
					@param pCompCount, number of channels(RGBA = 4, RGB = 3)
					@param pQuality, Quality(1 - 100)
				*/
				W_VK_EXP static W_RESULT save_tga_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount);
				/*
					save hdr image file
					@param pFilePath, path of file
					@param pSize, size of file (width, height)
					@param pData, pointer to rgba data
					@param pCompCount, number of channels(RGBA = 4, RGB = 3)
				*/
				W_VK_EXP static W_RESULT save_hdr_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const float* pData, _In_ int pCompCount);
				/*
					save jpg image file
					@param pFilePath, path of file
					@param pSize, size of file (width, height)
					@param pData, pointer to rgba data
					@param pCompCount, number of channels(RGBA = 4, RGB = 3)
					@param pQuality, Quality(1 - 100)
				*/
				W_VK_EXP static W_RESULT save_jpg_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount, _In_ int pQuality);

				//release all shared textures
				W_VK_EXP ULONG static release_shared_textures();

#pragma region Getters

				//get width of image
				W_VK_EXP const uint32_t get_width() const;
				//get height of image
				W_VK_EXP const uint32_t get_height() const;
				//get image usage
				W_VK_EXP const uint32_t get_usage_flags() const;
				//get sampler of image
				W_VK_EXP w_sampler get_sampler(_In_ const w_sampler_type& pSamplerType = w_sampler_type::NO_MIPMAP_AND_NO_ANISOTROPY) const;
				//get image and view resources
				W_VK_EXP w_image_view get_image_view() const;
				//get image type
				W_VK_EXP w_image_type get_image_type() const;
				//get image view type
				W_VK_EXP w_image_view_type get_image_view_type() const;
				//get image format
				W_VK_EXP w_format get_format() const;
				//get write descriptor image info
				W_VK_EXP const w_descriptor_image_info get_descriptor_info(_In_ const w_sampler_type& pSamplerType = w_sampler_type::NO_MIPMAP_AND_NO_ANISOTROPY) const;
				//get number of mip maps levels
				W_VK_EXP const uint32_t get_mip_maps_level() const;

#pragma endregion

#pragma region Setters

				//set image format
				W_VK_EXP void set_format(_In_ w_format pFormat);
				//set image usage
				W_VK_EXP void set_usage_flags(_In_ uint32_t pUsage);
				//set buffer type
				W_VK_EXP void set_buffer_type(_In_ w_texture_buffer_type pBufferType);
				//set image view type
				W_VK_EXP void set_view_type(_In_ w_image_view_type pViewType);

#pragma region

				W_VK_EXP static w_texture*                               default_texture;

#ifdef __PYTHON__

				W_RESULT py_initialize_as_staging(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const uint32_t& pWidth,
					_In_ const uint32_t& pHeight,
					_In_ const bool& pGenerateMipMapsLevels)
				{
					if (!pGDevice.get()) return W_FAILED;
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

					auto _hr = initialize(_gDevice, pWidth, pHeight, pGenerateMipMapsLevels, true);

					_gDevice.reset();
					return _hr;
				}

				W_RESULT py_initialize(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const uint32_t& pWidth = 32,
					_In_ const uint32_t& pHeight = 32,
					_In_ const bool& pGenerateMipMapsLevels = false,
					_In_ const uint32_t pMemoryPropertyFlags = w_memory_property_flag_bits::DEVICE_LOCAL_BIT)
				{
					if (!pGDevice.get()) return W_FAILED;
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

					auto _hr = initialize(_gDevice, pWidth, pHeight, pGenerateMipMapsLevels, pMemoryPropertyFlags);

					_gDevice.reset();
					return _hr;
				}

				W_RESULT py_load_texture_from_memory_rgba(_In_ boost::python::list pRGBAData)
				{
					auto _len = len(pRGBAData);
					if (!_len) return W_FAILED;

					auto _data = new uint8_t[_len];
					for (size_t i = 0; i < _len; ++i)
					{
						_data[i] = boost::python::extract<uint8_t>(pRGBAData[i]);
					}

					auto _hr = load_texture_from_memory_rgba(&_data[0]);
					SAFE_DELETE_ARRAY(_data);
					return _hr;
				}

				W_RESULT py_load_texture_from_memory_rgb(_In_ boost::python::list  pRGBData)
				{
					auto _len = len(pRGBData);
					if (!_len) return W_FAILED;

					auto _data = new uint8_t[_len];
					for (size_t i = 0; i < _len; ++i)
					{
						_data[i] = boost::python::extract<uint8_t>(pRGBData[i]);
					}

					auto _hr = load_texture_from_memory_rgb(&_data[0]);
					SAFE_DELETE_ARRAY(_data);
					return _hr;
				}

				W_RESULT py_copy_data_to_texture_2D(_In_ w_vector_uint8_t pRGBData)
				{
					if (!pRGBData.size()) return W_FAILED;
					return copy_data_to_texture_2D(pRGBData.data());
				}

				w_vector_uint8_t py_read_data_of_texture()
				{
					auto _size = get_width() * get_height() * 4;
					if (_size)
					{
						auto _data = (uint8_t*)read_data_of_texture();
						if (_data)
						{
							using namespace wolf::system;

							std::vector<uint8_t> _v(_size);

							//get number of cpu cores
							auto _number_of_cpu_cores = w_thread::get_number_of_hardware_thread_contexts();
							//get chuck size
							auto _chuck_size = (int)(_size / _number_of_cpu_cores);
							//must be added to last chuck
							auto _add_to_last_chuck = _size % _number_of_cpu_cores;

							if (_chuck_size > 1)
							{
								w_thread_pool _thread_pool;
								_thread_pool.allocate(_number_of_cpu_cores);

								for (size_t i = 0; i < _number_of_cpu_cores; ++i)
								{
									auto _index = i * _chuck_size;
									auto _size = _chuck_size;

									if (_add_to_last_chuck && i == _number_of_cpu_cores - 1)
									{
										_size += _add_to_last_chuck;
									}

									_thread_pool.add_job_for_thread(i, [&, _index, _size]()->void
									{
										memcpy(&_v[_index], &_data[_index], _size);
									});
								}
								_thread_pool.wait_all();
								_thread_pool.release();
							}
							else
							{
								memcpy(&_v[0], &_data[0], _size);
							}

							return _v;
						}
					}
					return std::vector<uint8_t>();
				}

				static W_RESULT py_load_to_shared_textures(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_z_ std::wstring pPath)
				{
					w_texture* _texture = nullptr;

					if (!pGDevice.get()) return W_FAILED;
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

					auto _hr = load_to_shared_textures(
						_gDevice,
						pPath,
						&_texture);

					_gDevice.reset();

					if (_hr != W_PASSED)
					{
						SAFE_DELETE(_texture);
						return W_FAILED;
					}

					return W_PASSED;
				}

				static W_RESULT py_save_png_to_file(
					_In_z_ std::string pFilePath,
					_In_ uint32_t pWidth,
					_In_ uint32_t pHeight,
					_In_ boost::python::list pData,
					_In_ int pCompCount,
					_In_ int pStrideInBytes = 4 * sizeof(uint8_t))
				{
					auto _len = len(pData);
					if (pFilePath.empty() || pWidth == 0 || pHeight == 0 || _len == 0) return W_FAILED;

					auto _data = new uint8_t[_len];
					for (size_t i = 0; i < _len; ++i)
					{
						_data[i] = boost::python::extract<uint8_t>(pData[i]);
					}
					auto _hr = save_png_to_file(pFilePath.c_str(), pWidth, pHeight, &_data[0], pCompCount, pStrideInBytes);
					SAFE_DELETE_ARRAY(_data);
					return _hr;
				}

				static W_RESULT py_save_bmp_to_file(
					_In_z_ std::string pFilePath,
					_In_ uint32_t pWidth,
					_In_ uint32_t pHeight,
					_In_ boost::python::list pData,
					_In_ int pCompCount)
				{
					auto _len = len(pData);
					if (pFilePath.empty() || pWidth == 0 || pHeight == 0 || _len == 0) return W_FAILED;

					auto _data = new uint8_t[_len];
					for (size_t i = 0; i < _len; ++i)
					{
						_data[i] = boost::python::extract<uint8_t>(pData[i]);
					}
					auto _hr = save_bmp_to_file(pFilePath.c_str(), pWidth, pHeight, &_data[0], pCompCount);
					SAFE_DELETE_ARRAY(_data);
					return _hr;
				}

				static W_RESULT py_save_tga_to_file(
					_In_z_ std::string pFilePath,
					_In_ uint32_t pWidth,
					_In_ uint32_t pHeight,
					_In_ boost::python::list pData,
					_In_ int pCompCount)
				{
					auto _len = len(pData);
					if (pFilePath.empty() || pWidth == 0 || pHeight == 0 || _len == 0) return W_FAILED;

					auto _data = new uint8_t[_len];
					for (size_t i = 0; i < _len; ++i)
					{
						_data[i] = boost::python::extract<uint8_t>(pData[i]);
					}
					auto _hr = save_tga_to_file(pFilePath.c_str(), pWidth, pHeight, &_data[0], pCompCount);
					SAFE_DELETE_ARRAY(_data);
					return _hr;
				}

				static W_RESULT py_save_hdr_to_file(
					_In_z_ std::string pFilePath,
					_In_ uint32_t pWidth,
					_In_ uint32_t pHeight,
					_In_ boost::python::list pData,
					_In_ int pCompCount)
				{
					auto _len = len(pData);
					if (pFilePath.empty() || pWidth == 0 || pHeight == 0 || _len == 0) return W_FAILED;

					auto _data = new float[_len];
					for (size_t i = 0; i < _len; ++i)
					{
						_data[i] = boost::python::extract<float>(pData[i]);
					}
					auto _hr = save_hdr_to_file(pFilePath.c_str(), pWidth, pHeight, &_data[0], pCompCount);
					SAFE_DELETE_ARRAY(_data);
					return _hr;
				}

				static W_RESULT py_save_jpg_to_file(
					_In_z_ std::string pFilePath,
					_In_ uint32_t pWidth,
					_In_ uint32_t pHeight,
					_In_ boost::python::list pData,
					_In_ int pCompCount,
					_In_ int pQuality)
				{
					auto _len = len(pData);
					if (pFilePath.empty() || pWidth == 0 || pHeight == 0 || _len == 0) return W_FAILED;

					auto _data = new float[_len];
					for (size_t i = 0; i < _len; ++i)
					{
						_data[i] = boost::python::extract<uint8_t>(pData[i]);
					}
					auto _hr = save_jpg_to_file(pFilePath.c_str(), pWidth, pHeight, &_data[0], pCompCount, pQuality);
					SAFE_DELETE_ARRAY(_data);
					return _hr;
				}
#endif

			private:
				typedef system::w_object						_super;
				w_texture_pimp*                                 _pimp;

				static std::map<std::wstring, w_texture*>       _shared;
			};
		}
	}
}

#include "python_exporter/py_texture.h"

#endif//__W_TEXTURE_2D_H__
