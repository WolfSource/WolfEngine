#include "w_render_pch.h"
#include "w_texture.h"
#include <w_convert.h>
#include <w_io.h>
#include "w_buffer.h"
#include "w_command_buffer.h"

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
				_width(0),
				_height(0),
				_layer_count(1),
				_sampler(0),
				_memory(0),
				_usage(VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
				_is_staging(false),
				_staging_buffer_memory_pointer(nullptr),
				_format(VK_FORMAT_R8G8B8A8_UNORM),
				_image_type(VkImageType::VK_IMAGE_TYPE_2D),
				_image_view_type(VkImageViewType::VK_IMAGE_VIEW_TYPE_2D),
				_buffer_type(VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT)
			{
			}

            ~w_texture_pimp()
            {
                release();
            }

            HRESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const uint32_t pWidth, 
				_In_ const uint32_t pHeight, 
                _In_ const VkMemoryPropertyFlags pMemoryPropertyFlags)
            {
                this->_gDevice = pGDevice;
                this->_memory_property_flags = pMemoryPropertyFlags;
                this->_width = pWidth;
                this->_height = pHeight;
                
                if (pMemoryPropertyFlags & VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT ||
                    pMemoryPropertyFlags & VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
                {
                    this->_is_staging = true;
                }
                return S_OK;
            }

			HRESULT load()
			{
				auto _hr = _create_image();
				if (_hr == S_FALSE) return S_FALSE;

				_hr = _allocate_memory();
				if (_hr == S_FALSE) return S_FALSE;

				//bind to memory
				if (vkBindImageMemory(this->_gDevice->vk_device,
					this->_image_view.image,
					this->_memory,
					0))
				{
					V(S_FALSE, "binding VkImage for graphics device: " + this->_gDevice->device_name +
						" ID: " + std::to_string(this->_gDevice->device_id), this->_name, 3, false);
					return S_FALSE;
				}

				_hr = _create_image_view();
				if (_hr == S_FALSE) return S_FALSE;

				_hr = _create_sampler();
				if (_hr == S_FALSE) return S_FALSE;
				
				_create_descriptor_image_info();

				return S_OK;
			}

			HRESULT load_texture_2D_from_file(_In_z_ std::wstring pPath, _In_ bool pIsAbsolutePath)
			{
				using namespace std;
				using namespace system::io;

				std::wstring _path = pPath;
				if (!pIsAbsolutePath)
				{
					_path = content_path + pPath;
				}
				auto _ext = get_file_extentionW(_path.c_str());

#if defined(__WIN32) || defined(__UWP)
				std::wstring _str = _path;
#else
				std::string _str = wolf::system::convert::wstring_to_string(_path);
#endif

				if (S_FALSE == system::io::get_is_file(_str.c_str()))
				{
					wstring msg = L"could not find the texture file: ";
					V(S_FALSE, msg + _path, this->_name, 3);
					return S_FALSE;
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

						this->_width = _gli_tex_2D_array->extent().x;
						this->_height = _gli_tex_2D_array->extent().y;
						this->_layer_count = _gli_tex_2D_array->layers();
						this->_format = _gli_format_to_vulkan_format(_gli_tex_2D_array->format());
					}

					_g_path.clear();
				}
				else if (_ext == L".jpg" || _ext == L".bmp" || _ext == L".png" || _ext == L".psd" || _ext == L".tga")
				{
					//we re loading file with stbi header

					int __width, __height, __comp;

					_rgba = stbi_load(_g_path.c_str(), &__width, &__height, &__comp, STBI_rgb_alpha);

					this->_width = __width;
					this->_height = __height;
					this->_layer_count = 1;

					_g_path.clear();
				}
				else
				{
					logger.error(L"Format not supported yet for following file: " + _path);
					return S_FALSE;
				}
				
				if (this->_width == 0 || this->_height == 0)
				{
					wstring msg = L"Width or Height of texture file is zero: ";
					V(S_FALSE, msg + _path, this->_name, 3);
					if (_rgba) free(_rgba);
					release();
					return S_FALSE;
				}

				auto _hr = _create_image();
				if (_hr == S_FALSE)
				{
					if (_rgba) free(_rgba);
					release();
					return S_FALSE;
				}

				_hr = _allocate_memory();
				if (_hr == S_FALSE)
				{
					if (_rgba) free(_rgba);
					release();
					return S_FALSE;
				}

				//bind to memory
				if (vkBindImageMemory(this->_gDevice->vk_device,
					this->_image_view.image,
					this->_memory,
					0))
				{
					V(S_FALSE, "binding VkImage for graphics device: " + this->_gDevice->device_name +
						" ID: " + std::to_string(this->_gDevice->device_id), this->_name, 3, false);
					return S_FALSE;
				}

				_hr = _create_image_view();
				if (_hr == S_FALSE) return S_FALSE;

				_hr = _create_sampler();
				if (_hr == S_FALSE) return S_FALSE;


				//copy data to texture
				if (_rgba)
				{
					_hr = copy_data_to_texture_2D(_rgba);
					if (_hr == S_FALSE) return S_FALSE;

					free(_rgba);
				}
				else if (_gli_tex_2D_array)
				{
					_hr = copy_data_to_texture_2D_array(*_gli_tex_2D_array);
					if (_hr == S_FALSE) return S_FALSE;
					SAFE_DELETE(_gli_tex_2D_array);
				}
				else
				{
					wstring msg = L"texture file is corrupted: ";
					V(S_FALSE, msg + _path, this->_name, 3);
					release();
					return S_FALSE;
				}

				_create_descriptor_image_info();

				return S_OK;
			}

            HRESULT load_texture_from_memory_rgba(_In_ uint8_t* pRGBAData)
            {
                auto _hr = _create_image();
                if (_hr == S_FALSE) return S_FALSE;

                _hr = _allocate_memory();
                if (_hr == S_FALSE) return S_FALSE;

                //bind to memory
				if (vkBindImageMemory(this->_gDevice->vk_device,
					this->_image_view.image,
					this->_memory,
					0))
				{
					V(S_FALSE, "binding VkImage for graphics device: " + this->_gDevice->device_name +
						" ID: " + std::to_string(this->_gDevice->device_id), this->_name, 3, false);
					return S_FALSE;
				}

                _hr = _create_image_view();
                if (_hr == S_FALSE) return S_FALSE;

                _hr = _create_sampler();
                if (_hr == S_FALSE) return S_FALSE;
                
                _hr = copy_data_to_texture_2D(pRGBAData);
                if (_hr == S_FALSE) return S_FALSE;
                
				_create_descriptor_image_info();

                return S_OK;
            }

			HRESULT _create_image()
			{
				const VkImageCreateInfo _image_create_info =
				{
					VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,							// Type;
					nullptr,														// Next
					0,																// Flags
					this->_image_type,												// ImageType
					this->_format,													// Format
					{																// Extent
						this->_width,												// Width
						this->_height,												// Height
						1															// Depth
					},
					1,																// MipLevels
					this->_layer_count,												// ArrayLayers
					VK_SAMPLE_COUNT_1_BIT,											// Samples
					VK_IMAGE_TILING_OPTIMAL,										// Tiling
					this->_usage,													// Usage
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
					V(S_FALSE, "creating VkImage for graphics device: " + this->_gDevice->device_name +
						" ID: " + std::to_string(this->_gDevice->device_id), this->_name, 3, false);
					return S_FALSE;
				}

				return S_OK;
			}
            
            HRESULT _allocate_memory()
            {
                //release the old one
                if (this->_memory)
                {
                    vkFreeMemory(this->_gDevice->vk_device, this->_memory, nullptr);
                }
                
                VkMemoryRequirements _image_memory_requirements;
                vkGetImageMemoryRequirements(
					this->_gDevice->vk_device,
					this->_image_view.image,
                    &_image_memory_requirements);
                
                VkPhysicalDeviceMemoryProperties _memory_properties;
                vkGetPhysicalDeviceMemoryProperties( this->_gDevice->vk_physical_device, &_memory_properties);
                
                bool _found = false;
                for( uint32_t i = 0; i < _memory_properties.memoryTypeCount; ++i )
                {
                    if( (_image_memory_requirements.memoryTypeBits & (1 << i)) &&
                       (_memory_properties.memoryTypes[i].propertyFlags & this->_memory_property_flags) )
                    {
                        
                        const VkMemoryAllocateInfo _memory_allocate_info =
                        {
                            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,     // Type
                            nullptr,                                    // Next
                            _image_memory_requirements.size,            // AllocationSize
                            i                                           // MemoryTypeIndex
                        };
                        
                        if( vkAllocateMemory(this->_gDevice->vk_device,
                                             &_memory_allocate_info,
                                             nullptr,
                                             &this->_memory) == VK_SUCCESS )
                        {
                            _found = true;
                            break;
                        }
                    }
                }

                if(!_found)
                {
                    logger.error("Could not allocate memory for image");
                    return S_FALSE;
                }
                
                return S_OK;
            }
            
			HRESULT _create_image_view()
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
					this->_image_view_type,                               // ViewType
					this->_format,                                        // Format
					{                                                     // Components
						VK_COMPONENT_SWIZZLE_R,                                 // VkComponentSwizzle         r
						VK_COMPONENT_SWIZZLE_G,                                 // VkComponentSwizzle         g
						VK_COMPONENT_SWIZZLE_B,                                 // VkComponentSwizzle         b
						VK_COMPONENT_SWIZZLE_A                                  // VkComponentSwizzle         a
					},
					{                                                     // SubresourceRange
						this->_buffer_type,										// aspectMask
						0,                                                      // baseMipLevel
						1,                                                      // levelCount
						0,                                                      // baseArrayLayer
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
					V(S_FALSE, "creating image view", this->_name, false, true);
					return S_FALSE;
				}

				return S_OK;
			}
            
			HRESULT _create_sampler()
			{
				const VkSamplerCreateInfo _sampler_create_info =
				{
					VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,                // Type
					nullptr,                                              // Next
					0,                                                    // Flags
					VK_FILTER_LINEAR,                                     // MagFilter
					VK_FILTER_LINEAR,                                     // MinFilter
					VK_SAMPLER_MIPMAP_MODE_LINEAR,                        // MipmapMode
					VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,              // AddressModeU
					VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,              // AddressModeV
					VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,              // AddressModeW
					0.0f,                                                 // MipLodBias
					VK_TRUE,                                              // AnisotropyEnable
					8.0f,                                                 // MaxAnisotropy
					VK_FALSE,                                             // CompareEnable
					VK_COMPARE_OP_NEVER,                                  // CompareOp
					0.0f,                                                 // MinLod
					0.0f,                                                 // MaxLod
					VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,                   // BorderColor
					VK_FALSE                                              // UnnormalizedCoordinates
				};

				auto _hr = vkCreateSampler(
					this->_gDevice->vk_device,
					&_sampler_create_info,
					nullptr,
					&this->_sampler);
				if (_hr)
				{
					V(S_FALSE, "creating sampler", this->_name, false, true);
					return S_FALSE;
				}

				return S_OK;
			}
            
			VkFormat _gli_format_to_vulkan_format(_In_ gli::format pFormat)
			{
				//direct map to vulkan formats
				return (VkFormat)pFormat;
			}

			void _create_descriptor_image_info()
			{
				this->_desc_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				this->_desc_image_info.sampler = this->_sampler;
				this->_desc_image_info.imageView = this->_image_view.view;
			}

            HRESULT copy_data_to_texture_2D(_In_ const uint8_t* pRGBA)
            {
                auto _data_size = this->_width * this->_height * 4;

                auto _hResult = this->_staging_buffer.load_as_staging(this->_gDevice, _data_size);
                if (_hResult == S_FALSE)
                {
                    return _hResult;
                }

                _hResult = this->_staging_buffer.bind();
                if (_hResult == S_FALSE)
                {
                    return _hResult;
                }

                auto _hr = vkMapMemory(
					this->_gDevice->vk_device,
                    this->_staging_buffer.get_memory(),
                    0,
                    _data_size,
                    0,
                    &this->_staging_buffer_memory_pointer);

                if (_hr)
                {
                    V(S_FALSE, "Could not map memory and upload texture data to a staging buffer on graphics device: " +
                        this->_gDevice->device_name + " ID: " + std::to_string(this->_gDevice->device_id),
                        this->_name,
                        3,
                        false);
                    return S_FALSE;
                }

                memcpy(this->_staging_buffer_memory_pointer, pRGBA, (size_t)_data_size);

                VkMappedMemoryRange _flush_range =
                {
                    VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,              // Type
                    nullptr,                                            // Next
                    this->_staging_buffer.get_memory(),                       // Memory
                    0,                                                  // Offset
                    _data_size                                          // Size
                };

                vkFlushMappedMemoryRanges(
					this->_gDevice->vk_device,
                    1,
                    &_flush_range);
                vkUnmapMemory(this->_gDevice->vk_device,
                    this->_staging_buffer.get_memory());


                //create command buffer
                w_command_buffer _command_buffer;
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

					vkCmdPipelineBarrier(_cmd,
						VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&_image_memory_barrier_from_undefined_to_transfer_dst);

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
					vkCmdPipelineBarrier(_cmd,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&_image_memory_barrier_from_transfer_to_shader_read);

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
							this->_width,                     // Width
							this->_height,                    // Height
							1                                 // Depth
						}
					};

					vkCmdCopyBufferToImage(_cmd,
						this->_staging_buffer.get_handle(),
						this->_image_view.image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						1,
						&_buffer_image_copy_info);

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
                        &_cmd,                                // CommandBuffers
                        0,                                    // SignalSemaphoreCount
                        nullptr                               // SignalSemaphores
                };

                _hr = vkQueueSubmit(this->_gDevice->vk_graphics_queue.queue, 1, &_submit_info, VK_NULL_HANDLE);
                if (_hr)
                {
                    V(S_FALSE, "Could submit map memory and upload texture data to a staging buffer on graphics device: " +
                        this->_gDevice->device_name + " ID: " + std::to_string(this->_gDevice->device_id),
                        this->_name,
                        3,
                        false);

                    _command_buffer.release();
                    return S_FALSE;
                }
                vkDeviceWaitIdle(this->_gDevice->vk_device);

                //release command buffer
                _command_buffer.release();

                if (!this->_is_staging)
                {
                    //release staging buffer
                    this->_staging_buffer.release();
                }

                return S_OK;
            }
            
			HRESULT copy_data_to_texture_2D_array(_In_ const gli::texture2d_array& pTextureArrayRGBA)
			{				
				auto _data_size = static_cast<uint32_t>(pTextureArrayRGBA.size());

				auto _hResult = this->_staging_buffer.load_as_staging(this->_gDevice, _data_size);
				if (_hResult == S_FALSE)
				{
					return _hResult;
				}

				_hResult = this->_staging_buffer.bind();
				if (_hResult == S_FALSE)
				{
					return _hResult;
				}

				auto _hr = vkMapMemory(this->_gDevice->vk_device,
					this->_staging_buffer.get_memory(),
					0,
					_data_size,
					0,
					&this->_staging_buffer_memory_pointer);

				if (_hr)
				{
					V(S_FALSE, "Could not map memory and upload texture data to a staging buffer on graphics device: " +
						this->_gDevice->device_name + " ID: " + std::to_string(this->_gDevice->device_id),
						this->_name,
						3,
						false);
					return S_FALSE;
				}

				memcpy(this->_staging_buffer_memory_pointer, pTextureArrayRGBA.data(), (size_t)_data_size);

				VkMappedMemoryRange _flush_range =
				{
					VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,              // Type
					nullptr,                                            // Next
					this->_staging_buffer.get_memory(),                       // Memory
					0,                                                  // Offset
					_data_size                                          // Size
				};

				vkFlushMappedMemoryRanges(this->_gDevice->vk_device,
					1,
					&_flush_range);
				vkUnmapMemory(this->_gDevice->vk_device,
					this->_staging_buffer.get_memory());


				//create command buffer
				w_command_buffer _command_buffer;
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

					VkImageMemoryBarrier _image_memory_barrier_from_undefined_to_transfer_dst =
					{
						VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,         // Type
						nullptr,                                        // Next
						VK_IMAGE_LAYOUT_UNDEFINED,                      // SrcAccessMask
						VK_ACCESS_TRANSFER_WRITE_BIT,                   // DstAccessMask
						VK_IMAGE_LAYOUT_UNDEFINED,                      // OldLayout
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,           // NewLayout
						VK_QUEUE_FAMILY_IGNORED,                        // SrcQueueFamilyIndex
						VK_QUEUE_FAMILY_IGNORED,                        // DstQueueFamilyIndex
						this->_image_view.image,                        // image
						_image_subresource_range                        // SubresourceRange
					};

					vkCmdPipelineBarrier(_cmd,
						VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&_image_memory_barrier_from_undefined_to_transfer_dst);
					
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
					vkCmdPipelineBarrier(_cmd,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&_image_memory_barrier_from_transfer_to_shader_read);

					//execute copy to image
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
								this->_width,                     // Width
								this->_height,                    // Height
								1                                 // Depth
							}
						};

						_buffer_copy_regions.push_back(_buffer_image_copy_info);

						offset += pTextureArrayRGBA[i][0].size();
					}

					vkCmdCopyBufferToImage(_cmd,
						this->_staging_buffer.get_handle(),
						this->_image_view.image,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						static_cast<uint32_t>(_buffer_copy_regions.size()),
						_buffer_copy_regions.data());

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
					&_cmd,                                // CommandBuffers
					0,                                    // SignalSemaphoreCount
					nullptr                               // SignalSemaphores
				};

				_hr = vkQueueSubmit(this->_gDevice->vk_graphics_queue.queue, 1, &_submit_info, VK_NULL_HANDLE);
				if (_hr)
				{
					V(S_FALSE, "Could submit map memory and upload texture data to a staging buffer on graphics device: " +
						this->_gDevice->device_name + " ID: " + std::to_string(this->_gDevice->device_id),
						this->_name,
						3,
						false);

					_command_buffer.release();
					return S_FALSE;
				}
				vkDeviceWaitIdle(this->_gDevice->vk_device);

				//release command buffer
				_command_buffer.release();

				if (!this->_is_staging)
				{
					//release staging buffer
					this->_staging_buffer.release();
				}

				return S_OK;
			}

			HRESULT read_data_from_texture_2D(_In_ const void** pRGBA)
			{
				if (this->_is_staging)
				{
					*pRGBA = this->_staging_buffer_memory_pointer;
					return S_OK;
				}

				auto _data_size = this->_width * this->_height * 4;
				
				auto _hResult = this->_staging_buffer.load_as_staging(this->_gDevice, _data_size);
				if (_hResult == S_FALSE)
				{
					return _hResult;
				}

				_hResult = this->_staging_buffer.bind();
				if (_hResult == S_FALSE)
				{
					return _hResult;
				}

				//auto _hr = vkMapMemory(
				//	this->_gDevice->vk_device,
				//	this->_staging_buffer.get_memory(),
				//	0,
				//	_data_size,
				//	0,
				//	&this->_staging_buffer_memory_pointer);

				//if (_hr)
				//{
				//	V(S_FALSE, "Could not map memory and upload texture data to a staging buffer on graphics device: " +
				//		this->_gDevice->device_name + " ID: " + std::to_string(this->_gDevice->device_id),
				//		this->_name,
				//		3,
				//		false);
				//	return S_FALSE;
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
				w_command_buffer _command_buffer;
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

					vkCmdPipelineBarrier(_cmd,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						VK_PIPELINE_STAGE_TRANSFER_BIT,
						0,
						0,
						nullptr,
						0,
						nullptr,
						1,
						&_image_memory_barrier_from_undefined_to_transfer);

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
							this->_width,                     // Width
							this->_height,                    // Height
							1                                 // Depth
						}
					};

					vkCmdCopyImageToBuffer(_cmd,
						this->_image_view.image,
						VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
						this->_staging_buffer.get_handle(),
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
					vkCmdPipelineBarrier(_cmd,
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

				//// Submit command buffer and copy data from staging buffer to a vertex buffer
				//VkSubmitInfo _submit_info =
				//{
				//	VK_STRUCTURE_TYPE_SUBMIT_INFO,        // Type
				//	nullptr,                              // Next
				//	0,                                    // WaitSemaphoreCount
				//	nullptr,                              // WaitSemaphores
				//	nullptr,                              // WaitDstStageMask;
				//	1,                                    // CommandBufferCount
				//	&_cmd,                                // CommandBuffers
				//	0,                                    // SignalSemaphoreCount
				//	nullptr                               // SignalSemaphores
				//};

				//_hr = vkQueueSubmit(this->_gDevice->vk_graphics_queue.queue, 1, &_submit_info, VK_NULL_HANDLE);
				//if (_hr)
				//{
				//	V(S_FALSE, "Could submit map memory and upload texture data to a staging buffer on graphics device: " +
				//		this->_gDevice->device_name + " ID: " + std::to_string(this->_gDevice->device_id),
				//		this->_name,
				//		3,
				//		false);

				//	_command_buffer.release();
				//	return S_FALSE;
				//}
				//vkDeviceWaitIdle(this->_gDevice->vk_device);

				////release command buffer
				//_command_buffer.release();

				//if (!this->_is_staging)
				//{
				//	//release staging buffer
				//	this->_staging_buffer.release();
				//}

				return S_OK;
			}

            HRESULT flush_staging_data()
            {
                if (!this->_is_staging) return S_FALSE;
                
                //create command buffer
                w_command_buffer _command_buffer;
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

                    vkCmdPipelineBarrier(_cmd,
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
                            this->_width,                     // Width
                            this->_height,                    // Height
                            1                                 // Depth
                        }
                    };

                    vkCmdCopyBufferToImage(_cmd,
                        this->_staging_buffer.get_handle(),
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
                    vkCmdPipelineBarrier(_cmd,
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
                    &_cmd,                                // CommandBuffers
                    0,                                    // SignalSemaphoreCount
                    nullptr                               // SignalSemaphores
                };

                auto _vhr = vkQueueSubmit(this->_gDevice->vk_graphics_queue.queue, 1, &_submit_info, VK_NULL_HANDLE);
                if (_vhr)
                {
                    V(S_FALSE, "Could submit map memory and upload texture data to a staging buffer on graphics device: " +
                        this->_gDevice->device_name + " ID: " + std::to_string(this->_gDevice->device_id),
                        this->_name,
                        3,
                        false);

                    _command_buffer.release();
                    return S_FALSE;
                }
                vkDeviceWaitIdle(this->_gDevice->vk_device);

                //release command buffer
                _command_buffer.release();

                if (!this->_is_staging)
                {
                    //release staging buffer
                    this->_staging_buffer.release();
                }

                return S_OK;
            }
            
            ULONG release()
            {
                //release sampler
                if(this->_sampler)
                {
                    vkDestroySampler(this->_gDevice->vk_device,
                                     this->_sampler,
                                     nullptr);
                    this->_sampler = 0;
                }
                
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
                if( this->_memory)
                {
                    vkFreeMemory(this->_gDevice->vk_device,
                                 this->_memory,
                                 nullptr);
                    this->_memory= 0;
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
            
			const VkDescriptorImageInfo get_descriptor_info() const
			{
				return this->_desc_image_info;
			}

            const uint32_t get_width() const
            {
                return this->_width;
            }
            
            const uint32_t get_height() const
            {
                return this->_height;
            }
            
			const VkImageUsageFlags get_usage() const
			{
				return this->_usage;
			}

            VkSampler get_sampler() const
            {
                return this->_sampler;
            }
                       
            w_image_view get_image_view() const
            {
                return this->_image_view;
            }
            
            VkImageType get_image_type() const
            {
                return this->_image_type;
            }
            
            VkImageViewType get_image_view_type() const
            {
                return this->_image_view_type;
            }
            
            VkFormat get_format() const
            {
                return this->_format;
            }
            
            void* get_pointer_to_staging_data()
            {
                return this->_is_staging ? this->_staging_buffer_memory_pointer : nullptr;
            }

#pragma endregion
              
#pragma region Setters

			void set_format(_In_ VkFormat pFormat)
			{
				this->_format = pFormat;
			}

			void set_buffer_type(_In_ w_texture_buffer_type pBufferType)
			{
#ifdef __VULKAN__
				this->_buffer_type = (VkImageAspectFlagBits)pBufferType;
#else


#endif
			}

			void set_view_type(_In_ w_texture_view_type pViewType)
			{

#ifdef __VULKAN__
				this->_image_view_type = (VkImageViewType)pViewType;

#else


#endif
			}

			void set_usage(_In_ VkImageUsageFlags pUsage)
			{
				this->_usage = pUsage;
			}

#pragma endregion

        private:
            
            std::string                                     _name;
            std::shared_ptr<w_graphics_device>              _gDevice;
            uint32_t                                        _width;
            uint32_t                                        _height;
			VkImageUsageFlags								_usage;
            uint32_t                                        _layer_count;
            VkMemoryPropertyFlags                           _memory_property_flags;
            bool                                            _is_staging;
            void*                                           _staging_buffer_memory_pointer;
            w_buffer                                        _staging_buffer;
			w_image_view									_image_view;
			VkImageAspectFlags								_buffer_type;
            VkSampler                                       _sampler;
            VkDeviceMemory                                  _memory;
            VkFormat                                        _format;
            VkImageType                                     _image_type;
            VkImageViewType                                 _image_view_type;
            VkDescriptorImageInfo                           _desc_image_info;
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

HRESULT w_texture::initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const uint32_t& pWidth,
	_In_ const uint32_t& pHeight,
    _In_ const bool& pIsStaging)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->initialize(pGDevice,
        pWidth,
        pHeight,
        pIsStaging ? (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

HRESULT w_texture::initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const uint32_t& pWidth,
    _In_ const uint32_t& pHeight,
	_In_ const VkMemoryPropertyFlags pMemoryPropertyFlags)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->initialize(pGDevice, pWidth, pHeight, pMemoryPropertyFlags);
}

HRESULT w_texture::load()
{
	if (!this->_pimp) return S_FALSE;
	return this->_pimp->load();
}

HRESULT w_texture::load_texture_2D_from_file(_In_z_ std::wstring pPath, _In_ bool pIsAbsolutePath)
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->load_texture_2D_from_file(pPath, pIsAbsolutePath);
}

HRESULT w_texture::load_texture_from_memory_rgba(_In_ uint8_t* pRGBAData)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->load_texture_from_memory_rgba(pRGBAData);
}

