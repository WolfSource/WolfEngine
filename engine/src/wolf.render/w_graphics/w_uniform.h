/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_shader_buffer.h
	Description		 : HLSL/GLSL shader buffer(in HLSL refers to constant buffers and in GLSL refers to Uniforms)
	Comment          :
*/

#ifndef __W_SHADER_BUFFER_H__
#define __W_SHADER_BUFFER_H__

#include "w_graphics_device_manager.h"
#include <w_graphics/w_command_buffers.h>
#include "w_buffer.h"

namespace wolf
{
	namespace graphics
	{
		template<typename T>
		class w_uniform : public wolf::system::w_object
		{
		public:
            w_uniform()
            {
            }
            
			~w_uniform()
            {
				release();
			}

			//Load the uniform buffer
			HRESULT load(_In_ std::shared_ptr<w_graphics_device>& pGDevice)
			{
                //store the shared graphics device pointer
                auto _buffer_size = static_cast<UINT32>(sizeof(T));
                
                this->_gDevice = pGDevice;
                
                //create uniform buffer
                auto _hr = this->_buffer.load(pGDevice,
                                   _buffer_size,
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
                if(_hr)
                {
                    logger.error("Could not create shader buffer");
                    return S_FALSE;
                }
                
                _hr = this->_buffer.bind();
                if(_hr)
                {
                    logger.error("Error on binding to shader buffer");
                    return S_FALSE;
                }
                
                //create staging buffer
                _hr = this->_staging_buffer.load(pGDevice,
                                                 _buffer_size,
                                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
                if(_hr)
                {
                    logger.error("Could not create staging buffer of shader buffer");
                    return S_FALSE;
                }
                
                 _hr = this->_staging_buffer.bind();
                if(_hr)
                {
                    logger.error("Error on binding to staging buffer of shader buffer");
                    return S_FALSE;
                }
                
                _descriptor_buffer_info.buffer = this->_buffer.get_handle();
                _descriptor_buffer_info.offset = 0;
                _descriptor_buffer_info.range = this->_buffer.get_size();
                
                return S_OK;
            }

			HRESULT update()
			{
                auto _size = this->_staging_buffer.get_size();
                auto _staging_memory = this->_staging_buffer.get_memory();
                void* _staging_buffer_memory_pointer = nullptr;
                
                auto _hr = vkMapMemory(this->_gDevice->vk_device,
                                       this->_staging_buffer.get_memory(),
                                       0,
                                       _size,
                                       0,
                                       &_staging_buffer_memory_pointer);
                if(_hr)
                {
                    _staging_memory = nullptr;
                    
                    V(S_FALSE, "w_shader_buffer", "mapping memory and upload data to stagging buffer for graphics device: " + this->_gDevice->device_name +
                      " ID: " + std::to_string(this->_gDevice->device_id), 3, false, true);
                    return S_FALSE;
                }
                    

                memcpy(_staging_buffer_memory_pointer, &this->data, _size);
                
                VkMappedMemoryRange _flush_range =
                {
                    VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,              // Type
                    nullptr,                                            // Next
                    _staging_memory,                                    // Memory
                    0,                                                  // Offset
                    _size                                               // Size
                };
                vkFlushMappedMemoryRanges(this->_gDevice->vk_device, 1, &_flush_range );
                vkUnmapMemory(this->_gDevice->vk_device, _staging_memory);
                
                w_command_buffers _command_buffer;
                auto _HR = _command_buffer.load(this->_gDevice, 1);
                if(_HR)
                {
                    _staging_memory = nullptr;
                    _staging_buffer_memory_pointer = nullptr;
                    
                    return S_FALSE;
                }
                
                _HR = _command_buffer.begin(0, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
                {
                    if(_HR)
                    {
                        _staging_memory = nullptr;
                        _staging_buffer_memory_pointer = nullptr;
                        
                        return S_FALSE;
                    }
                
                    VkBufferCopy _buffer_copy_info =
                    {
                        0,                                                  // SrcOffset
                        0,                                                  // DstOffset
                        _size                                               // Size
                    };
                
                    auto _staging_handle = this->_staging_buffer.get_handle();
                    auto _buffer_handle = this->_buffer.get_handle();
                
                    auto __cmd = _command_buffer.get_command_at(0);
                    vkCmdCopyBuffer(__cmd,
                                    _staging_handle,
                                    _buffer_handle,
                                    1,
                                    &_buffer_copy_info);
                
                    VkBufferMemoryBarrier _buffer_memory_barrier =
                    {
                        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,            // Type;
                        nullptr,                                            // Next
                        VK_ACCESS_MEMORY_WRITE_BIT,                         // SrcAccessMask
                        VK_ACCESS_UNIFORM_READ_BIT,                         // DstAccessMask
                        VK_QUEUE_FAMILY_IGNORED,                            // SrcQueueFamilyIndex
                        VK_QUEUE_FAMILY_IGNORED,                            // DstQueueFamilyIndex
                        _buffer_handle,                                     // Buffer
                        0,                                                  // Offset
                        VK_WHOLE_SIZE                                       // Size
                    };
                    vkCmdPipelineBarrier( __cmd,
                                         VK_PIPELINE_STAGE_TRANSFER_BIT,
                                         VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
                                         0,
                                         0,
                                         nullptr,
                                         1,
                                         &_buffer_memory_barrier,
                                         0,
                                         nullptr);
                }
                _command_buffer.end(0);
                
                // Submit command buffer and copy data from staging buffer to a vertex buffer
                VkSubmitInfo _submit_info =
                {
                    VK_STRUCTURE_TYPE_SUBMIT_INFO,                      // Type
                    nullptr,                                            // Next
                    0,                                                  // WaitSemaphoreCount
                    nullptr,                                            // WaitSemaphores
                    nullptr,                                            // WaitDstStageMask;
                    1,                                                  // CommandBufferCount
                    _command_buffer.get_commands(),                     // CommandBuffers
                    0,                                                  // SignalSemaphoreCount
                    nullptr                                             // SignalSemaphores
                };
                
                _hr = vkQueueSubmit(this->_gDevice->vk_graphics_queue,
                                    1,
                                    &_submit_info,
                                    0);
                
                _staging_memory = nullptr;
                _staging_buffer_memory_pointer = nullptr;
                
                if(_hr)
                {
                    V(S_FALSE, "w_shader_buffer", "mapping memory and upload data to stagging buffer for graphics device: " + this->_gDevice->device_name +
                      " ID: " + std::to_string(this->_gDevice->device_id), 3, false, true);
                    return S_FALSE;
                }
                
                vkDeviceWaitIdle(this->_gDevice->vk_device);
                
                _command_buffer.release();
                
                return S_OK;
            }

            const VkDescriptorBufferInfo get_descriptor_info() const
            {
                const VkDescriptorBufferInfo _info =
                {
                    this->_buffer.get_handle(),
                    0,
                    this->_buffer.get_size(),
                };
                return _info;
            }
                
			//Release resources
			ULONG release()
			{
				if (this->get_is_released()) return 0;
				
                this->_buffer.release();
                this->_staging_buffer.release();
                this->_gDevice = nullptr;
                
				return _super::release();
			}
            
            UINT32 get_size()
            {
                return this->_buffer.get_size();
            }
            
            VkBuffer get_handle()
            {
                return this->_buffer.get_handle();
            }
            
            
            //constant buffer/uniform data
			T data;

		private:
            typedef  wolf::system::w_object      _super;
            std::shared_ptr<w_graphics_device>   _gDevice;
            w_buffer                             _buffer;
            w_buffer                             _staging_buffer;
            VkDescriptorBufferInfo               _descriptor_buffer_info;
            
		};
	}
}

#endif
