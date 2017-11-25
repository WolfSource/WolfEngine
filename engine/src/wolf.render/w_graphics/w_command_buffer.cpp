#include "w_render_pch.h"
#include "w_command_buffer.h"
#include <w_convert.h>

namespace wolf
{
    namespace graphics
    {
        class w_command_buffer_pimp
        {
        public:
            w_command_buffer_pimp() :
                _name("w_command_buffer"),
                _enable(true),
                _order(0),
                _command_pool(0)
            {
            }
            
            HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice, 
                _In_ const size_t& pCount,
                _In_ const VkCommandBufferLevel& pLevel,
                _In_ const bool& pCreateCommandPool,
                _In_ const w_queue* pCommandPoolQueue)
            {
                if (pCreateCommandPool)
                {
                    VkCommandPoolCreateInfo _command_pool_info = {};
                    _command_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                    _command_pool_info.queueFamilyIndex = pCommandPoolQueue ? pCommandPoolQueue->index : pGDevice->vk_graphics_queue.index;
                    _command_pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

                    auto _hr = vkCreateCommandPool(
                        pGDevice->vk_device,
                        &_command_pool_info,
                        nullptr,
                        &this->_command_pool);
                    if (_hr)
                    {
                        V(S_FALSE, "creating vulkan command pool for graphics device :" + 
                            this->_gDevice->device_info->get_device_name() +
                            " ID: " + std::to_string(this->_gDevice->device_info->get_device_id()),
                            this->_name, 3,
                            false);

                        return S_FALSE;
                    }
                }

                this->_counts = pCount;
                
                if (!this->_counts) return S_FALSE;
                this->_gDevice = pGDevice;
                
                //if we have an existing command buffers
                if (this->_commands.size())
                {
                    vkFreeCommandBuffers(this->_gDevice->vk_device,
                                         this->_gDevice->vk_command_allocator_pool,
                                         static_cast<uint32_t>(_commands.size()),
                                         _commands.data());
                    this->_commands.clear();
                }
                
                //resize them
                this->_commands.resize(this->_counts, VK_NULL_HANDLE);
                
                //create the command buffer from the command pool
                VkCommandBufferAllocateInfo _command_buffer_info = {};
                _command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                _command_buffer_info.pNext = nullptr;
                _command_buffer_info.commandPool = this->_gDevice->vk_command_allocator_pool;
                _command_buffer_info.level = pLevel;
                _command_buffer_info.commandBufferCount = static_cast<uint32_t>(this->_counts);
                
                
                auto _hr = vkAllocateCommandBuffers(this->_gDevice->vk_device,
                                                    &_command_buffer_info,
                                                    this->_commands.data());
                if (_hr)
                {
                    V(S_FALSE, "creating vulkan command buffers for graphics device :" + 
                        this->_gDevice->device_info->get_device_name() +
                      " ID: " + std::to_string(this->_gDevice->device_info->get_device_id()),
                      this->_name, 3,
                      false);
                    
                    return S_FALSE;
                }
                
                return S_OK;
            }
            
			HRESULT begin(_In_ size_t pCommandBufferIndex, _In_ VkCommandBufferUsageFlags pFlags)
			{
				if (pCommandBufferIndex >= this->_commands.size()) return S_FALSE;

				//prepare data for recording command buffers
				const VkCommandBufferBeginInfo _command_buffer_begin_info =
				{
					VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,        //Type;
					nullptr,                                            //Next
					pFlags,												// Flags
					nullptr,
				};

				auto _hr = vkBeginCommandBuffer(this->_commands.at(pCommandBufferIndex), &_command_buffer_begin_info);
				V(_hr, L"begining command buffer for graphics device :" + wolf::system::convert::string_to_wstring(this->_gDevice->device_info->get_device_name()) +
					L" ID: " + std::to_wstring(this->_gDevice->device_info->get_device_id()),
					this->_name, 3,
					false);

				return _hr == VK_SUCCESS ? S_OK : S_FALSE;
			}
            