HRESULT w_texture::load_texture_from_memory_rgb(_In_ uint8_t* pRGBData)
{
    if (!this->_pimp) return S_FALSE;
    
    const auto _width = this->_pimp->get_width();
    const auto _height = this->_pimp->get_height();

    if (_width == 0 || _height == 0) return S_FALSE;
    
    auto _rgba = (uint8_t*)malloc(_width * _height * 4);
    if (!_rgba) return S_FALSE;
    
    size_t _count = _width * _height;
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

HRESULT w_texture::load_texture_from_memory_all_channels_same(_In_ uint8_t pData)
{
    if (!this->_pimp) return S_FALSE;

    const auto _width = this->_pimp->get_width();
    const auto _height = this->_pimp->get_height();

    if (_width == 0 || _height == 0) return S_FALSE;

    auto _length = _width * _height * 4;
    auto _rgba = (uint8_t*)malloc(_length * sizeof(uint8_t));
    if (!_rgba) return S_FALSE;

    std::memset(_rgba, pData, _length);
    auto _hr = this->_pimp->load_texture_from_memory_rgba(_rgba);
    free(_rgba);
    return _hr;
}

HRESULT w_texture::load_texture_from_memory_color(_In_ w_color pColor)
{
    if (!this->_pimp) return S_FALSE;

    const auto _width = this->_pimp->get_width();
    const auto _height = this->_pimp->get_height();

    if (_width == 0 || _height == 0) return S_FALSE;

    auto _length = _width * _height * 4;
    auto _rgba = (uint8_t*)malloc(_length * sizeof(uint8_t));
    if (!_rgba) return S_FALSE;

    for (size_t i = 0; i < _length; i += 4)
    {
        _rgba[i + 0] = pColor.r;
        _rgba[i + 1] = pColor.g;
        _rgba[i + 2] = pColor.b;
        _rgba[i + 3] = pColor.a;
    }
    auto _hr = this->_pimp->load_texture_from_memory_rgba(_rgba);
    free(_rgba);
    return _hr;
}

HRESULT w_texture::copy_data_to_texture_2D(_In_ const uint8_t* pRGBA)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->copy_data_to_texture_2D(pRGBA);
}

