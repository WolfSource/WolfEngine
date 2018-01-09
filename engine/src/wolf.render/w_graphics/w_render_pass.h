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
#include "w_attachment_buffer_desc.h"

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
                _In_ const w_viewport& pViewPort,
                _In_ const w_viewport_scissor& pViewPortScissor,
                _In_ const std::vector<w_attachment_buffer_desc>& pAttachmentsBuffersDescriptions,
                _In_ const std::vector<VkSubpassDescription>* pSubpassDescriptions = nullptr,
                _In_ const std::vector<VkSubpassDependency>* pSubpassDependencies = nullptr);


            W_EXP void begin(
                _In_ const VkCommandBuffer& pCommandBuffer,
                _In_ const VkFramebuffer& pFrameBuffer,
                _In_ const w_color& pClearColor,
                _In_ const float&   pClearDepth = 1.0f,
                _In_ const uint32_t&    pClearStencil = 0,
                _In_ const VkSubpassContents& pSubpassContents = VK_SUBPASS_CONTENTS_INLINE);

            W_EXP void begin(
                _In_ const VkCommandBuffer& pCommandBuffer,
                _In_ const VkFramebuffer& pFrameBuffer);

            W_EXP void end(_In_ VkCommandBuffer& pCommandBuffer);

            //release all resources
            W_EXP virtual ULONG release() override;


#pragma region Getters

            W_EXP const VkRenderPass get_handle() const;
            W_EXP w_viewport get_viewport() const;
            W_EXP w_viewport_scissor get_viewport_scissor() const;

#pragma endregion

#pragma region Setters

            W_EXP void set_viewport(_In_ const w_viewport& pViewPort);
            W_EXP void set_viewport_scissor(_In_ const w_viewport_scissor& pViewPortScissor);

#pragma endregion

        private:
            typedef system::w_object						_super;
            w_render_pass_pimp*                             _pimp;
        };
    }
}

#endif
