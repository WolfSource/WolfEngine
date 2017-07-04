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
                _size(0),
                _handle(0),
                _memory(0),
                _mapped(nullptr)
            {
            }
            
            HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const uint32_t pBufferSize,
                _In_ const VkBufferUsageFlags pUsageFlags,
                _In_ const VkMemoryPropertyFlags pMemoryFlags)
            {
                this->_gDevice = pGDevice;
                this->_usage_flags = pUsageFlags;
                this->_memory_flags = pMemoryFlags;

                this->_size = pBufferSize;

                const VkBufferCreateInfo _buffer_create_info =
                {
                    VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,             // Type
                    nullptr,                                          // Next
                    0,                                                // Flags
                    (VkDeviceSize)this->_size,                        // Size
                    this->_usage_flags,                                           // Usage
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
                    V(S_FALSE, "w_buffer", "creating buffer for graphics device: " + this->_gDevice->device_name +
                        " ID: " + std::to_string(this->_gDevice->device_id), 3, false, true);
                    return S_FALSE;
                }

                VkMemoryRequirements _buffer_memory_requirements;
                vkGetBufferMemoryRequirements(this->_gDevice->vk_device,
                    this->_handle,
                    &_buffer_memory_requirements);

                VkPhysicalDeviceMemoryProperties _memory_properties;
                vkGetPhysicalDeviceMemoryProperties(this->_gDevice->vk_physical_device,
                    &_memory_properties);

                for (uint32_t i = 0; i < _memory_properties.memoryTypeCount; ++i)
                {
                    if ((_buffer_memory_requirements.memoryTypeBits & (1 << i)) &&
                        (_memory_properties.memoryTypes[i].propertyFlags & this->_memory_flags))
                    {

                        VkMemoryAllocateInfo _memory_allocate_info =
                        {
                            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, // Type
                            nullptr,                                // Next
                            _buffer_memory_requirements.size,       // AllocationSize
                            i                                       // MemoryTypeIndex
                        };

                        if (vkAllocateMemory(this->_gDevice->vk_device,
                            &_memory_allocate_info,
                            nullptr,
                            &this->_memory) == VK_SUCCESS)
                        {
                            this->_descriptor_info.buffer = this->_handle;
                            this->_descriptor_info.offset = 0;
                            this->_descriptor_info.range = this->_size;

                            return S_OK;
                        }
                    }
                }

                logger.error("Could not create buffer, because proposed memory property not found.");

                return S_FALSE;
            }
            
            HRESULT bind()
            {
                return vkBindBufferMemory(this->_gDevice->vk_device,
                    this->_handle,
                    this->_memory,
                    0) == VK_SUCCESS ? S_OK : S_FALSE;
            }

            //Set data to DRAM
            HRESULT set_data(_In_ const void* const pData)
            {
                //we can not access to VRAM, but we can copy our data to DRAM
                if (this->_memory_flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) return S_FALSE;

                if (map() == nullptr) return S_FALSE;
                memcpy(this->_mapped, pData, (size_t)_size);

                if (flush(VK_WHOLE_SIZE, 0) == S_FALSE) return S_FALSE;

                unmap();

                return S_OK;
            }
            
            HRESULT copy_to(_In_ w_buffer& pDestinationBuffer)
            {
                const std::string _trace = this->_name + "copy_to";
                
                //create one command buffer
                w_command_buffers _copy_command_buffer;
                auto _hr = _copy_command_buffer.load(this->_gDevice, 1);
                if (_hr != S_OK)
                {
                    V(S_FALSE, "loading command buffer " +
                        _gDevice->print_info(),
                        _trace,
                        3);
                    return _hr;
                }

                _hr = _copy_command_buffer.begin(0);
                if (_hr != S_OK)
                {
                    V(S_FALSE, "begining command buffer " +
                        _gDevice->print_info(),
                        _trace,
                        3);
                    return _hr;
                }

                auto _copy_cmd = _copy_command_buffer.get_command_at(0);

                VkBufferCopy _copy_region = {};

                _copy_region.size = this->_size;
                vkCmdCopyBuffer(
                    _copy_cmd,
                    this->_handle,
                    pDestinationBuffer.get_handle(),
                    1,
                    &_copy_region);

                _hr = _copy_command_buffer.flush(0);
                if (_hr != S_OK)
                {
                    V(S_FALSE,
                        "flushing command buffer" +
                        _gDevice->print_info(),
                        _trace,
                        3);
                    return _hr;
                }

                _copy_command_buffer.release();

                return S_OK;
            }

            void* map()
            {
                const std::string _trace = this->_name + "map";

                //we can not access to VRAM, but we can copy our data to DRAM
                if (this->_memory_flags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) return nullptr;

                auto _size = this->get_size();
                auto _memory = this->get_memory();

                auto _hr = vkMapMemory(this->_gDevice->vk_device,
                    _memory,
                    0,
                    _size,
                    0,
                    &this->_mapped);
                if (_hr)
                {
                    this->_mapped = nullptr;
                    V(S_FALSE, "mapping data to to vertex buffer's memory " + 
                        _gDevice->print_info(),
                        _trace, 3, false, true);

                    return nullptr;
                }

                return this->_mapped;
            }

            void unmap()
            {
                if (this->_mapped)
                {
                    vkUnmapMemory(this->_gDevice->vk_device, _memory);
                    this->_mapped = nullptr;
                }
            }

            HRESULT flush(VkDeviceSize pSize, VkDeviceSize pOffset)
            {
                VkMappedMemoryRange _mapped_range = {};
                _mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
                _mapped_range.memory = this->_memory;
                _mapped_range.offset = pOffset;
                _mapped_range.size = pSize;
                return vkFlushMappedMemoryRanges(this->_gDevice->vk_device, 1, &_mapped_range) == VK_SUCCESS ? S_OK : S_FALSE;
            }
            
            ULONG release()
            {
                this->_size = 0;
                this->_name.clear();
                
                unmap();

                if(this->_handle)
                {
                    vkDestroyBuffer(this->_gDevice->vk_device,
                                    this->_handle,
                                    nullptr);
                    
                    this->_handle = 0;
                }
                
                if(this->_memory)
                {
                    vkFreeMemory(this->_gDevice->vk_device,
                                 this->_memory,
                                 nullptr);
                    
                    this->_memory = 0;
                }

                this->_gDevice = nullptr;
                
                return 1;
            }
            
            const UINT32 get_size() const
            {
                return this->_size;
            }
            
            const VkBufferUsageFlags get_usage_flags() const
            {
                return this->_usage_flags;
            }
            
            const VkMemoryPropertyFlags get_memory_flags() const
            {
                return this->_memory_flags;
            }
            
            const VkBuffer get_handle() const
            {
                return this->_handle;
            }
            
            const VkDeviceMemory get_memory() const
            {
                return this->_memory;
            }
            
            const VkDescriptorBufferInfo get_descriptor_info() const
            {
                return this->_descriptor_info;
            }

        private:
            std::string                                         _name;
            std::shared_ptr<w_graphics_device>                  _gDevice;
            UINT32                                              _size;
            void*                                               _mapped;
            
