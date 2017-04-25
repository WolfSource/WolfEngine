/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_frame_buffers.h
	Description		 : create frame buffers
	Comment          :
*/

#ifndef __W_FRAME_BUFFERS_H__
#define __W_FRAME_BUFFERS_H__

#include "w_graphics_device_manager.h"

namespace wolf
{
	namespace graphics
	{
        class w_frame_buffers_pimp;
		class w_frame_buffers : public system::w_object
		{
		public:
			W_EXP w_frame_buffers();
			W_EXP virtual ~w_frame_buffers();

            W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                               _In_ const VkRenderPass pRenderPass,
                               _In_ size_t pNumberOfFrameBuffers,
                               _In_ w_image_view pAttachments[],
                               _In_ uint32_t pFrameBufferWidth,
                               _In_ uint32_t pFrameBufferHeight,
                               _In_ uint32_t pNumberOfLayers);
            
            W_EXP virtual ULONG release() override;
            
            
#pragma region Getters

            const VkFramebuffer get_frame_buffer_at(_In_ size_t pIndex) const;
            const size_t get_frame_buffers_size() const;
            
#pragma endregion

#pragma region Setters


#pragma endregion

		private:
			typedef system::w_object						_super;
            w_frame_buffers_pimp*                           _pimp;
        };
    }
}

#endif
