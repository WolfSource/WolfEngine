#include "w_render_pch.h"
#include "w_render_pass.h"

namespace wolf
{
    namespace graphics
    {
        class w_render_pass_pimp
        {
        public:
            w_render_pass_pimp() :
                _name("w_render_pass"),
                _render_pass(0),
                _layer_count(1)
            {
            }
            
            HRESULT load(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const w_viewport& pViewPort,
                _In_ const w_viewport_scissor& pViewPortScissor,
                _In_ const std::vector<w_attachment_buffer_desc> pAttachmentBufferDescriptions,
                _In_ std::vector<std::vector<w_texture*>> pBufferAttachmentsGroup,
                _In_ const std::vector<VkSubpassDescription>* pSubpassDescriptions,
                _In_ const std::vector<VkSubpassDependency>* pSubpassDependencies)
            {
                const std::string _trace_info = this->_name + "::load";
                
                this->_gDevice = pGDevice;
                this->_viewport = pViewPort;
                this->_viewport_scissor = pViewPortScissor;

                std::vector<VkAttachmentDescription> _attachment_descriptions;
                std::vector<VkSubpassDescription> _subpass_descriptions;
                std::vector<VkSubpassDependency> _subpass_dependencies;

				w_attachment_buffer_desc _color(w_texture_buffer_type::W_TEXTURE_COLOR_BUFFER);
				w_attachment_buffer_desc _depth(w_texture_buffer_type::W_TEXTURE_DEPTH_BUFFER);
				if (!pAttachmentBufferDescriptions.size())
				{
					_attachment_descriptions.push_back(_color.desc);
					_attachment_descriptions.push_back(_depth.desc);
				}
				else
				{
					for (auto _iter : pAttachmentBufferDescriptions)
					{
						_attachment_descriptions.push_back(_iter.desc);
					}
				}

                if (!pSubpassDescriptions)
                {
					VkSubpassDescription _subpass_description =
					{
						0,
						VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
						0,
						nullptr,
						1,
						&_color.ref,
						nullptr,
						&_depth.ref,
						0,
						nullptr
					};

                    _subpass_descriptions.push_back(_subpass_description);
                }
                else
                {
                    _subpass_descriptions = *pSubpassDescriptions;
                }


                if (!pSubpassDependencies)
                {
                    _subpass_dependencies = w_graphics_device::defaults::vk_default_subpass_dependencies;
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
                    &this->_render_pass);
                if (_hr)
                {
                    V(S_FALSE, L"creating render pass for graphics device: " +
                      wolf::system::convert::string_to_wstring(this->_gDevice->device_info->get_device_name()) +
                      L" ID:" + std::to_wstring(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
                    return S_FALSE;
                }
                
                //create frame buffer
                VkFramebuffer _frame_buffer = 0;
                std::vector<VkImageView> _attachments;
                
                auto _window = &pGDevice->output_presentation_window;
                auto _size = pGDevice->output_presentation_window.vk_swap_chain_image_views.size();
                for (size_t i = 0; i < _size; ++i)
                {
                    _attachments.clear();
                    
                    //color
                    _attachments.push_back(_window->vk_swap_chain_image_views[i].view);
                    //depth
                    _attachments.push_back(_window->vk_depth_buffer_image_view.view);
                    
                    VkFramebufferCreateInfo _framebuffer_create_info =
                    {
                        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // Type
                        nullptr,                                    // Next
                        0,                                          // Flags
                        this->_render_pass,                         // Render pass
                        static_cast<uint32_t>(_attachments.size()), // AttachmentCount
                        _attachments.data(),                        // Attachments
                        _window->width,                             // Width
                        _window->height,                            // Height
                        this->_layer_count                          // Layers
                    };
                    
                    auto _hr = vkCreateFramebuffer(pGDevice->vk_device, &_framebuffer_create_info, nullptr, &_frame_buffer);
                    if (_hr)
                    {
                        V(S_FALSE, "creating frame buffer for graphics device: " + pGDevice->device_info->get_device_name() +
                          " ID:" + std::to_string(pGDevice->device_info->get_device_id()), _trace_info, 3, false);
                        return S_FALSE;
                    }
                    
                    this->_frame_buffers.push_back(_frame_buffer);
                    
                    _attachments.clear();
                }
                
                return S_OK;
            }

            void begin(
                _In_ const uint32_t& pFrameBufferIndex,
                _In_ const VkCommandBuffer& pCommandBuffer,
                _In_ const w_color& pClearColor,
                _In_ const float& pClearDepth,
                _In_ const uint32_t& pClearStencil,
                _In_ const VkSubpassContents& pSubpassContents)
            {
                const std::string _trace_info = this->_name + "::begin";
                
                if (pFrameBufferIndex >= this->_frame_buffers.size())
                {
                    V(S_FALSE,
                      "parameter count mismatch, index of frame buffer does not match with index of command buffer for graphics device: " +
                      this->_gDevice->device_info->get_device_name() +
                      " ID:" + std::to_string(this->_gDevice->device_info->get_device_id()),
                      _trace_info, 3, false);
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
                    this->_render_pass,                                     // RenderPass
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
                vkCmdBeginRenderPass(pCommandBuffer, &_render_pass_begin_info, pSubpassContents);
            }

            void begin(
                _In_ const uint32_t& pFrameBufferIndex,
                _In_ const VkCommandBuffer& pCommandBuffer)
            {
                const std::string _trace_info = this->_name + "::begin";
                
                if (pFrameBufferIndex >= this->_frame_buffers.size())
                {
                    V(S_FALSE,
                      "mismatch between index of frame buffer and index of command buffer for graphics device: " +
                      this->_gDevice->device_info->get_device_name() +
                      " ID:" + std::to_string(this->_gDevice->device_info->get_device_id()),
                      _trace_info, 3, false);
                    return;
                }
                
                VkRenderPassBeginInfo _render_pass_begin_info =
                {
                    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,               // Type
                    nullptr,                                                // Next
                    this->_render_pass,                                     // RenderPass
                     this->_frame_buffers[pFrameBufferIndex],               // Framebuffer
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
                    0,                                                  // ClearValueCount
                    nullptr                                             // ClearValues
                };
                vkCmdBeginRenderPass(pCommandBuffer, &_render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
                vkCmdSetViewport(pCommandBuffer, 0, 1, &this->_viewport);
                vkCmdSetScissor(pCommandBuffer, 0, 1, &this->_viewport_scissor);
            }


            void end(_In_ const VkCommandBuffer& pCommandBuffer)
            {
                vkCmdEndRenderPass(pCommandBuffer);
            }

            ULONG release()
            {
                for ( auto _iter :  this->_frame_buffers )
                {
                    vkDestroyFramebuffer(this->_gDevice->vk_device, _iter, nullptr);
                    _iter = 0;
                }
                if (this->_render_pass)
                {
                    vkDestroyRenderPass(this->_gDevice->vk_device,
                        this->_render_pass,
                        nullptr);
                    this->_render_pass = nullptr;
                }
                this->_gDevice = nullptr;
                
                return 0;
            }

#pragma region Getters

            const VkRenderPass get_handle() const
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
            VkRenderPass                                    _render_pass;
            w_viewport                                      _viewport;
            w_viewport_scissor                              _viewport_scissor;
            std::vector<VkFramebuffer>                      _frame_buffers;
            uint32_t                                        _layer_count;
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

HRESULT w_render_pass::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const w_viewport& pViewPort,
	_In_ const w_viewport_scissor& pViewPortScissor,
	_In_ const std::vector<w_attachment_buffer_desc>& pAttachmentBuffersDescriptions,
    _In_ std::vector<std::vector<w_texture*>> pBufferAttachmentsGroup,
	_In_ const std::vector<VkSubpassDescription>* pSubpassDescriptions,
	_In_ const std::vector<VkSubpassDependency>* pSubpassDependencies)
{
	if (!this->_pimp) return S_FALSE;
	return this->_pimp->load(
		pGDevice,
		pViewPort,
		pViewPortScissor,
		pAttachmentBuffersDescriptions,
        pBufferAttachmentsGroup,
		pSubpassDescriptions,
		pSubpassDependencies);
}

void w_render_pass::begin(
    _In_ const uint32_t& pFrameBufferIndex,
    _In_ const VkCommandBuffer& pCommandBuffer,
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

void w_render_pass::begin(
    _In_ const uint32_t& pFrameBufferIndex,
    _In_ const VkCommandBuffer& pCommandBuffer)
{
    if (!this->_pimp) return;
    this->_pimp->begin(
        pFrameBufferIndex,
        pCommandBuffer);
}

void w_render_pass::end(_In_ VkCommandBuffer& pCommandBuffer)
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

const VkRenderPass w_render_pass::get_handle() const
{
    if(!this->_pimp) return 0;
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
