#include "w_render_pch.h"
#include "w_render_pass.h"
#include <assert.h>

namespace wolf
{
    namespace graphics
    {
        class w_render_pass_pimp
        {
        public:
            w_render_pass_pimp() :
                _name("w_render_pass"),
                _layer_count(1),
				_depth_stencil_enabled(false)
            {
            }
            
			W_RESULT load(
				_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const w_viewport& pViewPort,
				_In_ const w_viewport_scissor& pViewPortScissor,
				_In_ std::vector<std::vector<w_image_view>> pAttachments,
				_In_ const std::vector<VkSubpassDescription>* pSubpassDescriptions,
				_In_ const std::vector<VkSubpassDependency>* pSubpassDependencies)
			{
				const char* _trace_info = (this->_name + "::load").c_str();

				if (!pAttachments.size())
				{
					V(W_FAILED,
						w_log_type::W_ERROR,
						"attachments not defined for render pass of graphics device: {}. trace info: {}",
						this->_gDevice->get_info(),
						_trace_info);
					return W_FAILED;
				}

				this->_gDevice = pGDevice;
				this->_viewport = pViewPort;
				this->_viewport_scissor = pViewPortScissor;

				std::vector<VkAttachmentDescription> _attachment_descriptions;
				std::vector<VkSubpassDescription> _subpass_descriptions;
				std::vector<VkSubpassDependency> _subpass_dependencies;

				//assign first frame buffer attachment description
				for (auto _iter : pAttachments[0])
				{
					_attachment_descriptions.push_back(_iter.attachment_desc.desc);
				}

				this->_depth_stencil_enabled = false;
				if (!pSubpassDescriptions)
				{
					auto _color_buffer_desc = w_attachment_buffer_desc::create_color_desc_buffer();
					auto _depth_buffer_desc = w_attachment_buffer_desc::create_depth_desc_buffer();

					this->_depth_stencil_enabled = true;
					VkSubpassDescription _subpass_description =
					{
						0,
						VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
						0,
						nullptr,
						1,
						&(_color_buffer_desc.ref),
						nullptr,
						&(_depth_buffer_desc.ref),
						0,
						nullptr
					};

					_subpass_descriptions.push_back(_subpass_description);
				}
				else
				{
					_subpass_descriptions = *pSubpassDescriptions;

					//check attachments for depth stencil
					for (auto& _sub_pass : _subpass_descriptions)
					{
						if (_sub_pass.pDepthStencilAttachment && _sub_pass.pDepthStencilAttachment->layout)
						{
							//contains depth stencil attachment
							this->_depth_stencil_enabled = true;
							break;
						}
					}
				}

				if (!pSubpassDependencies)
				{
                    _subpass_dependencies = w_graphics_device::defaults_states::vk_default_subpass_dependencies;
				}
				else
				{
					_subpass_dependencies = *pSubpassDependencies;
				}

				const VkRenderPassCreateInfo _render_pass_create_info =
				{
					VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,                              //sType
					nullptr,                                                                // Next
					0,                                                                      // Flags

					static_cast<uint32_t>(_attachment_descriptions.size()),                 // AttachmentCount
					_attachment_descriptions.data(),                                        // Attachments

					static_cast<uint32_t>(_subpass_descriptions.size()),                    // SubpassCount
					_subpass_descriptions.data(),                                           // Subpasses

					static_cast<uint32_t>(_subpass_dependencies.size()),                    // DependencyCount
					_subpass_dependencies.data()                                            // Dependencies
				};

				auto _hr = vkCreateRenderPass(this->_gDevice->vk_device,
					&_render_pass_create_info,
					nullptr,
					&this->_render_pass.handle);
				if (_hr)
				{
					V(W_FAILED, 
						w_log_type::W_ERROR,
						"creating render pass for graphics device: {}. trace info: {}",
						this->_gDevice->get_info(),
						_trace_info);
					return W_FAILED;
				}

				//create frame buffers
				W_RESULT __hr = W_PASSED;
				std::vector<VkImageView> _frame_buffer_attachments;
				for (auto& _attach : pAttachments)
				{
					for (auto& _iter : _attach)
					{
						_frame_buffer_attachments.push_back(_iter.view);
					}

					uint32_t _width = _attach[0].width;
					uint32_t _height = _attach[0].height;

					for (auto& _iter : _attach)
					{
						if (_width != _iter.width || _height != _iter.height)
						{
							__hr = W_FAILED;
							V(__hr,
								w_log_type::W_ERROR,
								"width and height of all attachments must be same. creating frame buffers for graphics device: {}. trace info: {}",
								this->_gDevice->get_info(),
								_trace_info);
							break;
						}
					}

					if (__hr == W_FAILED) break;

					VkFramebufferCreateInfo _framebuffer_create_info =
					{
						VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,					// Type
						nullptr,													// Next
						0,															// Flags
						this->_render_pass.handle,  								// Render pass
						static_cast<uint32_t>(_frame_buffer_attachments.size()),	// AttachmentCount
						_frame_buffer_attachments.data(),							// Attachments
						_width,														// Width
						_height,													// Height
						this->_layer_count											// Layers
					};

					VkFramebuffer _frame_buffer = nullptr;
					_hr = vkCreateFramebuffer(this->_gDevice->vk_device, &_framebuffer_create_info, nullptr, &_frame_buffer);
					if (_hr == VkResult::VK_SUCCESS)
					{
						this->_frame_buffers.push_back(_frame_buffer);
					}
					else
					{
						__hr = W_FAILED;
						V(__hr,
							w_log_type::W_ERROR,
							"creating frame buffer for graphics device: {}. trace info: {}",
							this->_gDevice->get_info(),
							_trace_info);
						break;
					}
					_frame_buffer_attachments.clear();
				}

				return __hr;
			}

