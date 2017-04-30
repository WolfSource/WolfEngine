/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_render_pass.h
	Description		 : create render pass
	Comment          :
*/

#ifndef __W_RENDER_PASS_H__
#define __W_RENDER_PASS_H__

#include "w_graphics_device_manager.h"

namespace wolf
{
	namespace graphics
	{
        class w_render_pass_pimp;
		class w_render_pass : public system::w_object
		{
		public:
			W_EXP w_render_pass();
			W_EXP virtual ~w_render_pass();

            W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                               _In_ const std::vector<VkAttachmentDescription>* pAttachmentDescriptions = nullptr,
                               _In_ const std::vector<VkSubpassDescription>* pSubpassDescriptions = nullptr,
                               _In_ const std::vector<VkSubpassDependency>* pSubpassDependencies = nullptr);
            
            
            W_EXP void begin(_In_ const VkCommandBuffer pCommandBuffer,
                                _In_ const VkFramebuffer pFrameBuffer,
                                _In_ const w_color pClearColor = w_color::from_hex(w_color::CORNFLOWER_BLUE),
                                _In_ const INT32 pStartOffsetX = 0,
                                _In_ const INT32 pStartOffsetY = 0,
                                _In_ const UINT  pWidth = 800,
                                _In_ const UINT  pHeight = 600);
            
            W_EXP void end(_In_ VkCommandBuffer pCommandBuffer);
            
            //release all resources
            W_EXP virtual ULONG release() override;
            
            
#pragma region Getters

            const VkRenderPass get_handle() const;
            
#pragma endregion

#pragma region Setters

 

#pragma endregion

		private:
			typedef system::w_object						_super;
            w_render_pass_pimp*                             _pimp;
        };
    }
}

#endif
