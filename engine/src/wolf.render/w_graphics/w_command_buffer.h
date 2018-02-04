/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_command_buffer
	Description		 : Command buffers
	Comment          : 
*/
#ifndef __W_COMMAND_BUFFER_H__
#define __W_COMMAND_BUFFER_H__

#include "w_graphics_device_manager.h"

namespace wolf
{
	namespace graphics
	{        
        class w_command_buffer_pimp;
        class w_command_buffer : public system::w_object
        {
        public:
            W_EXP w_command_buffer();
            W_EXP ~w_command_buffer();
            
            /*
                pGDevice = graphices device, 
                pCount = count of command buffers to be created, 
                pCreateCommandPool = create seperated command pool,
                pCommandPoolQueue = if pCreateCommandPool set true, then use this w_queue_index for creating command pool
            */
            W_EXP HRESULT load(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice, 
                _In_ const size_t& pCount, 
                _In_ const VkCommandBufferLevel& pLevel = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                _In_ const bool& pCreateCommandPool = false,
                _In_ const w_queue* pCommandPoolQueue = nullptr);
            
            W_EXP HRESULT begin(_In_ const size_t pCommandBufferIndex,
                                _In_ const VkCommandBufferUsageFlags pFlags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
            
            W_EXP HRESULT begin_all(_In_ const VkCommandBufferUsageFlags pFlags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
            
            W_EXP HRESULT end(_In_ const size_t pCommandBufferIndex);
            W_EXP HRESULT end_all();
            
            /*
                Flushing the command buffer will also submit it to the queue and uses a fence to ensure that command has been executed before returning
            */
            W_EXP HRESULT flush(_In_ const size_t pCommandBufferIndex);
            /*
                Flushing all command buffers will also submit these commands buffers to the queue and uses a fence to ensure that all commands have been executed before returning
             */
            W_EXP HRESULT flush_all();
            
            W_EXP ULONG release() override;
            
#pragma region Getters
            
            W_EXP bool get_enable() const;
            W_EXP int  get_order() const;
            W_EXP const VkCommandBuffer* get_commands() const;
            W_EXP const VkCommandBuffer get_command_at(_In_ const size_t pIndex) const;
            W_EXP const size_t get_commands_size() const;
            
#pragma endregion
            
#pragma region Setters
            
            W_EXP void set_enable(_In_ const bool pEnable);
            W_EXP void set_order(_In_ const bool pOrder);
            
#pragma endregion
            
        private:
            typedef system::w_object            _super;
            w_command_buffer_pimp*              _pimp;                               
        };

	}
}

#include "python_exporter/py_command_buffer.h"

#endif