			void begin(
				_In_ const uint32_t& pFrameBufferIndex,
				_In_ const w_command_buffer pCommandBuffer,
				_In_ const w_color& pClearColor,
				_In_ const float& pClearDepth,
				_In_ const uint32_t& pClearStencil,
				_In_ const VkSubpassContents& pSubpassContents)
			{
				const std::string _trace_info = this->_name + "::begin";

				if (pFrameBufferIndex >= this->_frame_buffers.size())
				{
					V(W_FAILED,
						w_log_type::W_ERROR,
						"parameter count mismatch, index of frame buffer does not match with index of command buffer for graphics device: {}. trace info: {}",
						this->_gDevice->get_info(),
						_trace_info);
					return;
				}

				std::array<VkClearValue, 2> _clear_values = {};
				_clear_values[0].color =
				{
					pClearColor.r / 255.0f,
					pClearColor.g / 255.0f,
					pClearColor.b / 255.0f,
					pClearColor.a / 255.0f
				};
				_clear_values[1].depthStencil.depth = pClearDepth;
				_clear_values[1].depthStencil.stencil = pClearStencil;

				VkRenderPassBeginInfo _render_pass_begin_info =
				{
					VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,               // Type
					nullptr,                                                // Next
					this->_render_pass.handle,                              // RenderPass
					this->_frame_buffers[pFrameBufferIndex],                // Framebuffer
					{                                                       // RenderArea
						{
							static_cast<int32_t>(this->_viewport.x),        // X
							static_cast<int32_t>(this->_viewport.y)         // Y
						},
						{
							static_cast<uint32_t>(this->_viewport.width),   // Width
							static_cast<uint32_t>(this->_viewport.height),  // Height
						}
					},
					static_cast<uint32_t>(_clear_values.size()),            // ClearValueCount
					_clear_values.data()                                    // ClearValues
				};

				auto _cmd = pCommandBuffer.handle;
				vkCmdBeginRenderPass(_cmd, &_render_pass_begin_info, pSubpassContents);
				vkCmdSetViewport(_cmd, 0, 1, &this->_viewport);
				vkCmdSetScissor(_cmd, 0, 1, &this->_viewport_scissor);
			}

			void end(_In_ const w_command_buffer pCommandBuffer)
			{
				vkCmdEndRenderPass(pCommandBuffer.handle);
			}

            ULONG release()
            {
				for (auto _iter : this->_frame_buffers)
				{
					vkDestroyFramebuffer(this->_gDevice->vk_device, _iter, nullptr);
					_iter = 0;
				}
                if (this->_render_pass.handle)
                {
                    vkDestroyRenderPass(this->_gDevice->vk_device,
                        this->_render_pass.handle,
                        nullptr);
                    this->_render_pass.handle = nullptr;
                }
                this->_gDevice = nullptr;
                
                return 0;
            }

#pragma region Getters

