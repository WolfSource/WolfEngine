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
#include "w_command_buffers.h"

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

            //load render pass which contains frame buffers
            W_EXP W_RESULT load(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const w_viewport& pViewPort,
                _In_ const w_viewport_scissor& pViewPortScissor,
				_In_ std::vector<std::vector<w_image_view>> pAttachments,
                _In_ const std::vector<VkSubpassDescription>* pSubpassDescriptions = nullptr,
                _In_ const std::vector<VkSubpassDependency>* pSubpassDependencies = nullptr);

            //begin render pass
            W_EXP void begin(
                _In_ const uint32_t& pFrameBufferIndex,
                _In_ const w_command_buffer& pCommandBuffer,
                _In_ const w_color& pClearColor,
                _In_ const float& pClearDepth = 1.0f,
                _In_ const uint32_t& pClearStencil = 0,
                _In_ const VkSubpassContents& pSubpassContents = VK_SUBPASS_CONTENTS_INLINE);

            //end render pass
            W_EXP void end(_In_ const w_command_buffer& pCommandBuffer);

            //release all resources
            W_EXP virtual ULONG release() override;


#pragma region Getters

            W_EXP const VkRenderPass get_handle() const;
            W_EXP w_viewport get_viewport() const;
            W_EXP w_viewport_scissor get_viewport_scissor() const;
            W_EXP const size_t get_number_of_frame_buffers() const;
			W_EXP const bool get_depth_stencil_enabled() const;

#pragma endregion

#pragma region Setters

            W_EXP void set_viewport(_In_ const w_viewport& pViewPort);
            W_EXP void set_viewport_scissor(_In_ const w_viewport_scissor& pViewPortScissor);

#pragma endregion

#ifdef __PYTHON__

			W_RESULT py_load(
				_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const w_viewport& pViewPort,
				_In_ const w_viewport_scissor& pViewPortScissor,
				_In_ boost::python::list pAttachments)
			{
				//create render pass attchaments
				if (!pGDevice.get()) return W_FAILED;

				auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

				std::vector<std::vector<w_image_view>> _render_pass_attachments;
				for (size_t i = 0; i < len(pAttachments); ++i)
				{
					boost::python::extract<boost::python::list> _att(pAttachments[i]);
					if (_att.check())
					{
						std::vector<w_image_view> _image_views;
						auto _1_att = _att();
						for (size_t j = 0; j < len(_1_att); j++)
						{
							boost::python::extract<w_image_view> _image_view(_1_att[j]);
							if (_image_view.check())
							{
								_image_views.push_back(_image_view());
							}
						}
						if (_image_views.size())
						{
							_render_pass_attachments.push_back(_image_views);
						}
					}
				}
				if (!_render_pass_attachments.size()) return W_FAILED;

				auto _hr = load(
					_gDevice,
					pViewPort,
					pViewPortScissor,
					_render_pass_attachments);

				//reset local shared_ptr
				_gDevice.reset();

				return _hr;
			}

#endif

        private:
            typedef system::w_object						_super;
            w_render_pass_pimp*                             _pimp;
        };
    }
}

#include "python_exporter/py_render_pass.h"

#endif