HRESULT w_texture::flush_staging_data()
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->flush_staging_data();
}

HRESULT w_texture::load_to_shared_textures(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_z_ std::wstring pPath,
    _Inout_ w_texture** pPointerToTexture)
{
    //check if already exists
    auto _iter = _shared.find(pPath);
    if (_iter != _shared.end())
    {
        *pPointerToTexture = _shared[pPath];
        return S_OK;
    }

    auto _texture = new (std::nothrow) w_texture();
    if (!_texture)
    {
        logger.error(L"Could not perform allocation for shared texture: " + pPath);
        return S_FALSE;
    }

    auto _hr = _texture->initialize(pGDevice);
    if (_hr == S_FALSE)
    {
        SAFE_RELEASE(_texture);
        return S_FALSE;
    }
    _hr = _texture->load_texture_2D_from_file(pPath, true);
    if (_hr == S_FALSE)
    {
        SAFE_RELEASE(_texture);
        return S_FALSE;
    }

    _shared[pPath] = _texture;
    *pPointerToTexture = _texture;

    return S_OK;
}

void w_texture::write_bitmap_to_file(
    _In_z_ const char* pFilename,
    _In_ const uint8_t* pData,
    _In_ const int& pWidth, const int& pHeight)
{
    short header[] = { 0x4D42, 0, 0, 0, 0, 26, 0, 12, 0, (short)pWidth, (short)pHeight, 1, 24 };
    FILE *f;
    
#ifdef __WIN32
    if (!fopen_s(&f, pFilename, "wb"))
#else
    f = fopen(pFilename, "wb");
    if (!f)
#endif
    {
        fwrite(header, 1, sizeof(header), f);
        fwrite(pData, 1, pWidth * pHeight * 3, f);
        fclose(f);
    }
}

