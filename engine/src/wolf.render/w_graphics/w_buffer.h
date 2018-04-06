/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_buffer
	Description		 : graphics buffer
	Comment          : 
*/

#ifndef __W_BUFFER_H__
#define __W_BUFFER_H__

#include <w_graphics_headers.h>
#include <w_render_export.h>

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
            
            W_EXP W_RESULT load_as_staging(
				_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const uint32_t& pBufferSize);
            
            W_EXP W_RESULT load(
				_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const uint32_t& pBufferSize,
                _In_ const uint32_t& pUsage,
                _In_ const uint32_t& pMemoryFlags);
            
            W_EXP W_RESULT bind();    
            
            W_EXP W_RESULT copy_to(_In_ w_buffer& pDestinationBuffer);

            W_EXP void* map();
            
            W_EXP void unmap();
            
            W_EXP W_RESULT flush(VkDeviceSize pSize = VK_WHOLE_SIZE, VkDeviceSize pOffset = 0);

            W_EXP ULONG release() override;
            
#pragma region Getters
            
            W_EXP const uint32_t                      get_size() const;
            W_EXP const uint32_t					  get_usage_flags() const;
            W_EXP const uint32_t				      get_memory_flags() const;
            W_EXP const w_buffer_handle               get_buffer_handle() const;
            W_EXP const w_device_memory               get_memory() const;
            W_EXP const w_descriptor_buffer_info      get_descriptor_info() const;

#pragma endregion

#pragma region Setters

			W_EXP W_RESULT set_data(_In_ const void* const pData);

#pragma endregion

#ifdef __PYTHON__

			W_RESULT py_load(
				_In_ boost::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
				_In_ const uint32_t& pBufferSize,
				_In_ const uint32_t& pUsage,
				_In_ const uint32_t& pMemoryFlags)
			{
				if (!pGDevice.get()) return W_FAILED;
				auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

				auto _hr = load(_gDevice, pBufferSize, pUsage, pMemoryFlags);

				_gDevice.reset();
				return _hr;
			}

#endif
            
        private:
            typedef system::w_object            _super;
            w_buffer_pimp*                      _pimp;
        };

	}
}

#include "python_exporter/py_buffer.h"

#endif