#ifdef __VULKAN__
            VkBuffer                                            _handle;
            VkDeviceMemory                                      _memory;
            VkMemoryPropertyFlags                               _memory_flags;
            VkBufferUsageFlags                                  _usage_flags;
            VkDescriptorBufferInfo                              _descriptor_info;
#endif
            
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

HRESULT w_buffer::load_as_staging(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                                  _In_ const uint32_t pBufferSize)
{
    if(!this->_pimp) return S_FALSE;
    
    return this->_pimp->load(pGDevice,
                             pBufferSize,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

HRESULT w_buffer::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                       _In_ const uint32_t pBufferSize,
                       _In_ const VkBufferUsageFlags pUsageFlags,
                       _In_ const VkMemoryPropertyFlags pMemoryFlags)
{
    if(!this->_pimp) return S_FALSE;
    
    return this->_pimp->load(pGDevice, pBufferSize, pUsageFlags, pMemoryFlags);
}

HRESULT w_buffer::bind()
{
    if (!this->_pimp) return S_FALSE;

    return this->_pimp->bind();
}

HRESULT w_buffer::set_data(_In_ const void* const pData)
{
    if(!this->_pimp) return S_FALSE;
    
    return this->_pimp->set_data(pData);
}

HRESULT w_buffer::copy_to(_In_ w_buffer& pDestinationBuffer)
{
    if (!this->_pimp) return S_FALSE;

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

HRESULT w_buffer::flush(VkDeviceSize pSize, VkDeviceSize pOffset)
{
    if (!this->_pimp) return S_FALSE;

    return this->_pimp->flush(pSize, pOffset);
}

ULONG w_buffer::release()
{
    if(_super::get_is_released()) return 0;
    
    SAFE_RELEASE(this->_pimp);
    
    return _super::release();
}

#pragma region Getters

const UINT32 w_buffer::get_size() const
{
    if(!this->_pimp) return 0;
    
    return this->_pimp->get_size();
}

const VkBufferUsageFlags w_buffer::get_usage_flags() const
{
    if(!this->_pimp) return VkBufferUsageFlagBits::VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
    
    return this->_pimp->get_usage_flags();
}

const VkMemoryPropertyFlags w_buffer::get_memory_flags() const
{
    if(!this->_pimp) return VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    
    return this->_pimp->get_memory_flags();
}

const VkBuffer w_buffer::get_handle() const
{
    if(!this->_pimp) return 0;
    
    return this->_pimp->get_handle();
}

const VkDeviceMemory w_buffer::get_memory() const
{
    if(!this->_pimp) return 0;
    
    return this->_pimp->get_memory();
}

const VkDescriptorBufferInfo w_buffer::get_descriptor_info() const
{
    if (!this->_pimp) return VkDescriptorBufferInfo();

    return this->_pimp->get_descriptor_info();
}

#pragma endregion