HRESULT w_texture::save_png_to_file(_In_z_ const char* pFileName, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount, _In_ int pStrideInBytes)
{
	return stbi_write_png(pFileName, static_cast<int>(pWidth), static_cast<int>(pHeight), pCompCount, pData, pStrideInBytes) == 1 ? S_OK : S_FALSE;
}

HRESULT w_texture::save_bmp_to_file(_In_z_ const char* pFileName, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount)
{
	return stbi_write_bmp(pFileName, static_cast<int>(pWidth), static_cast<int>(pHeight), pCompCount, pData) == 1 ? S_OK : S_FALSE;
}

HRESULT w_texture::save_tga_to_file(_In_z_ const char* pFileName, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount)
{
	return stbi_write_tga(pFileName, static_cast<int>(pWidth), static_cast<int>(pHeight), pCompCount, pData) == 1 ? S_OK : S_FALSE;
}

HRESULT w_texture::save_hdr_to_file(_In_z_ const char* pFileName, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const float* pData, _In_ int pCompCount)
{
	return stbi_write_hdr(pFileName, static_cast<int>(pWidth), static_cast<int>(pHeight), pCompCount, pData) == 1 ? S_OK : S_FALSE;
}

HRESULT w_texture::save_jpg_to_file(_In_z_ const char* pFileName, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pCompCount, _In_ int pQuality)
{
	return stbi_write_jpg(pFileName, static_cast<int>(pWidth), static_cast<int>(pHeight), pCompCount, pData, pQuality) == 1 ? S_OK : S_FALSE;
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

    default_texture = nullptr;
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

const VkImageUsageFlags w_texture::get_usage() const
{
	if (!this->_pimp) return 0;
	return this->_pimp->get_usage();
}

VkSampler w_texture::get_sampler() const
{
    if(!this->_pimp) return 0;
    return this->_pimp->get_sampler();
}

//get image and view resources
w_image_view w_texture::get_image_view() const
{
	if (!this->_pimp) return w_image_view();
	return this->_pimp->get_image_view();
}

//get image type
VkImageType w_texture::get_image_type() const
{
    if(!this->_pimp) return VkImageType::VK_IMAGE_TYPE_1D;
    return this->_pimp->get_image_type();
}

//get image view type
VkImageViewType w_texture::get_image_view_type() const
{
    if(!this->_pimp) return VkImageViewType::VK_IMAGE_VIEW_TYPE_1D;
    return this->_pimp->get_image_view_type();
}

//get image format
VkFormat w_texture::get_format() const
{
    if(!this->_pimp) return VkFormat::VK_FORMAT_UNDEFINED;
    return this->_pimp->get_format();
}

const VkDescriptorImageInfo w_texture::get_descriptor_info() const
{
    if(!this->_pimp) return VkDescriptorImageInfo();
    return this->_pimp->get_descriptor_info();
}

void* w_texture::get_pointer_to_staging_data()
{
    if (!this->_pimp) return nullptr;
    return this->_pimp->get_pointer_to_staging_data();
}

#pragma endregion

#pragma region Setters

void w_texture::set_format(_In_ VkFormat pFormat)
{
	if (!this->_pimp) return;
	return this->_pimp->set_format(pFormat);
}

void w_texture::set_usage(_In_ VkImageUsageFlags pUsage)
{
	if (!this->_pimp) return;
	return this->_pimp->set_usage(pUsage);
}

void w_texture::set_buffer_type(_In_ w_texture_buffer_type pBufferType)
{
	if (!this->_pimp) return;
	return this->_pimp->set_buffer_type(pBufferType);
}

void w_texture::set_view_type(_In_ w_texture_view_type pViewType)
{
	if (!this->_pimp) return;
	return this->_pimp->set_view_type(pViewType);
}

#pragma endregion