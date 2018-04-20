#include "w_render_pch.h"
#include "w_graphics_device_manager.h"
#include "w_buffer.h"
#include <w_convert.h>
#include "w_command_buffers.h"
#include <glm_extension.h>

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
				_map_data(nullptr),
				_memory_allocation(nullptr),
				_mapped(false),
				_allocated_from_pool(false)
            {
            }
            
			W_RESULT allocate(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ uint32_t& pBufferSizeInBytes,
				_In_ const uint32_t& pUsageFlags,
				_In_ const w_memory_usage_flag& pMemoryFlag,
				_In_ const bool& pAllocateFromMemoryPool)
			{
				const std::string _trace_info = this->_name + "::allocate";

				this->_gDevice = pGDevice;
				this->_usage_flags = pUsageFlags;
				this->_memory_flag = pMemoryFlag;
				this->_allocated_from_pool = pAllocateFromMemoryPool;

				if (pAllocateFromMemoryPool)
				{
					pBufferSizeInBytes = static_cast<uint32_t>(std::round_up(pBufferSizeInBytes, 4));

					const VkBufferCreateInfo _buffer_create_info =
					{
						VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,             // Type
						nullptr,                                          // Next
						0,                                                // Flags
						(VkDeviceSize)pBufferSizeInBytes,				  // Size
						(VkBufferUsageFlags)this->_usage_flags,           // Usage
						VK_SHARING_MODE_EXCLUSIVE,                        // SharingMode
						0,                                                // QueueFamilyIndexCount
						nullptr                                           // QueueFamilyIndices
					};

					//alocate memory for this buffer
					this->_memory_allocation = this->_gDevice->memory_allocator.allocate_buffer(
						_buffer_create_info,
						this->_memory_flag,
						this->_buffer_handle.handle,
						this->_memory_allocation_info);
					if (!this->_memory_allocation)
					{

						V(W_FAILED, "allocating memory for graphics device: " + this->_gDevice->get_info(), _trace_info, 3);
						return W_FAILED;
					}
				}
				else
				{
					//create seperated buffer

					if (this->_memory_flag & w_memory_usage_flag::MEMORY_USAGE_CPU_ONLY)
					{
						this->_memory_property_flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
					}
					else if (this->_memory_flag & w_memory_usage_flag::MEMORY_USAGE_GPU_ONLY)
					{
						this->_memory_property_flags = w_memory_property_flag_bits::DEVICE_LOCAL_BIT;
					}

					const VkBufferCreateInfo _buffer_create_info =
					{
						VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,             // Type
						nullptr,                                          // Next
						0,                                                // Flags
						(VkDeviceSize)pBufferSizeInBytes,               // Size
						(VkBufferUsageFlags)this->_usage_flags,           // Usage
						VK_SHARING_MODE_EXCLUSIVE,                        // SharingMode
						0,                                                // QueueFamilyIndexCount
						nullptr                                           // QueueFamilyIndices
					};

					auto _hr = vkCreateBuffer(this->_gDevice->vk_device,
						&_buffer_create_info,
						nullptr,
						&this->_buffer_handle.handle);
					if (_hr)
					{
						V(W_FAILED, "w_buffer", "creating buffer for graphics device: " + this->_gDevice->device_info->get_device_name() +
							" ID: " + std::to_string(this->_gDevice->device_info->get_device_id()), 3, false);
						return W_FAILED;
					}

					VkMemoryRequirements _buffer_memory_requirements;
					vkGetBufferMemoryRequirements(this->_gDevice->vk_device,
						this->_buffer_handle.handle,
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
						&this->_memory_allocation_info.deviceMemory))
					{

						V(W_FAILED, "Allocating memory of buffer for graphics device: " + this->_gDevice->device_info->get_device_name() +
							" ID: " + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
						return W_FAILED;
					}

					this->_memory_allocation_info.offset = 0;
					this->_memory_allocation_info.size = pBufferSizeInBytes;
				}

				this->_descriptor_info.buffer = this->_buffer_handle.handle;
				this->_descriptor_info.offset = 0;// this->_memory_allocation_info.offset;
				this->_descriptor_info.range = this->_memory_allocation_info.size;

				return W_PASSED;
			}
            
			W_RESULT reallocate(_In_ uint32_t& pBufferSizeInBytes)
			{
				const std::string _trace_info = this->_name + "::reallocate";

				if (!this->_gDevice) return W_FAILED;
				
				if (free() == W_PASSED)
				{
					return allocate(this->_gDevice, pBufferSizeInBytes, this->_usage_flags, this->_memory_flag, this->_allocated_from_pool);
				}
				return W_FAILED;
			}

			W_RESULT bind()
			{
				if (this->_allocated_from_pool)
				{
					return this->_gDevice->memory_allocator.bind(this->_memory_allocation, this->_buffer_handle.handle);
				}
				else
				{
					return vkBindBufferMemory(this->_gDevice->vk_device,
						this->_buffer_handle.handle,
						this->_memory_allocation_info.deviceMemory,
						0) == VK_SUCCESS ? W_PASSED : W_FAILED;
				}
			}

            //Set data to DRAM
            W_RESULT set_data(_In_ const void* const pData)
            {
                //we can not access to VRAM, but we can copy our data to DRAM
                if (this->_memory_flag == w_memory_usage_flag::MEMORY_USAGE_GPU_ONLY) return W_FAILED;

                if (map() == nullptr) return W_FAILED;
                memcpy(this->_map_data, pData, (size_t)this->_memory_allocation_info.size);
                if (flush() == W_FAILED) return W_FAILED;

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

				auto _dest_buffer = pDestinationBuffer.get_buffer_handle();
                auto _copy_cmd = _copy_command_buffer.get_command_at(0);
                VkBufferCopy _copy_region = {};
                _copy_region.size = this->_memory_allocation_info.size;
                vkCmdCopyBuffer(
                    _copy_cmd.handle,
                    this->_buffer_handle.handle,
					_dest_buffer.handle,
                    1,
                    &_copy_region);

                _hr = _copy_command_buffer.flush(0);
                if (_hr != W_PASSED)
                {
					_copy_cmd.handle = nullptr;
                    V(W_FAILED,
                        "flushing command buffer of " +
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
				if (this->_allocated_from_pool)
				{
					if (this->_gDevice->memory_allocator.map(this->_memory_allocation, &this->_map_data) == W_FAILED)
					{
						this->_map_data = nullptr;
						V(W_FAILED, "mapping memory for graphics device: " + this->_gDevice->get_info(), _trace_info, 3);
						return nullptr;
					}
				}
				else
				{
					if (this->_memory_property_flags & w_memory_property_flag_bits::DEVICE_LOCAL_BIT) return nullptr;
					if (vkMapMemory(this->_gDevice->vk_device,
						this->_memory_allocation_info.deviceMemory,
						0,
						this->_memory_allocation_info.size,
						0,
						&this->_map_data))
					{
						this->_map_data = nullptr;
						V(W_FAILED, "mapping memory for graphics device: " + this->_gDevice->get_info(), _trace_info, 3);
						return nullptr;
					}
				}
				this->_mapped = true;
				return this->_map_data;
			}

            void unmap()
            {
				this->_gDevice->memory_allocator.unmap(this->_memory_allocation);
				this->_mapped = false;
                if (this->_map_data)
                {
					this->_map_data = nullptr;
                }
            }

			W_RESULT flush()
			{
				VkMemoryPropertyFlags _mem_flags = this->_gDevice->memory_allocator.get_memory_property_flags(this->_memory_allocation_info);
				if ((_mem_flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
				{
					VkMappedMemoryRange _mem_range = { VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE };
					_mem_range.memory = this->_memory_allocation_info.deviceMemory;
					_mem_range.offset = this->_memory_allocation_info.offset;
					_mem_range.size = this->_memory_allocation_info.size;
					return vkFlushMappedMemoryRanges(this->_gDevice->vk_device, 1, &_mem_range) ? W_FAILED : W_PASSED;
				}
				return W_PASSED;
			}
            
			W_RESULT free()
			{
				if (!this->_gDevice) return W_FAILED;

				if (this->_mapped)
				{
					unmap();
				}
				if (this->_allocated_from_pool)
				{
					this->_gDevice->memory_allocator.free_buffer(this->_memory_allocation, this->_buffer_handle.handle);
					SAFE_DELETE(this->_memory_allocation);
				}
				else
				{
					if (this->_buffer_handle.handle)
					{
						vkDestroyBuffer(this->_gDevice->vk_device,
							this->_buffer_handle.handle,
							nullptr);

						this->_buffer_handle.handle = 0;
					}

					if (this->_memory_allocation_info.deviceMemory)
					{
						vkFreeMemory(this->_gDevice->vk_device,
							this->_memory_allocation_info.deviceMemory,
							nullptr);

						this->_memory_allocation_info.deviceMemory = 0;
					}
				}

				return W_PASSED;
			}

            ULONG release()
            {
                this->_name.clear();
                
				free();
				this->_gDevice = nullptr;

                return 0;
            }
            
            const uint32_t get_size() const
            {
                return (uint32_t)this->_memory_allocation_info.size;
            }

			const uint32_t get_offset() const
			{
				return (uint32_t)this->_memory_allocation_info.offset;
			}

			const w_device_memory get_memory() const
			{
				w_device_memory _memory;
				_memory.handle = this->_memory_allocation_info.deviceMemory;
				return _memory;
			}
            
            const uint32_t get_usage_flags() const
            {
                return this->_usage_flags;
            }
            
            const uint32_t get_memory_flags() const
            {
                return this->_memory_flag;
            }
            
            const w_buffer_handle get_handle() const
            {
                return this->_buffer_handle;
            }
                        
            const w_descriptor_buffer_info get_descriptor_info() const
            {
                return this->_descriptor_info;
            }

        private:
            std::string                                         _name;
            std::shared_ptr<w_graphics_device>                  _gDevice;
			bool												_mapped;
            void*                                               _map_data;            
			w_buffer_handle                                     _buffer_handle;
			w_memory_usage_flag									_memory_flag;
			uint32_t											_usage_flags;
			w_descriptor_buffer_info                            _descriptor_info;
			VmaAllocation*										_memory_allocation;
			VmaAllocationInfo									_memory_allocation_info;

			bool												_allocated_from_pool;

			uint32_t											_memory_property_flags;
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

W_RESULT w_buffer::allocate_as_staging(
	_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ uint32_t& pBufferSize,
	_In_ const bool& pAllocateFromMemoryPool)
{
	if (!this->_pimp) return W_FAILED;

	return this->_pimp->allocate(pGDevice,
		pBufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		w_memory_usage_flag::MEMORY_USAGE_CPU_ONLY,
		pAllocateFromMemoryPool);
}

W_RESULT w_buffer::allocate(
	_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ uint32_t& pBufferSizeInBytes,
    _In_ const uint32_t& pUsageFlags,
    _In_ const w_memory_usage_flag& pMemoryFlag,
	_In_ const bool& pAllocateFromMemoryPool)
{
    if(!this->_pimp) return W_FAILED;
    
    return this->_pimp->allocate(pGDevice, pBufferSizeInBytes, pUsageFlags, pMemoryFlag, pAllocateFromMemoryPool);
}

W_RESULT w_buffer::reallocate(_In_ uint32_t& pBufferSizeInBytes)
{
	if (!this->_pimp) return W_FAILED;

	return this->_pimp->reallocate(pBufferSizeInBytes);
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

W_RESULT w_buffer::flush()
{
    if (!this->_pimp) return W_FAILED;

    return this->_pimp->flush();
}

W_RESULT w_buffer::free()
{
	if (!this->_pimp) return W_FAILED;

	return this->_pimp->free();
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

const uint32_t w_buffer::get_offset() const
{
	if (!this->_pimp) return 0;

	return this->_pimp->get_offset();
}

const uint32_t w_buffer::get_usage_flags() const
{
    if(!this->_pimp) return VkBufferUsageFlagBits::VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    
    return this->_pimp->get_usage_flags();
}

const uint32_t w_buffer::get_memory_flags() const
{
    if(!this->_pimp) return w_memory_property_flag_bits::DEVICE_LOCAL_BIT;
    
    return this->_pimp->get_memory_flags();
}

const w_buffer_handle w_buffer::get_buffer_handle() const
{
    if(!this->_pimp) return w_buffer_handle();
    
    return this->_pimp->get_handle();
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

const w_device_memory w_buffer::get_memory() const
{
	return this->_pimp ? this->_pimp->get_memory() : w_device_memory();
}

#pragma endregion

