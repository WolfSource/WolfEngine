/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_render_target.h
	Description		 : render to texture
	Comment          : 
*/

#ifndef __W_RENDER_TARGET_H__
#define __W_RENDER_TARGET_H__

#include "w_graphics_device_manager.h"
#include "w_command_buffer.h"
#include "w_attachment_buffer_desc.h"

namespace wolf
{
	namespace graphics
	{			
        class w_render_target_pimp;
		class w_render_target : public system::w_object
		{
		public:
			W_EXP w_render_target();
			W_EXP virtual ~w_render_target();

			W_EXP HRESULT load(
				_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ w_viewport pViewPort,
				_In_ w_viewport_scissor pViewportScissor,
				_In_ std::vector<w_attachment_buffer_desc> pAttachmentBuffersDescriptions,
				_In_ const size_t& pCount);
            
			W_EXP HRESULT record_command_buffer(_In_ w_command_buffer* pCommandBuffer,
				_In_ w_color pClearColor = w_color::PURPLE(), 
				_In_ const float& pClearDepth = 1.0f, 
				_In_ const UINT&  pClearStencil = 0);

            //flush staging buffer
            W_EXP HRESULT flush_staging_buffer(_In_ size_t pBufferIndex);
			//flush all staging buffers
			W_EXP HRESULT flush_all_staging_buffers();

            //save texture as bitmap file
            W_EXP HRESULT save_to_file(_In_z_ const char* pFileName);

			//release all resources
			W_EXP virtual ULONG release() override;

#pragma region Getters
			
			//get width of render target
			W_EXP const uint32_t get_width() const;
			//get height of render target
			W_EXP const uint32_t get_height() const;		
			//get sampler of render target's buffers
			W_EXP VkSampler get_sampler(_In_ size_t pBufferIndex) const;
			//get image view handles of render target's buffers
			W_EXP w_image_view get_image_view(_In_ size_t pBufferIndex) const;
			//get image format of render target's buffers
			W_EXP VkImageType get_image_type(_In_ size_t pBufferIndex) const;
			//get image view type render target's buffer
			W_EXP VkImageViewType get_image_view_type(_In_ size_t pBufferIndex) const;
			//get pointer to the staging data
			W_EXP void* get_pointer_to_staging_data_of_attachment(_In_ size_t pBufferIndex);
			//get format of attached buffer
			W_EXP const VkFormat get_attachment_format(_In_ size_t pBufferIndex) const;
			//get description info of attached buffer
			W_EXP const VkDescriptorImageInfo get_attachment_descriptor_info(_In_ size_t pBufferIndex) const;
            
#pragma endregion
			
		private:
			typedef system::w_object						_super;
            w_render_target_pimp*                           _pimp;
        };
    }
}

#endif