            HRESULT begin_all(_In_ VkCommandBufferUsageFlags pFlags)
            {
                HRESULT _result = S_OK;
                for (size_t i = 0; i < this->_commands.size(); ++i)
                {
                    if(begin(i, pFlags) == S_FALSE)
                    {
                        _result =  S_FALSE;
                        break;
                    }
                }
                return _result;
            }
            
            HRESULT end(_In_ size_t pCommandBufferIndex)
            {
                if  (pCommandBufferIndex >= this->_commands.size()) return S_FALSE;
                
                auto _hr = vkEndCommandBuffer(this->_commands.at(pCommandBufferIndex));
                V(_hr, L"ending command buffer for graphics device :" + wolf::system::convert::string_to_wstring(this->_gDevice->device_info->get_device_name()) +
                  L" ID: " + std::to_wstring(this->_gDevice->device_info->get_device_id()),
                  this->_name, 3,
                  false);
                
                return _hr == VK_SUCCESS ? S_OK : S_FALSE;
            }
            
            HRESULT end_all()
            {
                HRESULT _result = S_OK;
                for (size_t i = 0; i < this->_commands.size(); ++i)
                {
                    if(end(i) == S_FALSE)
                    {
                        _result =  S_FALSE;
                        break;
                    }
                }
                return _result;
            }
            
            HRESULT flush(_In_ size_t pCommandBufferIndex)
            {
                if  (pCommandBufferIndex >= this->_commands.size()) return S_FALSE;
                
                auto _cmd = this->_commands.at(pCommandBufferIndex);
                
                auto _hr = vkEndCommandBuffer(_cmd);
                if (_hr)
                {
                    V(S_FALSE, L"ending command buffer buffer for graphics device :" + wolf::system::convert::string_to_wstring(this->_gDevice->device_info->get_device_name()) +
                      L" ID: " + std::to_wstring(this->_gDevice->device_info->get_device_id()),
                      this->_name, 3,
                      false);
                    
                    return S_FALSE;
                }
                
                VkSubmitInfo _submit_info = {};
                _submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                _submit_info.commandBufferCount = 1;
                _submit_info.pCommandBuffers = &_cmd;
                    
                // Create fence to ensure that the command buffer has finished executing
                VkFenceCreateInfo _fence_create_info = {};
                _fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
                _fence_create_info.flags = 0;
                    
                VkFence _fence;
                _hr = vkCreateFence(this->_gDevice->vk_device, &_fence_create_info, nullptr, &_fence);
                if (_hr)
                {
                    V(S_FALSE, L"creating fence for command buffer for graphics device :" + wolf::system::convert::string_to_wstring(this->_gDevice->device_info->get_device_name()) +
                      L" ID: " + std::to_wstring(this->_gDevice->device_info->get_device_id()),
                      this->_name, 3,
                      false);
                    return S_FALSE;
                }
                
                // Submit to the queue
                _hr = vkQueueSubmit(this->_gDevice->vk_present_queue.queue,
                                    1,
                                    &_submit_info,
                                    _fence);
                if (_hr)
                {
                    V(S_FALSE, L"submiting queue for graphics device :" + wolf::system::convert::string_to_wstring(this->_gDevice->device_info->get_device_name()) +
                      L" ID: " + std::to_wstring(this->_gDevice->device_info->get_device_id()),
                      this->_name, 3,
                      false);
                    return S_FALSE;
                }
                    
                // Wait for the fence to signal that command buffer has finished executing
                _hr = vkWaitForFences(this->_gDevice->vk_device,
                                      1,
                                      &_fence,
                                      VK_TRUE,
                                      DEFAULT_FENCE_TIMEOUT);
                vkDestroyFence(this->_gDevice->vk_device, _fence, nullptr);
                
                return S_OK;
            }
            
            HRESULT flush_all()
            {
                HRESULT _result = S_OK;
                for (size_t i = 0; i < this->_commands.size(); ++i)
                {
                    if(flush(i) == S_FALSE)
                    {
                        _result =  S_FALSE;
                        break;
                    }
                }
                return _result;
            }
            
