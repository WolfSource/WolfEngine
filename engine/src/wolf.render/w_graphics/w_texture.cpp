#include "w_render_pch.h"
#include "w_texture.h"
#include <w_convert.h>
#include <w_io.h>
#include "w_buffer.h"
#include "w_command_buffers.h"
#include <map>

#include <gli/gli.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace wolf
{
    namespace graphics
    {
        class w_texture_pimp
        {
        public:
			w_texture_pimp() :
				_name("w_texture"),
				_gDevice(nullptr),
				_layer_count(1),
				_generate_mip_maps(false),
				_mip_map_levels(1),
				_usage_flags(w_image_usage_flag_bits::IMAGE_USAGE_TRANSFER_DST_BIT | w_image_usage_flag_bits::IMAGE_USAGE_SAMPLED_BIT),
				_is_staging(false),
				_staging_buffer_memory_pointer(nullptr),
				_image_type(w_image_type::_2D_TYPE),
				_image_view_type(w_image_view_type::_2D),
				_buffer_type(VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT),
				_image_layout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				this->_image_view.attachment_desc.desc.format = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
			}

            ~w_texture_pimp()
            {
                release();
            }

			W_RESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const uint32_t& pWidth,
				_In_ const uint32_t& pHeight,
				_In_ const bool& pGenerateMipMaps,
				_In_ const bool& pHasStagingBuffer,
				_In_ const uint32_t& pMemoryPropertyFlags)
			{
				this->_gDevice = pGDevice;
				this->_memory_property_flags = pMemoryPropertyFlags;
				this->_image_view.width = pWidth;
				this->_image_view.height = pHeight;
				this->_generate_mip_maps = pGenerateMipMaps;
				if (this->_generate_mip_maps)
				{
					//add VK_IMAGE_USAGE_TRANSFER_SRC_BIT flag to image's usage
					this->_usage_flags |= w_image_usage_flag_bits::IMAGE_USAGE_TRANSFER_SRC_BIT;
				}

				this->_is_staging = pHasStagingBuffer;
				this->_just_initialized = true;

				return W_PASSED;
			}

			W_RESULT load()
			{
				auto _hr = _create_image();
				if (_hr == W_FAILED) return W_FAILED;

				_hr = _allocate_memory();
				if (_hr == W_FAILED) return W_FAILED;

				//bind to memory
				if (vkBindImageMemory(this->_gDevice->vk_device,
					this->_image_view.image,
					this->_memory.handle,
					0))
				{
					V(W_FAILED, "binding VkImage for graphics device: " + this->_gDevice->device_info->get_device_name() +
						" ID: " + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
					return W_FAILED;
				}

				_hr = _create_sampler();
				if (_hr == W_FAILED) return W_FAILED;
				
				_hr = _create_image_view();
				if (_hr == W_FAILED) return W_FAILED;

				return W_PASSED;
			}

			W_RESULT load_texture_2D_from_file(_In_z_ std::wstring pPath, _In_ bool pIsAbsolutePath)
			{
				using namespace std;
				using namespace system::io;

				std::wstring _path = pPath;
				if (!pIsAbsolutePath)
				{
					_path = content_path + pPath;
				}
				auto _ext = get_file_extentionW(_path.c_str());
				this->_texture_name = get_base_file_nameW(_path.c_str()) + L"." + _ext;

#if defined(__WIN32) || defined(__UWP)
				std::wstring _str = _path;
#else
				std::string _str = wolf::system::convert::wstring_to_string(_path);
#endif

				if (W_FAILED == system::io::get_is_file(_str.c_str()))
				{
					wstring msg = L"could not find the texture file: ";
					V(W_FAILED, msg + _path, this->_name, 3);
					return W_FAILED;
				}

				std::string _g_path;

#if defined(__WIN32) || defined(__UWP)
				_g_path = wolf::system::convert::to_utf8(_str).c_str();
#else
				_g_path = _str;
#endif

				//ptr to rgba data
				uint8_t* _rgba = nullptr;
				//for dds and ktx
				gli::texture2d_array* _gli_tex_2D_array = nullptr;

				//lower it
				std::transform(_ext.begin(), _ext.end(), _ext.begin(), ::tolower);

				if (_ext == L".dds" || _ext == L".ktx")
				{

#if defined(__ANDROID__)
					// Textures are stored inside the apk on Android (compressed)
					// So they need to be loaded via the asset manager
					/*AAsset* asset = AAssetManager_open(androidApp->activity->assetManager, filename.c_str(), AASSET_MODE_STREAMING);
					assert(asset);
					size_t size = AAsset_getLength(asset);
					assert(size > 0);

					void *textureData = malloc(size);
					AAsset_read(asset, textureData, size);
					AAsset_close(asset);

					_gli_texture = gli::load((const char*)textureData, size);
*/
#else
					//we re loading file with gli header
					auto _gli_tex = gli::load(_g_path.c_str());
#endif
					if (_gli_tex.size())
					{
						_gli_tex_2D_array = new gli::texture2d_array(_gli_tex);

						this->_image_view.width = _gli_tex_2D_array->extent().x;
						this->_image_view.height = _gli_tex_2D_array->extent().y;
						this->_layer_count = (uint32_t)_gli_tex_2D_array->layers();
						this->_image_view.attachment_desc.desc.format = (VkFormat)_gli_format_to_wolf_format(_gli_tex_2D_array->format());
					}

					_g_path.clear();
				}
				else if (_ext == L".jpg" || _ext == L".bmp" || _ext == L".png" || _ext == L".psd" || _ext == L".tga")
				{
					//we re loading file with stbi header

					int __width, __height, __comp;

					_rgba = stbi_load(_g_path.c_str(), &__width, &__height, &__comp, STBI_rgb_alpha);

					this->_image_view.width = __width;
					this->_image_view.height = __height;
					this->_layer_count = 1;

					_g_path.clear();
				}
				else
				{
					logger.error(L"Format not supported yet for following file: " + _path);
					return W_FAILED;
				}
				
				if (this->_image_view.width == 0 || this->_image_view.height == 0)
				{
					wstring msg = L"Width or Height of texture file is zero: ";
					V(W_FAILED, msg + _path, this->_name, 3);
					if (_rgba)
					{
						free(_rgba);
						_rgba = nullptr;
					}
					release();
					return W_FAILED;
				}

				auto _hr = _create_image();
				if (_hr == W_FAILED)
				{
					if (_rgba)
					{
						free(_rgba);
						_rgba = nullptr;
					}
					release();
					return W_FAILED;
				}

				_hr = _allocate_memory();
				if (_hr == W_FAILED)
				{
					if (_rgba)
					{
						free(_rgba);
						_rgba = nullptr;
					}
					release();
					return W_FAILED;
				}

				//bind to memory
				if (vkBindImageMemory(this->_gDevice->vk_device,
					this->_image_view.image,
					this->_memory.handle,
					0))
				{
					V(W_FAILED, "binding VkImage for graphics device: " + this->_gDevice->device_info->get_device_name() +
						" ID: " + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
					return W_FAILED;
				}

				//copy data to texture
				if (_rgba)
				{
					_hr = copy_data_to_texture_2D(_rgba);
					if (_hr == W_FAILED) return W_FAILED;

					free(_rgba);
				}
				else if (_gli_tex_2D_array)
				{
					_hr = copy_data_to_texture_2D_array(*_gli_tex_2D_array);
					if (_hr == W_FAILED) return W_FAILED;
					SAFE_DELETE(_gli_tex_2D_array);
				}
				else
				{
					wstring msg = L"texture file is corrupted: ";
					V(W_FAILED, msg + _path, this->_name, 3);
					release();
					return W_FAILED;
				}

                _hr = _create_sampler();
                if (_hr == W_FAILED) return W_FAILED;
                
				_hr = _create_image_view();
				if (_hr == W_FAILED) return W_FAILED;

				return W_PASSED;
			}

            W_RESULT load_texture_from_memory_rgba(_In_ uint8_t* pRGBAData)
            {
				this->_texture_name = L"texture_from_memory_rgba";

                auto _hr = _create_image();
                if (_hr == W_FAILED) return W_FAILED;

                _hr = _allocate_memory();
                if (_hr == W_FAILED) return W_FAILED;

                //bind to memory
				if (vkBindImageMemory(this->_gDevice->vk_device,
					this->_image_view.image,
					this->_memory.handle,
					0))
				{
					V(W_FAILED, "binding VkImage for graphics device: " + this->_gDevice->device_info->get_device_name() +
						" ID: " + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
					return W_FAILED;
				}
                
                _hr = copy_data_to_texture_2D(pRGBAData);
                if (_hr == W_FAILED) return W_FAILED;
                
                _hr = _create_sampler();
                if (_hr == W_FAILED) return W_FAILED;
                
				_hr = _create_image_view();
				if (_hr == W_FAILED) return W_FAILED;

                return W_PASSED;
            }

			W_RESULT _create_image()
			{
                //generate number of mip maps
                if (this->_generate_mip_maps)
                {
                    this->_mip_map_levels = static_cast<uint32_t>(std::floor(std::log2(std::max(this->_image_view.width, this->_image_view.height))) + 1);
                }
				const VkImageCreateInfo _image_create_info =
				{
					VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,							// Type;
					nullptr,														// Next
					0,																// Flags
					(VkImageType)this->_image_type,								    // ImageType
					this->_image_view.attachment_desc.desc.format,					// Format
					{																// Extent
						this->_image_view.width,									// Width
						this->_image_view.height,									// Height
						1															// Depth
					},
					this->_mip_map_levels ,											// MipLevels
					this->_layer_count,												// ArrayLayers
					VK_SAMPLE_COUNT_1_BIT,											// Samples
					VK_IMAGE_TILING_OPTIMAL,										// Tiling
					(VkImageUsageFlags)this->_usage_flags,							// Usage
					VK_SHARING_MODE_EXCLUSIVE,										// SharingMode
					0,																// QueueFamilyIndexCount
					nullptr,														// QueueFamilyIndices
					VK_IMAGE_LAYOUT_UNDEFINED										// InitialLayout
				};
								
				//release the previous image
				if (this->_image_view.image)
				{
					//first release it
					vkDestroyImage(
						this->_gDevice->vk_device,
						this->_image_view.image,
						nullptr);
				}

				auto _hr = vkCreateImage(
					this->_gDevice->vk_device,
					&_image_create_info,
					nullptr,
					&this->_image_view.image);
				if (_hr)
				{
					V(W_FAILED, "creating VkImage for graphics device: " + this->_gDevice->device_info->get_device_name() +
						" ID: " + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
					return W_FAILED;
				}

				return W_PASSED;
			}
            
			W_RESULT _allocate_memory()
			{
				//release the old one
				if (this->_memory.handle)
				{
					vkFreeMemory(this->_gDevice->vk_device, this->_memory.handle, nullptr);
				}

				VkMemoryRequirements _image_memory_requirements;
				vkGetImageMemoryRequirements(
					this->_gDevice->vk_device,
					this->_image_view.image,
					&_image_memory_requirements);

				VkPhysicalDeviceMemoryProperties _memory_properties;
				vkGetPhysicalDeviceMemoryProperties(this->_gDevice->vk_physical_device, &_memory_properties);

				uint32_t _mem_index = 0;
				if (w_graphics_device_manager::memory_type_from_properties(
					_memory_properties,
					_image_memory_requirements.memoryTypeBits,
					(VkMemoryPropertyFlags)this->_memory_property_flags,
					&_mem_index))
				{
					V(W_FAILED, "finding memory index of Image for graphics device: " + this->_gDevice->device_info->get_device_name() +
						" ID: " + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
					return W_FAILED;
				}

				const VkMemoryAllocateInfo _memory_allocate_info =
				{
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // Type
					nullptr,                                    // Next
					_image_memory_requirements.size,            // AllocationSize
					_mem_index                                  // MemoryTypeIndex
				};

				if (vkAllocateMemory(this->_gDevice->vk_device,
					&_memory_allocate_info,
					nullptr,
					&this->_memory.handle))
				{
					V(W_FAILED, "allocating memory for Image for graphics device: " + this->_gDevice->device_info->get_device_name() +
						" ID: " + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
					return W_FAILED;
				}

				return W_PASSED;
			}
            
			W_RESULT _create_image_view()
			{
				if (this->_image_view.view)
				{
					vkDestroyImageView(this->_gDevice->vk_device, this->_image_view.view, nullptr);
					this->_image_view.view = 0;
				}

				const VkImageViewCreateInfo _image_view_create_info =
				{
					VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,             // Type
					nullptr,                                              // Next
					0,                                                    // Flags
					this->_image_view.image,                              // Image
					(VkImageViewType)this->_image_view_type,              // ViewType
					this->_image_view.attachment_desc.desc.format,        // Format
					{                                                     // Components
						VK_COMPONENT_SWIZZLE_R,                                 // VkComponentSwizzle         r
						VK_COMPONENT_SWIZZLE_G,                                 // VkComponentSwizzle         g
						VK_COMPONENT_SWIZZLE_B,                                 // VkComponentSwizzle         b
						VK_COMPONENT_SWIZZLE_A                                  // VkComponentSwizzle         a
					},
					{                                                     // SubresourceRange
						this->_buffer_type,										// aspectMask
						0,                                                      // baseMipLevel
						this->_mip_map_levels,                                  // levelCount
						0,														// baseArrayLayer
						this->_layer_count                                      // layerCount
					}
				};

				auto _hr = vkCreateImageView(
					this->_gDevice->vk_device,
					&_image_view_create_info,
					nullptr,
					&this->_image_view.view);
				if (_hr)
				{
					V(W_FAILED, "creating image view", this->_name, false, true);
					return W_FAILED;
				}

				this->_image_view.width = this->_image_view.width;
				this->_image_view.height = this->_image_view.height;

				return W_PASSED;
			}
            
			W_RESULT _create_sampler()
			{
				VkSamplerCreateInfo _sampler_create_info = {};
				_sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				_sampler_create_info.pNext = nullptr;
				_sampler_create_info.flags = 0;
				_sampler_create_info.magFilter = VK_FILTER_LINEAR;
				_sampler_create_info.minFilter = VK_FILTER_LINEAR;
				_sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				_sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
				_sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
				_sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
				_sampler_create_info.mipLodBias = 0.0f;
				_sampler_create_info.compareOp = VK_COMPARE_OP_NEVER;
				_sampler_create_info.compareEnable = VK_FALSE;
				_sampler_create_info.minLod = 0.0f;
				_sampler_create_info.maxLod = 0.0f;
				_sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
				_sampler_create_info.maxAnisotropy = 1.0;
				_sampler_create_info.anisotropyEnable = VK_FALSE;
				_sampler_create_info.unnormalizedCoordinates = VK_FALSE;
								
				//create sampler without mipmap and anistropy
				w_sampler _no_mip_map_no_anisotropy_sampler;
				auto _hr = vkCreateSampler(
					this->_gDevice->vk_device,
					&_sampler_create_info,
					nullptr,
					&_no_mip_map_no_anisotropy_sampler.handle);
				if (_hr)
				{
					V(W_FAILED, "creating sampler without mip map and without anisotropy", this->_name, false, true);
					return W_FAILED;
				}
				this->_samplers.insert({ w_sampler_type::NO_MIPMAP_AND_NO_ANISOTROPY, _no_mip_map_no_anisotropy_sampler });

				//create sampler with mipmap and without anistropy
				w_sampler _mip_map_no_anisotropy_sampler;
				_sampler_create_info.maxLod = static_cast<float>(this->_mip_map_levels);
				_hr = vkCreateSampler(
					this->_gDevice->vk_device,
					&_sampler_create_info,
					nullptr,
					&_mip_map_no_anisotropy_sampler.handle);
				if (_hr)
				{
					V(W_FAILED, "creating sampler with mip map and without anisotropy", this->_name, false, true);
					return W_FAILED;
				}
				this->_samplers.insert({ w_sampler_type::MIPMAP_AND_NO_ANISOTROPY, _mip_map_no_anisotropy_sampler });

				//create sampler with mipmap and anistropy
				if (this->_gDevice->device_info->device_features->samplerAnisotropy)
				{
					_sampler_create_info.maxAnisotropy = this->_gDevice->device_info->device_properties->limits.maxSamplerAnisotropy;
					_sampler_create_info.anisotropyEnable = VK_TRUE;

					w_sampler _mip_map_anisotropy_sampler;
					_hr = vkCreateSampler(
						this->_gDevice->vk_device,
						&_sampler_create_info,
						nullptr,
						&_mip_map_anisotropy_sampler.handle);
					if (_hr)
					{
						V(W_FAILED, "creating sampler with mip map and with anisotropy", this->_name, false, true);
						return W_FAILED;
					}

					this->_samplers.insert({ w_sampler_type::MIPMAP_AND_ANISOTROPY, _mip_map_anisotropy_sampler });

					//create sampler with no mipmap and anistropy
					w_sampler _no_mip_map_anisotropy_sampler;
					_sampler_create_info.maxLod = 0;					
					_hr = vkCreateSampler(
						this->_gDevice->vk_device,
						&_sampler_create_info,
						nullptr,
						&_no_mip_map_anisotropy_sampler.handle);
					if (_hr)
					{
						V(W_FAILED, "creating sampler without mip map and with anisotropy", this->_name, false, true);
						return W_FAILED;
					}
					this->_samplers.insert({ w_sampler_type::NO_MIPMAP_AND_ANISOTROPY, _no_mip_map_anisotropy_sampler });
				}

				return W_PASSED;
			}
            
			w_format _gli_format_to_wolf_format(_In_ gli::format pFormat)
			{
				//direct map to vulkan formats
				return (w_format)pFormat;
			}

            W_RESULT copy_data_to_texture_2D(_In_ const uint8_t* pRGBA)
            {
				const std::string _trace_info = "w_texture::copy_data_to_texture_2D";

                auto _data_size = this->_image_view.width * this->_image_view.height * 4;
				W_RESULT _hr = W_RESULT::W_FAILED;
				
				if (this->_just_initialized)
				{
					this->_just_initialized = false;
					_hr = this->_staging_buffer.allocate_as_staging(this->_gDevice, _data_size);
					if (_hr == W_FAILED)
					{
						return _hr;
					}
				}

				this->_staging_buffer_memory_pointer = this->_staging_buffer.map();
				if (this->_staging_buffer_memory_pointer)
				{
					memcpy(this->_staging_buffer_memory_pointer, pRGBA, (size_t)_data_size);
					_hr = this->_staging_buffer.flush();
					this->_staging_buffer.unmap();

					if (_hr == W_FAILED)
					{
						V(W_FAILED,
							"flushing staging memory for graphics device" + this->_gDevice->get_info(),
							_trace_info,
							3,
							false);
						return W_FAILED;
					}
				}
				else
				{
					V(W_FAILED,
						"mapping staging memory for graphics device" + this->_gDevice->get_info(),
						_trace_info,
						3,
						false);
					return W_FAILED;
				}

                //create command buffer
                w_command_buffers _command_buffer;
                _command_buffer.load(this->_gDevice, 1);
                auto _cmd = _command_buffer.get_command_at(0);

                _command_buffer.begin(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
				{
					VkImageSubresourceRange _image_subresource_range =
					{
						this->_buffer_type,								// AspectMask
						0,                                              // BaseMipLevel
						1,                                              // LevelCount
						0,                                              // BaseArrayLayer
						this->_layer_count                              // LayerCount
					};

					VkImageMemoryBarrier _image_memory_barrier = {};
					_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					_image_memory_barrier.pNext = nullptr;
					_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					_image_memory_barrier.image = this->_image_view.image;
					_image_memory_barrier.subresourceRange = _image_subresource_range;

					w_graphics_device_manager::set_src_dst_masks_of_image_barrier(_image_memory_barrier);

					vkCmdPipelineBarrier(_cmd.handle,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,//VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,//VK_PIPELINE_STAGE_TRANSFER_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&_image_memory_barrier);

					//execute copy to image
					VkBufferImageCopy _buffer_image_copy_info =
					{
						0,                                    // BufferOffset
						0,                                    // BufferRowLength
						0,                                    // BufferImageHeight
						{                                     // ImageSubresource
							this->_buffer_type,				  // AspectMask
							0,                                // MipLevel
							0,                                // BaseArrayLayer
							this->_layer_count                // LayerCount
						},
						{                                     // ImageOffset
							0,                                // X
							0,                                // Y
							0                                 // Z
						},
						{                                     // ImageExtent
							this->_image_view.width,          // Width
							this->_image_view.height,         // Height
							1                                 // Depth
						}
					};

					auto _staging_buffer_handle = this->_staging_buffer.get_buffer_handle();
					vkCmdCopyBufferToImage(_cmd.handle,
						_staging_buffer_handle.handle,
						this->_image_view.image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1,
						&_buffer_image_copy_info);

					_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					_image_memory_barrier.newLayout = this->_generate_mip_maps ? VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL :
						VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
					w_graphics_device_manager::set_src_dst_masks_of_image_barrier(_image_memory_barrier);

					vkCmdPipelineBarrier(_cmd.handle,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,//VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,//VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&_image_memory_barrier);

				}
                _command_buffer.end(0);

				_hr = this->_gDevice->submit(
					{ &_cmd },
					this->_gDevice->vk_graphics_queue,
					{},
					{},
					{},
					nullptr,
					true);
				
				_command_buffer.release();

				if (_hr == W_FAILED)
				{
					V(W_FAILED,
						"submit commad buffer for generating mipmaps for graphics device" + this->_gDevice->device_info->get_device_name() +
						" ID:" + std::to_string(this->_gDevice->device_info->get_device_id()),
						_trace_info,
						3,
						false);
					return W_FAILED;
				}
				
                if (!this->_is_staging)
                {
                    //release staging buffer
                    this->_staging_buffer.release();
                }

				if (this->_generate_mip_maps)
				{
					if (_copy_mip_maps() == W_FAILED)
					{
						V(W_FAILED, "copying mip maps data to texture buffer on graphics device: " +
							this->_gDevice->device_info->get_device_name() + " ID: " + std::to_string(this->_gDevice->device_info->get_device_id()),
							this->_name,
							3,
							false);
						return W_FAILED;
					}
				}

                return W_PASSED;
            }
            
			W_RESULT copy_data_to_texture_2D_array(_In_ const gli::texture2d_array& pTextureArrayRGBA)
			{				
				const std::string _trace_info = "w_texture::copy_data_to_texture_2D_array";

				auto _data_size = static_cast<uint32_t>(pTextureArrayRGBA.size());

				auto _hResult = this->_staging_buffer.allocate_as_staging(this->_gDevice, _data_size);
				if (_hResult == W_FAILED)
				{
					return _hResult;
				}

				//_hResult = this->_staging_buffer.bind();
				//if (_hResult == W_FAILED)
				//{
				//	return _hResult;
				//}

				auto _mem_handle = this->_staging_buffer.get_memory().handle;
				if(vkMapMemory(this->_gDevice->vk_device,
					_mem_handle,
					0,
					_data_size,
					0,
					&this->_staging_buffer_memory_pointer))
				{
					V(W_FAILED, "Could not map memory and upload texture data to a staging buffer on graphics device: " +
						this->_gDevice->device_info->get_device_name() + " ID: " + std::to_string(this->_gDevice->device_info->get_device_id()),
						this->_name,
						3,
						false);
					return W_FAILED;
				}

				memcpy(this->_staging_buffer_memory_pointer, pTextureArrayRGBA.data(), (size_t)_data_size);

				VkMappedMemoryRange _flush_range =
				{
					VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,              // Type
					nullptr,                                            // Next
					_mem_handle,										// Memory
					0,                                                  // Offset
					_data_size                                          // Size
				};

				vkFlushMappedMemoryRanges(this->_gDevice->vk_device,
					1,
					&_flush_range);
				vkUnmapMemory(this->_gDevice->vk_device, _mem_handle);


				//create command buffer
				w_command_buffers _command_buffer;
				_command_buffer.load(this->_gDevice, 1);
				auto _cmd = _command_buffer.get_command_at(0);

				_command_buffer.begin(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
				{
					VkImageSubresourceRange _image_subresource_range =
					{
						this->_buffer_type,								// AspectMask
						0,                                              // BaseMipLevel
						1,                                              // LevelCount
						0,                                              // BaseArrayLayer
						this->_layer_count                              // LayerCount
					};

					VkImageMemoryBarrier _image_memory_barrier = {};
					_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					_image_memory_barrier.pNext = nullptr;
					_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
					_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					_image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					_image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					_image_memory_barrier.image = this->_image_view.image;
					_image_memory_barrier.subresourceRange = _image_subresource_range;

					w_graphics_device_manager::set_src_dst_masks_of_image_barrier(_image_memory_barrier);

					vkCmdPipelineBarrier(_cmd.handle,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&_image_memory_barrier);
					
#pragma region copy all textures to buffer
					std::vector<VkBufferImageCopy> _buffer_copy_regions;
					size_t offset = 0;

					for (uint32_t i = 0; i < this->_layer_count; ++i)
					{
						VkBufferImageCopy _buffer_image_copy_info =
						{
							offset,                               // BufferOffset
							0,                                    // BufferRowLength
							0,                                    // BufferImageHeight
							{                                     // ImageSubresource
								this->_buffer_type,				  // AspectMask
								0,                                // MipLevel
								i,                                // BaseArrayLayer
								1                                 // LayerCount
							},
							{                                     // ImageOffset
								0,                                // X
								0,                                // Y
								0                                 // Z
							},
							{                                     // ImageExtent
								this->_image_view.width,          // Width
								this->_image_view.height,         // Height
								1                                 // Depth
							}
						};

						_buffer_copy_regions.push_back(_buffer_image_copy_info);

						offset += pTextureArrayRGBA[i][0].size();
					}

					auto _staging_buffer_handle = this->_staging_buffer.get_buffer_handle();
					vkCmdCopyBufferToImage(_cmd.handle,
						_staging_buffer_handle.handle,
						this->_image_view.image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						static_cast<uint32_t>(_buffer_copy_regions.size()),
						_buffer_copy_regions.data());
#pragma endregion

					// After all textures have been copied, change layout to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
					_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					_image_memory_barrier.newLayout = this->_image_layout;
					w_graphics_device_manager::set_src_dst_masks_of_image_barrier(_image_memory_barrier);

					vkCmdPipelineBarrier(_cmd.handle,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&_image_memory_barrier);

				}
				_command_buffer.end(0);

				auto _hr = this->_gDevice->submit(
					{ &_cmd },
					this->_gDevice->vk_graphics_queue,
					{},
					{},
					{},
					nullptr,
					true);
				//release command buffer
				_command_buffer.release();
				if (_hr == W_FAILED)
				{
					V(W_FAILED, "Could submit map memory and upload texture data to a staging buffer on graphics device: " +
						this->_gDevice->device_info->get_device_name() + " ID: " + std::to_string(this->_gDevice->device_info->get_device_id()),
						this->_name,
						3,
						false);

					return W_FAILED;
				}

				if (!this->_is_staging)
				{
					//release staging buffer
					this->_staging_buffer.release();
				}
                
               /* if (this->_generate_mip_maps)
                {
                    if (_copy_mip_maps() == W_FAILED)
                    {
                        V(W_FAILED, "copying mip maps data to texture buffer on graphics device: " +
                          this->_gDevice->device_info->get_device_name() + " ID: " + std::to_string(this->_gDevice->device_info->get_device_id()),
                          this->_name,
                          3,
                          false);
                        
                        return W_FAILED;
                    }
                }*/
                               
				return W_PASSED;
            }
            
            W_RESULT _copy_mip_maps()
            {
                const std::string _trace_info = "w_texture::_copy_mip_maps";
                
                bool _bliting_supported = true;
                
				auto _format = this->_image_view.attachment_desc.desc.format;
                //check whether blitting is supported or not
                VkFormatProperties _format_properties;
                // Check for whether blitting is supported for src
                vkGetPhysicalDeviceFormatProperties(
					this->_gDevice->vk_physical_device,
                    _format,
                    &_format_properties);
				if (!(_format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT))
				{
					logger.warning("Blitting feature not supported from optimal tiled image for graphics device: " +
						this->_gDevice->device_info->get_device_name() +
						" ID:" + std::to_string(this->_gDevice->device_info->get_device_id()) +
						" and following format: " + std::to_string(_format));
					_bliting_supported = false;
				}
				//if (_bliting_supported)
				//{
				//	// Check for whether blitting is supported for destination
				//	if (!(_format_properties.linearTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT))
				//	{
				//		logger.warning("Blitting feature not supported from linear tiled image for graphics device: " +
				//			this->_gDevice->device_info->get_device_name() +
				//			" ID:" + std::to_string(this->_gDevice->device_info->get_device_id()) +
				//			" and following format: " + std::to_string(_format));
				//		//_bliting_supported = false;
				//	}
				//}
                
				//create command buffer
				w_command_buffers _command_buffer;
				_command_buffer.load(this->_gDevice, 1);
				_command_buffer.begin(0);
				auto _cmd = _command_buffer.get_command_at(0);
                
				if (_bliting_supported)
				{
					for (auto i = 1; i < this->_mip_map_levels; ++i)
					{
						VkImageBlit _image_blit = {};

						//source
						_image_blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						_image_blit.srcSubresource.layerCount = 1;
						_image_blit.srcSubresource.mipLevel = i - 1;
						_image_blit.srcOffsets[1].x = int32_t(this->_image_view.width >> (i - 1));
						_image_blit.srcOffsets[1].y = int32_t(this->_image_view.height >> (i - 1));
						_image_blit.srcOffsets[1].z = 1;

						//destination
						_image_blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						_image_blit.dstSubresource.layerCount = 1;
						_image_blit.dstSubresource.mipLevel = i;
						_image_blit.dstOffsets[1].x = int32_t(this->_image_view.width >> i);
						_image_blit.dstOffsets[1].y = int32_t(this->_image_view.height >> i);
						_image_blit.dstOffsets[1].z = 1;

						VkImageSubresourceRange _mip_sub_range = {};
						_mip_sub_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						_mip_sub_range.baseMipLevel = i;
						_mip_sub_range.levelCount = 1;
						_mip_sub_range.layerCount = 1;

						// Put barrier inside setup command buffer
						VkImageMemoryBarrier _source_image_memory_barrier = {};
						_source_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
						_source_image_memory_barrier.pNext = nullptr;
						_source_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
						_source_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
						_source_image_memory_barrier.image = this->_image_view.image;
						_source_image_memory_barrier.subresourceRange = _mip_sub_range;

						w_graphics_device_manager::set_src_dst_masks_of_image_barrier(_source_image_memory_barrier);

						vkCmdPipelineBarrier(
							_cmd.handle,
							VK_PIPELINE_STAGE_TRANSFER_BIT,
							VK_PIPELINE_STAGE_HOST_BIT,
							0,
							0, nullptr,
							0, nullptr,
							1,
							&_source_image_memory_barrier);

						//now blit from previous mip map level
						vkCmdBlitImage(
							_cmd.handle,
							this->_image_view.image,
							VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							this->_image_view.image,
							VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							1,
							&_image_blit,
							VK_FILTER_LINEAR);

						//transiton current mip level to transfer source for read in next iteration
						VkImageMemoryBarrier _dst_image_memory_barrier = {};
						_dst_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
						_dst_image_memory_barrier.pNext = nullptr;
						_dst_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
						_dst_image_memory_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
						_dst_image_memory_barrier.image = this->_image_view.image;
						_dst_image_memory_barrier.subresourceRange = _mip_sub_range;

						w_graphics_device_manager::set_src_dst_masks_of_image_barrier(_dst_image_memory_barrier);

						vkCmdPipelineBarrier(
							_cmd.handle,
							VK_PIPELINE_STAGE_HOST_BIT,
							VK_PIPELINE_STAGE_TRANSFER_BIT,
							0,
							0, nullptr,
							0, nullptr,
							1,
							&_dst_image_memory_barrier);

					}

					VkImageSubresourceRange _mip_maps_subresource_range =
					{
						this->_buffer_type,                             // AspectMask
						0,                                              // BaseMipLevel
						this->_mip_map_levels,                          // LevelCount
						0,                                              // BaseArrayLayer
						this->_layer_count                              // LayerCount
					};

					//Now all mip layers are in TRANSFER_SRC layout, so transition all to SHADER_READ
					VkImageMemoryBarrier _image_memory_barrier = {};
					_image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
					_image_memory_barrier.pNext = nullptr;
					_image_memory_barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
					_image_memory_barrier.newLayout = this->_image_layout;
					_image_memory_barrier.image = this->_image_view.image;
					_image_memory_barrier.subresourceRange = _mip_maps_subresource_range;

					w_graphics_device_manager::set_src_dst_masks_of_image_barrier(_image_memory_barrier);

					vkCmdPipelineBarrier(
						_cmd.handle,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
						VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
						0,
						0, nullptr,
						0, nullptr,
						1,
						&_image_memory_barrier);


					_command_buffer.end(0);

					auto _hr = this->_gDevice->submit(
						{ &_cmd },
						this->_gDevice->vk_graphics_queue,
						{},
						{},
						{},
						nullptr,
						true);
					//release command buffer
					_command_buffer.release();

					if (_hr == W_FAILED)
					{
						V(W_FAILED,
							"submit commad buffer for generating mipmaps for graphics device" + this->_gDevice->device_info->get_device_name() +
							" ID:" + std::to_string(this->_gDevice->device_info->get_device_id()),
							_trace_info,
							3,
							false);
						return W_FAILED;
					}
				}
                else
                {
                    //do it with cpu(ffmpeg) or render targets
                }
                
                return W_PASSED;
            }
            
			void* read_data_from_texture()
			{
				if (this->_is_staging)
				{
					return this->_staging_buffer_memory_pointer;
				}

				auto _data_size = this->_image_view.width * this->_image_view.height * 4;
				
				auto _hResult = this->_staging_buffer.allocate_as_staging(this->_gDevice, _data_size);
				if (_hResult == W_FAILED)
				{
					return nullptr;
				}

				//_hResult = this->_staging_buffer.bind();
				//if (_hResult == W_FAILED)
				//{
				//	return nullptr;
				//}

				//auto _hr = vkMapMemory(
				//	this->_gDevice->vk_device,
				//	this->_staging_buffer.get_memory(),
				//	0,
				//	_data_size,
				//	0,
				//	&this->_staging_buffer_memory_pointer);

				//if (_hr)
				//{
				//	V(W_FAILED, "Could not map memory and upload texture data to a staging buffer on graphics device: " +
				//		this->_gDevice->device_name + " ID: " + std::to_string(this->_gDevice->device_id),
				//		this->_name,
				//		3,
				//		false);
				//	return W_FAILED;
				//}

				//VkMappedMemoryRange _flush_range =
				//{
				//	VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,              // Type
				//	nullptr,                                            // Next
				//	this->_staging_buffer.get_memory(),                       // Memory
				//	0,                                                  // Offset
				//	_data_size                                          // Size
				//};

				//vkFlushMappedMemoryRanges(
				//	this->_gDevice->vk_device,
				//	1,
				//	&_flush_range);
				//vkUnmapMemory(this->_gDevice->vk_device,
				//	this->_staging_buffer.get_memory());


				//create command buffer
				w_command_buffers _command_buffer;
				_command_buffer.load(this->_gDevice, 1);
				auto _cmd = _command_buffer.get_command_at(0);

				_command_buffer.begin(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
				{
					VkImageSubresourceRange _image_subresource_range =
					{
						this->_buffer_type,								// AspectMask
						0,                                              // BaseMipLevel
						1,                                              // LevelCount
						0,                                              // BaseArrayLayer
						this->_layer_count                              // LayerCount
					};

					VkImageMemoryBarrier _image_memory_barrier_from_undefined_to_transfer =
					{
						VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,         // Type
						nullptr,                                        // Next
						VK_IMAGE_LAYOUT_UNDEFINED,						// SrcAccessMask
						VK_ACCESS_TRANSFER_WRITE_BIT,                   // DstAccessMask
						VK_IMAGE_LAYOUT_UNDEFINED,						// OldLayout
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,			// NewLayout
						VK_QUEUE_FAMILY_IGNORED,                        // SrcQueueFamilyIndex
						VK_QUEUE_FAMILY_IGNORED,                        // DstQueueFamilyIndex
						this->_image_view.image,                        // image
						_image_subresource_range                        // SubresourceRange
					};
					vkCmdPipelineBarrier(_cmd.handle,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&_image_memory_barrier_from_undefined_to_transfer);

					VkImageMemoryBarrier _image_memory_barrier_from_memory_to_transfer =
					{
						VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,             // Type
						nullptr,                                            // Next
						VK_ACCESS_MEMORY_READ_BIT,							// SrcAccessMask
						VK_ACCESS_TRANSFER_READ_BIT,                        // DstAccessMask
						VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,					// OldLayout
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,				// NewLayout
						VK_QUEUE_FAMILY_IGNORED,                            // SrcQueueFamilyIndex
						VK_QUEUE_FAMILY_IGNORED,                            // DstQueueFamilyIndex
						this->_image_view.image,                            // Image
						_image_subresource_range                            // SubresourceRange
					};
					vkCmdPipelineBarrier(_cmd.handle,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&_image_memory_barrier_from_memory_to_transfer);

					VkBufferImageCopy _buffer_image_copy_info =
					{
						0,                                    // BufferOffset
						0,                                    // BufferRowLength
						0,                                    // BufferImageHeight
						{                                     // ImageSubresource
							this->_buffer_type,				  // AspectMask
							0,                                // MipLevel
							0,                                // BaseArrayLayer
							this->_layer_count                // LayerCount
						},
						{                                     // ImageOffset
							0,                                // X
							0,                                // Y
							0                                 // Z
						},
						{                                     // ImageExtent
							this->_image_view.width,          // Width
							this->_image_view.height,         // Height
							1                                 // Depth
						}
					};

					auto _staging_buffer_handle = this->_staging_buffer.get_buffer_handle();
					// execute copy command
					vkCmdCopyImageToBuffer(
						_cmd.handle,
						this->_image_view.image,
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						_staging_buffer_handle.handle,
						1,
						&_buffer_image_copy_info);

					this->_staging_buffer.flush();
				}
				_command_buffer.end(0);

				// Submit command buffer and copy data from staging buffer to a vertex buffer
				VkSubmitInfo _submit_info =
				{
					VK_STRUCTURE_TYPE_SUBMIT_INFO,        // Type
					nullptr,                              // Next
					0,                                    // WaitSemaphoreCount
					nullptr,                              // WaitSemaphores
					nullptr,                              // WaitDstStageMask;
					1,                                    // CommandBufferCount
					&_cmd.handle,                         // CommandBuffers
					0,                                    // SignalSemaphoreCount
					nullptr                               // SignalSemaphores
				};

				auto _hr = vkQueueSubmit(this->_gDevice->vk_graphics_queue.queue, 1, &_submit_info, VK_NULL_HANDLE);
				if (_hr)
				{
					V(W_FAILED, "Could submit map memory and upload texture data to a staging buffer on graphics device: " +
						this->_gDevice->device_info->get_device_name() + " ID: " + std::to_string(this->_gDevice->device_info->get_device_id()),
						this->_name,
						3,
						false);

					_command_buffer.release();
					return nullptr;
				}
				vkDeviceWaitIdle(this->_gDevice->vk_device);

				auto _ptr = (uint8_t*)this->_staging_buffer.map();
				for (size_t i = 0; i < this->_image_view.width * this->_image_view.height; i++)
				{
					logger.write(std::to_string(_ptr[i]));
				}

				this->_staging_buffer.unmap();

				//release command buffer
				_command_buffer.release();

				this->_staging_buffer.release();
				
				return nullptr;
			}

            W_RESULT flush_staging_data()
            {
                if (!this->_is_staging) return W_FAILED;
                
                //create command buffer
                w_command_buffers _command_buffer;
                _command_buffer.load(this->_gDevice, 1);
                auto _cmd = _command_buffer.get_command_at(0);

                _command_buffer.begin(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
                {
                    VkImageSubresourceRange _image_subresource_range =
                    {
                        this->_buffer_type,							    // AspectMask
                        0,                                              // BaseMipLevel
                        1,                                              // LevelCount
                        0,                                              // BaseArrayLayer
                        this->_layer_count                              // LayerCount
                    };

                    VkImageMemoryBarrier _image_memory_barrier_from_undefined_to_transfer_dst =
                    {
                        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,         // Type
                        nullptr,                                        // Next
                        0,                                              // SrcAccessMask
                        VK_ACCESS_TRANSFER_WRITE_BIT,                   // DstAccessMask
                        VK_IMAGE_LAYOUT_UNDEFINED,                      // OldLayout
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,           // NewLayout
                        VK_QUEUE_FAMILY_IGNORED,                        // SrcQueueFamilyIndex
                        VK_QUEUE_FAMILY_IGNORED,                        // DstQueueFamilyIndex
						this->_image_view.image,                        // image
                        _image_subresource_range                        // SubresourceRange
                    };

                    vkCmdPipelineBarrier(_cmd.handle,
                        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        0,
                        0,
                        nullptr,
                        0,
                        nullptr,
                        1,
                        &_image_memory_barrier_from_undefined_to_transfer_dst);

                    VkBufferImageCopy _buffer_image_copy_info =
                    {
                        0,                                    // BufferOffset
                        0,                                    // BufferRowLength
                        0,                                    // BufferImageHeight
                        {                                     // ImageSubresource
                            this->_buffer_type,				  // AspectMask
                            0,                                // MipLevel
                            0,                                // BaseArrayLayer
                            this->_layer_count                // LayerCount
                        },
                        {                                     // ImageOffset
                            0,                                // X
                            0,                                // Y
                            0                                 // Z
                        },
                        {                                     // ImageExtent
                            this->_image_view.width,          // Width
                            this->_image_view.height,         // Height
                            1                                 // Depth
                        }
                    };

					auto _staging_buffer_handle = this->_staging_buffer.get_buffer_handle();
                    vkCmdCopyBufferToImage(_cmd.handle,
						_staging_buffer_handle.handle,
						this->_image_view.image,
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                        1,
                        &_buffer_image_copy_info);

                    VkImageMemoryBarrier _image_memory_barrier_from_transfer_to_shader_read =
                    {
                        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,             // Type
                        nullptr,                                            // Next
                        VK_ACCESS_TRANSFER_WRITE_BIT,                       // SrcAccessMask
                        VK_ACCESS_SHADER_READ_BIT,                          // DstAccessMask
                        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,               // OldLayout
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,           // NewLayout
                        VK_QUEUE_FAMILY_IGNORED,                            // SrcQueueFamilyIndex
                        VK_QUEUE_FAMILY_IGNORED,                            // DstQueueFamilyIndex
                        this->_image_view.image,                            // Image
                        _image_subresource_range                            // SubresourceRange
                    };
                    vkCmdPipelineBarrier(_cmd.handle,
                        VK_PIPELINE_STAGE_TRANSFER_BIT,
                        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
                        0,
                        0,
                        nullptr,
                        0,
                        nullptr,
                        1,
                        &_image_memory_barrier_from_transfer_to_shader_read);

                }
                _command_buffer.end(0);

                // Submit command buffer and copy data from staging buffer to a vertex buffer
                VkSubmitInfo _submit_info =
                {
                    VK_STRUCTURE_TYPE_SUBMIT_INFO,        // Type
                    nullptr,                              // Next
                    0,                                    // WaitSemaphoreCount
                    nullptr,                              // WaitSemaphores
                    nullptr,                              // WaitDstStageMask;
                    1,                                    // CommandBufferCount
                    &_cmd.handle,                           // CommandBuffers
                    0,                                    // SignalSemaphoreCount
                    nullptr                               // SignalSemaphores
                };

                auto _vhr = vkQueueSubmit(this->_gDevice->vk_graphics_queue.queue, 1, &_submit_info, VK_NULL_HANDLE);
                if (_vhr)
                {
                    V(W_FAILED, "Could submit map memory and upload texture data to a staging buffer on graphics device: " +
                        this->_gDevice->device_info->get_device_name() + " ID: " + std::to_string(this->_gDevice->device_info->get_device_id()),
                        this->_name,
                        3,
                        false);

                    _command_buffer.release();
                    return W_FAILED;
                }
                vkDeviceWaitIdle(this->_gDevice->vk_device);

                //release command buffer
                _command_buffer.release();

                if (!this->_is_staging)
                {
                    //release staging buffer
                    this->_staging_buffer.release();
                }

                return W_PASSED;
            }
            
            ULONG release()
            {
                //release sampler
				for (auto _iter : this->_samplers)
				{
					auto _sampler = _iter.second;
					if (_sampler.handle)
					{
						vkDestroySampler(this->_gDevice->vk_device,
							_sampler.handle,
							nullptr);
						_sampler.handle = nullptr;
					}
				}
				this->_samplers.clear();
                
                //release view
				if (this->_image_view.view)
				{
					vkDestroyImageView(
						this->_gDevice->vk_device,
						this->_image_view.view,
						nullptr);
					this->_image_view.view = 0;
				}
                
                //release image
				if (this->_image_view.image)
				{
					vkDestroyImage(
						this->_gDevice->vk_device,
						this->_image_view.image,
						nullptr);
					this->_image_view.image = 0;
				}
                
                //release memory
                if( this->_memory.handle)
                {
                    vkFreeMemory(this->_gDevice->vk_device,
                                 this->_memory.handle,
                                 nullptr);
                    this->_memory.handle = 0;
                }
                
                if (this->_is_staging)
                {
                    //release staging buffer
                    this->_staging_buffer.release();
                }

                this->_staging_buffer_memory_pointer = nullptr;
                this->_gDevice = nullptr;
                
                return 0;
            }
                 
#pragma region Getters
            
			const w_descriptor_image_info get_descriptor_info(_In_ const w_sampler_type& pSamplerType) const
			{
				auto _sampler = get_sampler(pSamplerType);
				
				w_descriptor_image_info  _desc_image_info = {};

				_desc_image_info.imageView = this->_image_view.view;
				_desc_image_info.imageLayout = this->_image_layout;
				_desc_image_info.sampler = _sampler.handle;

				return _desc_image_info;
			}

            const uint32_t get_width() const
            {
                return this->_image_view.width;
            }
            
            const uint32_t get_height() const
            {
                return this->_image_view.height;
            }
            
			const uint32_t get_usage_flags() const
			{
				return this->_usage_flags;
			}

            w_sampler get_sampler(_In_ w_sampler_type pSamplerType) const
            {
				auto _find = this->_samplers.find(pSamplerType);
				if (_find != this->_samplers.end())
				{
					return _find->second;
				}
                return w_sampler();
            }
                       
            w_image_view get_image_view() const
            {
                return this->_image_view;
            }
            
			w_image_type get_image_type() const
            {
                return this->_image_type;
            }
            
            w_image_view_type get_image_view_type() const
            {
                return this->_image_view_type;
            }
            
            w_format get_format() const
            {
                return (w_format)this->_image_view.attachment_desc.desc.format;
            }
            
			const uint32_t get_mip_maps_level() const
			{
				return this->_mip_map_levels;
			}

			const wchar_t* get_texture_name() const
			{
				return this->_texture_name.c_str();
			}

#pragma endregion
              
#pragma region Setters

			void set_format(_In_ w_format pFormat)
			{
				this->_image_view.attachment_desc.desc.format = (VkFormat)pFormat;
			}

			void set_buffer_type(_In_ w_texture_buffer_type pBufferType)
			{
#ifdef __VULKAN__
				this->_buffer_type = (VkImageAspectFlagBits)pBufferType;
#else


#endif
			}

			void set_view_type(_In_ w_image_view_type pViewType)
			{
				this->_image_view_type = pViewType;
			}

			void set_usage_flags(_In_ uint32_t pUsageFlags)
			{
				this->_usage_flags = pUsageFlags;
			}

#pragma endregion

        private:
            
            std::string                                     _name;
            std::shared_ptr<w_graphics_device>              _gDevice;
			uint32_t										_usage_flags;
            uint32_t                                        _layer_count;
            uint32_t                                        _mip_map_levels;
            bool                                            _generate_mip_maps;
            uint32_t										_memory_property_flags;
            bool                                            _is_staging;
            void*                                           _staging_buffer_memory_pointer;
            w_buffer                                        _staging_buffer;
			w_image_view									_image_view;
			VkImageAspectFlags								_buffer_type;
			std::map<w_sampler_type, w_sampler>				_samplers;
            w_device_memory                                 _memory;
			w_image_type                                    _image_type;
            w_image_view_type                               _image_view_type;
			VkImageLayout									_image_layout;
			std::wstring									_texture_name;
			bool											_just_initialized;
        };
    }
}

using namespace wolf::graphics;

std::map<std::wstring, w_texture*> w_texture::_shared;
w_texture* w_texture::default_texture = nullptr;

w_texture::w_texture() : 
    _pimp(new w_texture_pimp())
{
	_super::set_class_name("w_texture");
}

w_texture::~w_texture()
{
	release();
}

W_RESULT w_texture::initialize(
	_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const uint32_t& pWidth,
	_In_ const uint32_t& pHeight,
	_In_ const bool& pGenerateMipMapsLevels,
	_In_ const bool& pHasStagingBuffer,
	_In_ const uint32_t pMemoryPropertyFlags)
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->initialize(
		pGDevice, 
		pWidth, 
		pHeight, 
		pGenerateMipMapsLevels, 
		pHasStagingBuffer,
		pMemoryPropertyFlags);
}

W_RESULT w_texture::load()
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->load();
}

W_RESULT w_texture::load_texture_2D_from_file(_In_z_ std::wstring pPath, _In_ bool pIsAbsolutePath)
{
    if(!this->_pimp) return W_FAILED;
    return this->_pimp->load_texture_2D_from_file(pPath, pIsAbsolutePath);
}

W_RESULT w_texture::load_texture_from_memory_rgba(_In_ uint8_t* pRGBAData)
{
    if (!this->_pimp) return W_FAILED;
    return this->_pimp->load_texture_from_memory_rgba(pRGBAData);
}

W_RESULT w_texture::load_texture_from_memory_rgb(_In_ uint8_t* pRGBData)
{
    if (!this->_pimp) return W_FAILED;
    
    const auto _width = this->_pimp->get_width();
    const auto _height = this->_pimp->get_height();

    if (_width == 0 || _height == 0) return W_FAILED;
    
    auto _rgba = (uint8_t*)malloc(_width * _height * 4);
    if (!_rgba) return W_FAILED;
    
    int _count = (int)(_width * _height);
    for (int i = _count; --i; _rgba += 4, pRGBData += 3) 
    {
        *(uint32_t*)(void*)_rgba = *(const uint32_t*)(const void*)pRGBData;
    }
    for (int j = 0; j < 3; ++j)
    {
        _rgba[j] = pRGBData[j];
    }

    return this->_pimp->load_texture_from_memory_rgba(_rgba);
}

W_RESULT w_texture::load_texture_from_memory_all_channels_same(_In_ uint8_t pData)
{
    if (!this->_pimp) return W_FAILED;

    const auto _width = this->_pimp->get_width();
    const auto _height = this->_pimp->get_height();

    if (_width == 0 || _height == 0) return W_FAILED;

    auto _length = _width * _height * 4;
    auto _rgba = (uint8_t*)malloc(_length * sizeof(uint8_t));
    if (!_rgba) return W_FAILED;

    std::memset(_rgba, pData, _length);
    auto _hr = this->_pimp->load_texture_from_memory_rgba(_rgba);
    free(_rgba);
    return _hr;
}

W_RESULT w_texture::load_texture_from_memory_color(_In_ w_color pColor)
{
    if (!this->_pimp) return W_FAILED;

    const auto _width = this->_pimp->get_width();
    const auto _height = this->_pimp->get_height();

    if (_width == 0 || _height == 0) return W_FAILED;

    auto _length = _width * _height * 4;
    auto _rgba = (uint8_t*)malloc(_length * sizeof(uint8_t));
    if (!_rgba) return W_FAILED;

    for (size_t i = 0; i < _length; i += 4)
    {
        _rgba[i] = pColor.r;
        _rgba[i + 1] = pColor.g;
        _rgba[i + 2] = pColor.b;
        _rgba[i + 3] = pColor.a;
    }
    auto _hr = this->_pimp->load_texture_from_memory_rgba(_rgba);
    free(_rgba);
    return _hr;
}

W_RESULT w_texture::copy_data_to_texture_2D(_In_ const uint8_t* pRGBA)
{
    if (!this->_pimp) return W_FAILED;
    return this->_pimp->copy_data_to_texture_2D(pRGBA);
}

void* w_texture::read_data_of_texture()
{
	if (!this->_pimp) return nullptr;
	return this->_pimp->read_data_from_texture();
}

W_RESULT w_texture::flush_staging_data()
{
    if (!this->_pimp) return W_FAILED;
    return this->_pimp->flush_staging_data();
}

W_RESULT w_texture::load_to_shared_textures(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_z_ const std::wstring& pPath,
	_In_z_ const bool& pGenerateMipMaps,
    _Inout_ w_texture** pPointerToTexture)
{
    //check if already exists
    auto _iter = _shared.find(pPath);
    if (_iter != _shared.end())
    {
        *pPointerToTexture = _shared[pPath];
        return W_PASSED;
    }

    auto _texture = new (std::nothrow) w_texture();
    if (!_texture)
    {
        logger.error(L"Could not perform allocation for shared texture: " + pPath);
        return W_FAILED;
    }

    auto _hr = _texture->initialize(pGDevice, 32, 32, pGenerateMipMaps);
    if (_hr == W_FAILED)
    {
        SAFE_RELEASE(_texture);
        return W_FAILED;
    }
    _hr = _texture->load_texture_2D_from_file(pPath, true);
    if (_hr == W_FAILED)
    {
        SAFE_RELEASE(_texture);
        return W_FAILED;
    }

    _shared[pPath] = _texture;
    *pPointerToTexture = _texture;

    return W_PASSED;
}

//void w_texture::write_bitmap_to_file(
//    _In_z_ const char* pFilename,
//    _In_ const uint8_t* pData,
//    _In_ const int& pWidth, const int& pHeight)
//{
//    short header[] = { 0x4D42, 0, 0, 0, 0, 26, 0, 12, 0, (short)pWidth, (short)pHeight, 1, 24 };
//    FILE *f;
//    
//#ifdef __WIN32
//    if (!fopen_s(&f, pFilename, "wb"))
//#else
//    f = fopen(pFilename, "wb");
//    if (!f)
//#endif
//    {
//        fwrite(header, 1, sizeof(header), f);
//        fwrite(pData, 1, pWidth * pHeight * 3, f);
//        fclose(f);
//    }
//}

W_RESULT w_texture::save_png_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount, _In_ int pStrideInBytes)
{
	return stbi_write_png(pFilePath, static_cast<int>(pWidth), static_cast<int>(pHeight), pCompCount, pData, pStrideInBytes) == 1 ? W_PASSED : W_FAILED;
}

W_RESULT w_texture::save_bmp_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount)
{
	return stbi_write_bmp(pFilePath, static_cast<int>(pWidth), static_cast<int>(pHeight), pCompCount, pData) == 1 ? W_PASSED : W_FAILED;
}

W_RESULT w_texture::save_tga_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount)
{
	return stbi_write_tga(pFilePath, static_cast<int>(pWidth), static_cast<int>(pHeight), pCompCount, pData) == 1 ? W_PASSED : W_FAILED;
}

W_RESULT w_texture::save_hdr_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const float* pData, _In_ int pCompCount)
{
	return stbi_write_hdr(pFilePath, static_cast<int>(pWidth), static_cast<int>(pHeight), pCompCount, pData) == 1 ? W_PASSED : W_FAILED;
}

W_RESULT w_texture::save_jpg_to_file(_In_z_ const char* pFilePath, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount, _In_ int pQuality)
{
	return stbi_write_jpg(pFilePath, static_cast<int>(pWidth), static_cast<int>(pHeight), pCompCount, pData, pQuality) == 1 ? W_PASSED : W_FAILED;
}

ULONG w_texture::release()
{
	if (_super::get_is_released()) return 1;
    
    SAFE_RELEASE(this->_pimp);
    
	return _super::release();
}

ULONG w_texture::release_shared_textures()
{
    if (!_shared.size()) return 1;

	SAFE_RELEASE(default_texture);
    for (auto _pair : _shared)
    {
        SAFE_RELEASE(_pair.second);
    }
    _shared.clear();

    return 0;
}

#pragma region Getters

const uint32_t w_texture::get_width() const
{
    if(!this->_pimp) return 0;
    return this->_pimp->get_width();
}

const uint32_t w_texture::get_height() const
{
    if(!this->_pimp) return 0;
    return this->_pimp->get_height();
}

const uint32_t w_texture::get_usage_flags() const
{
	if (!this->_pimp) return 0;
	return this->_pimp->get_usage_flags();
}

w_sampler w_texture::get_sampler(_In_ const w_sampler_type& pSamplerType) const
{
	if(!this->_pimp) return w_sampler();
    return this->_pimp->get_sampler(pSamplerType);
}

//get image and view resources
w_image_view w_texture::get_image_view() const
{
	if (!this->_pimp) return w_image_view();
	return this->_pimp->get_image_view();
}

//get image type
w_image_type w_texture::get_image_type() const
{
    if(!this->_pimp) return w_image_type::IMAGE_TYPE_END_RANGE;
    return this->_pimp->get_image_type();
}

//get image view type
w_image_view_type w_texture::get_image_view_type() const
{
    if(!this->_pimp) return w_image_view_type::_1D;
    return this->_pimp->get_image_view_type();
}

//get image format
w_format w_texture::get_format() const
{
	if (!this->_pimp) return w_format::UNDEFINED;
	return this->_pimp->get_format();
}

const w_descriptor_image_info w_texture::get_descriptor_info(_In_ const w_sampler_type& pSamplerType) const
{
	if (!this->_pimp)
	{
		w_descriptor_image_info _image_info;
		_image_info.imageView = 0;
		_image_info.sampler = 0;
		return _image_info;
	}
    return this->_pimp->get_descriptor_info(pSamplerType);
}

const uint32_t w_texture::get_mip_maps_level() const
{
	if (!this->_pimp) return 0;
	return this->_pimp->get_mip_maps_level();
}

#pragma endregion

#pragma region Setters

void w_texture::set_format(_In_ w_format pFormat)
{
	if (!this->_pimp) return;
	return this->_pimp->set_format(pFormat);
}

void w_texture::set_usage_flags(_In_ uint32_t pUsageFlags)
{
	if (!this->_pimp) return;
	return this->_pimp->set_usage_flags(pUsageFlags);
}

void w_texture::set_buffer_type(_In_ w_texture_buffer_type pBufferType)
{
	if (!this->_pimp) return;
	return this->_pimp->set_buffer_type(pBufferType);
}

void w_texture::set_view_type(_In_ w_image_view_type pViewType)
{
	if (!this->_pimp) return;
	return this->_pimp->set_view_type(pViewType);
}

#pragma endregion

