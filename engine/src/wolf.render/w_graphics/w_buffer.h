/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_buffer
	Description		 : buffer
	Comment          : 
*/

#ifndef __W_BUFFER_H__
#define __W_BUFFER_H__

#include "w_graphics_device_manager.h"

namespace wolf
{
	namespace graphics
	{        
        class w_buffer_pimp;
        class w_buffer : public system::w_object
        {
        public:
            W_EXP w_buffer();
            W_EXP ~w_buffer();
            
            W_EXP HRESULT load_as_staging(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                                          _In_ const uint32_t pBufferSize);
            
            W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                         _In_ const uint32_t pBufferSize,
                         _In_ const VkBufferUsageFlags pUsage,
                         _In_ const VkMemoryPropertyFlags pMemoryFlags);
            
            W_EXP HRESULT bind();

            W_EXP HRESULT set_data(_In_ const void* const pData);          
            
            W_EXP HRESULT copy_to(_In_ w_buffer& pDestinationBuffer);

            W_EXP void* map();
            
            W_EXP void unmap();
            
            W_EXP HRESULT flush(VkDeviceSize pSize = VK_WHOLE_SIZE, VkDeviceSize pOffset = 0);

            W_EXP ULONG release() override;
            
#pragma region Getters
            
            W_EXP const uint32_t                        get_size() const;
            W_EXP const VkBufferUsageFlags            get_usage_flags() const;
            W_EXP const VkMemoryPropertyFlags         get_memory_flags() const;
            W_EXP const VkBuffer                      get_handle() const;
            W_EXP const VkDeviceMemory                get_memory() const;
            W_EXP const VkDescriptorBufferInfo        get_descriptor_info() const;

#pragma endregion
            
        private:
            typedef system::w_object            _super;
            w_buffer_pimp*                      _pimp;
        };

	}
}

#endif