            ULONG release()
            {
                vkFreeCommandBuffers(this->_gDevice->vk_device,
                                     this->_gDevice->vk_command_allocator_pool,
                                     static_cast<uint32_t>(_commands.size()),
                                     _commands.data());
                
                this->_commands.clear();
                this->_counts = 0;
                this->_enable = false;
                this->_order = 0;
                this->_gDevice = nullptr;
                
                return 0;
            }
            
#pragma region Getters

            bool get_enable() const
            {
                return this->_enable;
            }
            
            int get_order() const
            {
                return this->_order;
            }
            
            const VkCommandBuffer* get_commands() const
            {
                return this->_commands.data();
            }
            
            const size_t get_commands_size() const
            {
                return this->_commands.size();
            }
            
#pragma endregion 
            
#pragma region Setters
            
            void set_enable(_In_ bool pEnable)
            {
                this->_enable = pEnable;
            }
            
            void set_order(_In_ bool pOrder)
            {
                this->_order = pOrder;
            }
            
#pragma endregion
            
        private:
            std::string                                         _name;
            std::shared_ptr<w_graphics_device>                  _gDevice;
            
#ifdef __VULKAN__
            std::vector<VkCommandBuffer>                        _commands;
            VkCommandPool                                       _command_pool;
#endif
            
            bool                                                _enable;
            int                                                 _order;
            size_t                                              _counts;
        };
    }
}


using namespace wolf::graphics;

w_command_buffer::w_command_buffer() : _pimp(new w_command_buffer_pimp())
{
    _super::set_class_name("w_command_buffer");
}

w_command_buffer::~w_command_buffer()
{
    release();
}

HRESULT w_command_buffer::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const size_t& pCount,
    _In_ const VkCommandBufferLevel& pLevel,
    _In_ const bool& pCreateCommandPool,
    _In_ const w_queue* pCommandPoolQueue)
{
    if(!this->_pimp) return S_FALSE;
    
    return this->_pimp->load(pGDevice, pCount, pLevel, pCreateCommandPool, pCommandPoolQueue);
}

HRESULT w_command_buffer::begin(_In_ const size_t pCommandBufferIndex, _In_ const VkCommandBufferUsageFlags pFlags)
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->begin(pCommandBufferIndex, pFlags);
}

HRESULT w_command_buffer::begin_all(_In_ const VkCommandBufferUsageFlags pFlags)
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->begin_all(pFlags);
}

HRESULT w_command_buffer::end(_In_ const size_t pCommandBufferIndex)
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->end(pCommandBufferIndex);
}

HRESULT w_command_buffer::end_all()
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->end_all();
}

HRESULT w_command_buffer::flush(_In_ const size_t pCommandBufferIndex)
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->flush(pCommandBufferIndex);
}

HRESULT w_command_buffer::flush_all()
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->flush_all();
}

ULONG w_command_buffer::release()
{
    if(_super::get_is_released()) return 0;
    
    SAFE_RELEASE(this->_pimp);
    
    return _super::release();
}

#pragma region Getters

bool w_command_buffer::get_enable() const
{
    if (!_pimp) return 0;
    
    return this->_pimp->get_enable();
}

int  w_command_buffer::get_order() const
{
    if (!_pimp) return 0;
    
    return this->_pimp->get_order();
}

const VkCommandBuffer w_command_buffer::get_command_at(_In_ const size_t pIndex) const
{
    if (!_pimp) return 0;
    
    auto _size = this->_pimp->get_commands_size();
    if(pIndex >= _size) return 0;
    
    auto _cmds = this->_pimp->get_commands();
    return _cmds[pIndex];
}

const VkCommandBuffer* w_command_buffer::get_commands() const
{
    if (!_pimp) return 0;
    
    return this->_pimp->get_commands();
}

const size_t w_command_buffer::get_commands_size() const
{
    if (!_pimp) return 0;
    
    return this->_pimp->get_commands_size();
}

#pragma endregion

#pragma region Setters

void w_command_buffer::set_enable(_In_ const bool pEnable)
{
    if(this->_pimp)
    {
        this->_pimp->set_enable(pEnable);
    }
}

void w_command_buffer::set_order(_In_ bool pOrder)
{
    if(this->_pimp)
    {
        this->_pimp->set_order(pOrder);
    }
}

#pragma endregion