            const w_render_pass_handle get_handle() const
            {
                return this->_render_pass;
            }

            w_viewport get_viewport() const
            {
                return this->_viewport;
            }

            w_viewport_scissor get_viewport_scissor() const
            {
                return this->_viewport_scissor;
            }
        
			const size_t get_number_of_frame_buffers() const
			{
				return this->_frame_buffers.size();
			}

			const bool get_depth_stencil_enabled() const
			{
				return this->_depth_stencil_enabled;
			}
            
#pragma endregion

#pragma region Setters

            void set_viewport(_In_ const w_viewport& pViewPort)
            {
                this->_viewport = pViewPort;
            }

            void set_viewport_scissor(_In_ const w_viewport_scissor& pViewPortScissor)
            {
                this->_viewport_scissor = pViewPortScissor;
            }

#pragma endregion

#pragma endregion

        private:
            std::string                                     _name;
            std::shared_ptr<w_graphics_device>              _gDevice;
			w_render_pass_handle                            _render_pass;
            w_viewport                                      _viewport;
            w_viewport_scissor                              _viewport_scissor;
            std::vector<VkFramebuffer>                      _frame_buffers;
            uint32_t                                        _layer_count;
			bool											_depth_stencil_enabled;
        };
    }
}

using namespace wolf::graphics;

w_render_pass::w_render_pass() : _pimp(new w_render_pass_pimp())
{
	_super::set_class_name("w_render_pass");
}

w_render_pass::~w_render_pass()
{
	release();
}

W_RESULT w_render_pass::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const w_viewport& pViewPort,
	_In_ const w_viewport_scissor& pViewPortScissor,
	_In_ std::vector<std::vector<w_image_view>> pAttachments,
	_In_ const std::vector<VkSubpassDescription>* pSubpassDescriptions,
	_In_ const std::vector<VkSubpassDependency>* pSubpassDependencies)
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->load(
		pGDevice,
		pViewPort,
		pViewPortScissor,
		pAttachments,
		pSubpassDescriptions,
		pSubpassDependencies);
}

void w_render_pass::begin(
    _In_ const uint32_t& pFrameBufferIndex,
    _In_ const w_command_buffer& pCommandBuffer,
    _In_ const w_color& pClearColor,
    _In_ const float&  pClearDepth,
    _In_ const uint32_t&  pClearStencil,
    _In_ const VkSubpassContents& pSubpassContents)
{
    if(!this->_pimp) return;
    this->_pimp->begin(
        pFrameBufferIndex,
        pCommandBuffer,
        pClearColor,
        pClearDepth,
        pClearStencil,
        pSubpassContents);
}

void w_render_pass::end(_In_ const w_command_buffer& pCommandBuffer)
{
    if(!this->_pimp) return;
    this->_pimp->end(pCommandBuffer);
}

ULONG w_render_pass::release()
{
	if (_super::get_is_released()) return 1;
    
    SAFE_RELEASE(this->_pimp);
    
	return _super::release();
}

#pragma region Getters

const w_render_pass_handle w_render_pass::get_handle() const
{
    if(!this->_pimp) return w_render_pass_handle();
    return this->_pimp->get_handle();
}

w_viewport w_render_pass::get_viewport() const
{
    if (!this->_pimp) return w_viewport();
    return this->_pimp->get_viewport();
}

w_viewport_scissor w_render_pass::get_viewport_scissor() const
{
    if (!this->_pimp) return w_viewport_scissor();
    return this->_pimp->get_viewport_scissor();
}

const size_t w_render_pass::get_number_of_frame_buffers() const
{
    if (!this->_pimp) return 0;
    return this->_pimp->get_number_of_frame_buffers();
}

const bool w_render_pass::get_depth_stencil_enabled() const
{
	return this->_pimp ? this->_pimp->get_depth_stencil_enabled() : false;
}

#pragma endregion

#pragma region Setters

void w_render_pass::set_viewport(_In_ const w_viewport& pViewPort)
{
    if (this->_pimp)
    {
        this->_pimp->set_viewport(pViewPort);
    }
}

void w_render_pass::set_viewport_scissor(_In_ const  w_viewport_scissor& pViewPortScissor)
{
    if (this->_pimp)
    {
        this->_pimp->set_viewport_scissor(pViewPortScissor);
    }
}

#pragma endregion
