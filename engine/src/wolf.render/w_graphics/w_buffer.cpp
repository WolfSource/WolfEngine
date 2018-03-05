#include "w_render_pch.h"
#include "w_buffer.h"
#include <w_convert.h>
#include "w_command_buffers.h"

namespace wolf
{
    namespace graphics
    {
        class w_buffer_pimp
        {
        public:
            w_buffer_pimp() :
                _name("w_buffer::"),
                _gDevice(nullptr),
				_size_in_bytes(0),
                _handle(0),
                _mapped(nullptr)
            {
            }
            
            W_RESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const uint32_t pBufferSizeInBytes,
                _In_ const w_buffer_usage_flags pUsageFlags,
                _In_ const w_memory_property_flags pMemoryPropertyFlags)
            {
                this->_gDevice = pGDevice;

                this->_usage_flags = pUsageFlags;
                this->_memory_property_flags = pMemoryPropertyFlags;
                this->_size_in_bytes = pBufferSizeInBytes;

                const VkBufferCreateInfo _buffer_create_info =
                {
                    VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,             // Type
                    nullptr,                                          // Next
                    0,                                                // Flags
                    (VkDeviceSize)this->_size_in_bytes,               // Size
                    (VkBufferUsageFlags)this->_usage_flags,           // Usage
                    VK_SHARING_MODE_EXCLUSIVE,                        // SharingMode
                    0,                                                // QueueFamilyIndexCount
                    nullptr                                           // QueueFamilyIndices
                };

                auto _hr = vkCreateBuffer(this->_gDevice->vk_device,
                    &_buffer_create_info,
                    nullptr,
                    &this->_handle);
                if (_hr)
                {
                    V(W_FAILED, "w_buffer", "creating buffer for graphics device: " + this->_gDevice->device_info->get_device_name() +
                        " ID: " + std::to_string(this->_gDevice->device_info->get_device_id()), 3, false);
                    return W_FAILED;
                }

                VkMemoryRequirements _buffer_memory_requirements;
                vkGetBufferMemoryRequirements(this->_gDevice->vk_device,
                    this->_handle,
                    &_buffer_memory_requirements);

                VkPhysicalDeviceMemoryProperties _memory_properties;
                vkGetPhysicalDeviceMemoryProperties(this->_gDevice->vk_physical_device, &_memory_properties);

				uint32_t _mem_index = 0;
				if (w_graphics_device_manager::memory_type_from_properties(
					_memory_properties,
					_buffer_memory_requirements.memoryTypeBits,
					_memory_property_flags,
					&_mem_index))
				{
					V(W_FAILED, "finding memory index of buffer for graphics device: " + this->_gDevice->device_info->get_device_name() +
						" ID: " + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
					return W_FAILED;
				}

				VkMemoryAllocateInfo _memory_allocate_info =
				{
					VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, // Type
					nullptr,                                // Next
					_buffer_memory_requirements.size,       // AllocationSize
					_mem_index                              // MemoryTypeIndex
				};
				if (vkAllocateMemory(this->_gDevice->vk_device,
					&_memory_allocate_info,
					nullptr,
					&this->_memory.handle))
				{
					
					V(W_FAILED, "Allocating memory of buffer for graphics device: " + this->_gDevice->device_info->get_device_name() +
						" ID: " + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
					return W_FAILED;
				}

				this->_descriptor_info.buffer = this->_handle;
				this->_descriptor_info.offset = 0;
				this->_descriptor_info.range = this->_size_in_bytes;
				
                return W_PASSED;
            }
            
            W_RESULT bind()
            {
                return vkBindBufferMemory(this->_gDevice->vk_device,
                    this->_handle,
                    this->_memory.handle,
                    0) == VK_SUCCESS ? W_PASSED : W_FAILED;

            }

            //Set data to DRAM
            W_RESULT set_data(_In_ const void* const pData)
            {
                //we can not access to VRAM, but we can copy our data to DRAM
                if (this->_memory_property_flags & w_memory_property_flag_bits::DEVICE_LOCAL_BIT) return W_FAILED;

                if (map() == nullptr) return W_FAILED;
                memcpy(this->_mapped, pData, (size_t)this->_size_in_bytes);

                if (flush(VK_WHOLE_SIZE, 0) == W_FAILED) return W_FAILED;

                unmap();

                return W_PASSED;
            }
            
            W_RESULT copy_to(_In_ w_buffer& pDestinationBuffer)
            {
                const std::string _trace = this->_name + "copy_to";
                
                //create one command buffer
                w_command_buffers _copy_command_buffer;
                auto _hr = _copy_command_buffer.load(this->_gDevice, 1);
                if (_hr != W_PASSED)
                {
                    V(W_FAILED, "loading command buffer " +
                        _gDevice->get_info(),
                        _trace,
                        3);
                    return _hr;
                }

                _hr = _copy_command_buffer.begin(0);
                if (_hr != W_PASSED)
                {
                    V(W_FAILED, "begining command buffer " +
                        _gDevice->get_info(),
                        _trace,
                        3);
                    return _hr;
                }

                auto _copy_cmd = _copy_command_buffer.get_command_at(0);
                VkBufferCopy _copy_region = {};
                _copy_region.size = this->_size_in_bytes;
                vkCmdCopyBuffer(
                    _copy_cmd.handle,
                    this->_handle,
                    pDestinationBuffer.get_handle(),
                    1,
                    &_copy_region);

                _hr = _copy_command_buffer.flush(0);
                if (_hr != W_PASSED)
                {
					_copy_cmd.handle = nullptr;
                    V(W_FAILED,
                        "flushing command buffer" +
                        _gDevice->get_info(),
                        _trace,
                        3);
                    return _hr;
                }

                _copy_command_buffer.release();
				_copy_cmd.handle = nullptr;

                return W_PASSED;
            }

            void* map()
            {
                const std::string _trace_info = this->_name + "::map";

                //we can not access to VRAM, but we can copy our data to DRAM
                if (this->_memory_property_flags & w_memory_property_flag_bits::DEVICE_LOCAL_BIT) return nullptr;

                auto _size = this->get_size();
                auto _memory = this->get_memory();

                auto _hr = vkMapMemory(this->_gDevice->vk_device,
                    _memory.handle,
                    0,
                    _size,
                    0,
                    &this->_mapped);

                if (_hr)
                {
                    this->_mapped = nullptr;
                    V(W_FAILED, "mapping data to to vertex buffer's memory " +
                        _gDevice->get_info(),
                        _trace_info, 3, false);

                    return nullptr;
                }

                return this->_mapped;
            }

            void unmap()
            {
                if (this->_mapped)
                {
                    vkUnmapMemory(this->_gDevice->vk_device, _memory.handle);
                    this->_mapped = nullptr;
                }
            }

            W_RESULT flush(VkDeviceSize pSize, VkDeviceSize pOffset)
            {
                VkMappedMemoryRange _mapped_range = {};
                _mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
                _mapped_range.memory = this->_memory.handle;
                _mapped_range.offset = pOffset;
                _mapped_range.size = pSize;

                return vkFlushMappedMemoryRanges(this->_gDevice->vk_device, 1, &_mapped_range) == VK_SUCCESS ? W_PASSED : W_FAILED;
            }
            
            ULONG release()
            {
                this->_size_in_bytes = 0;
                this->_name.clear();
                
                unmap();

                if(this->_handle)
                {
                    vkDestroyBuffer(this->_gDevice->vk_device,
                                    this->_handle,
                                    nullptr);
                    
                    this->_handle = 0;
                }
                
                if(this->_memory.handle)
                {
                    vkFreeMemory(this->_gDevice->vk_device,
                                 this->_memory.handle,
                                 nullptr);
                    
                    this->_memory.handle = 0;
                }

                this->_gDevice = nullptr;
                
                return 0;
            }
            
            const uint32_t get_size() const
            {
                return this->_size_in_bytes;
            }
            
            const VkBufferUsageFlags get_usage_flags() const
            {
                return this->_usage_flags;
            }
            
            const w_memory_property_flags get_memory_flags() const
            {
                return this->_memory_property_flags;
            }
            
            const VkBuffer get_handle() const
            {
                return this->_handle;
            }
            
            const w_device_memory get_memory() const
            {
                return this->_memory;
            }
            
            const w_descriptor_buffer_info get_descriptor_info() const
            {
                return this->_descriptor_info;
            }

        private:
            std::string                                         _name;
            std::shared_ptr<w_graphics_device>                  _gDevice;
            uint32_t                                            _size_in_bytes;
            void*                                               _mapped;
            
            VkBuffer                                            _handle;
			w_device_memory                                     _memory;
            w_memory_property_flags                             _memory_property_flags;
			w_buffer_usage_flags                                _usage_flags;
			w_descriptor_buffer_info                            _descriptor_info;
            
        };
    }
}

using namespace wolf::graphics;

w_buffer::w_buffer() : _pimp(new w_buffer_pimp())
{
    _super::set_class_name("w_buffer");
}

w_buffer::~w_buffer()
{
    release();
}

W_RESULT w_buffer::load_as_staging(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                                  _In_ const uint32_t pBufferSize)
{
    if(!this->_pimp) return W_FAILED;
    
    return this->_pimp->load(pGDevice,
                             pBufferSize,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

W_RESULT w_buffer::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                       _In_ const uint32_t pBufferSizeInBytes,
                       _In_ const w_buffer_usage_flags pUsageFlags,
                       _In_ const w_memory_property_flags pMemoryFlags)
{
    if(!this->_pimp) return W_FAILED;
    
    return this->_pimp->load(pGDevice, pBufferSizeInBytes, pUsageFlags, pMemoryFlags);
}

W_RESULT w_buffer::bind()
{
    if (!this->_pimp) return W_FAILED;

    return this->_pimp->bind();
}

W_RESULT w_buffer::set_data(_In_ const void* const pData)
{
    if(!this->_pimp) return W_FAILED;
    
    return this->_pimp->set_data(pData);
}

W_RESULT w_buffer::copy_to(_In_ w_buffer& pDestinationBuffer)
{
    if (!this->_pimp) return W_FAILED;

    return this->_pimp->copy_to(pDestinationBuffer);
}

void* w_buffer::map()
{
    if (!this->_pimp) return nullptr;

    return this->_pimp->map();
}

void w_buffer::unmap()
{
    if (!this->_pimp) return;

    this->_pimp->unmap();
}

W_RESULT w_buffer::flush(VkDeviceSize pSize, VkDeviceSize pOffset)
{
    if (!this->_pimp) return W_FAILED;

    return this->_pimp->flush(pSize, pOffset);
}

ULONG w_buffer::release()
{
    if(_super::get_is_released()) return 1;
    
    SAFE_RELEASE(this->_pimp);
    
    return _super::release();
}

#pragma region Getters

const uint32_t w_buffer::get_size() const
{
    if(!this->_pimp) return 0;
    
    return this->_pimp->get_size();
}

const VkBufferUsageFlags w_buffer::get_usage_flags() const
{
    if(!this->_pimp) return VkBufferUsageFlagBits::VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    
    return this->_pimp->get_usage_flags();
}

const w_memory_property_flags w_buffer::get_memory_flags() const
{
    if(!this->_pimp) return w_memory_property_flag_bits::DEVICE_LOCAL_BIT;
    
    return this->_pimp->get_memory_flags();
}

const VkBuffer w_buffer::get_handle() const
{
    if(!this->_pimp) return 0;
    
    return this->_pimp->get_handle();
}

const w_device_memory w_buffer::get_memory() const
{
    if(!this->_pimp) return w_device_memory();
    
    return this->_pimp->get_memory();
}

const w_descriptor_buffer_info w_buffer::get_descriptor_info() const
{
	if (!this->_pimp)
	{
		w_descriptor_buffer_info _buffer_info;
		_buffer_info.buffer = 0;
		_buffer_info.offset = 0;
		_buffer_info.range = 0;
		return _buffer_info;
	}
    return this->_pimp->get_descriptor_info();
}

#pragma endregion

