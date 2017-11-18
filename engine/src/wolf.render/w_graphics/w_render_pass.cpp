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
                _render_pass(0)
            {
            }

            HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const w_viewport& pViewPort,
                _In_ const w_viewport_scissor& pViewPortScissor,
                _In_ const std::vector<VkAttachmentDescription> pAttachmentDescriptions,
                _In_ const std::vector<VkSubpassDescription>* pSubpassDescriptions,
                _In_ const std::vector<VkSubpassDependency>* pSubpassDependencies)
            {
                this->_gDevice = pGDevice;
                this->_viewport = pViewPort;
                this->_viewport_scissor = pViewPortScissor;

                std::vector<VkAttachmentDescription> _attachment_descriptions;
                std::vector<VkSubpassDescription> _subpass_descriptions;
                std::vector<VkSubpassDependency> _subpass_dependencies;

                if (!pAttachmentDescriptions.size())
                {
                    _attachment_descriptions.push_back(w_graphics_device::w_render_pass_attachments::color_attachment_description);
                }
                else
                {
                    _attachment_descriptions.insert(_attachment_descriptions.end(), pAttachmentDescriptions.begin(), pAttachmentDescriptions.end());
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
                        &w_graphics_device::w_render_pass_attachments::color_attachment_reference,
                        nullptr,
                        &w_graphics_device::w_render_pass_attachments::depth_attachment_reference,
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
                V(_hr, L"creating render pass for graphics device: " +
                    std::wstring(this->_gDevice->device_name.begin(), this->_gDevice->device_name.end()) +
                    L" ID:" + std::to_wstring(this->_gDevice->device_id), this->_name, 3, false);

                return _hr ? S_FALSE : S_OK;
            }

            void begin(_In_ const VkCommandBuffer& pCommandBuffer,
                _In_ const VkFramebuffer& pFrameBuffer,
                _In_ const w_color& pClearColor,
                _In_ const float&   pClearDepth,
                _In_ const UINT&    pClearStencil,
                _In_ const VkSubpassContents& pSubpassContents)
            {
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
                    pFrameBuffer,                                           // Framebuffer
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

            void begin(_In_ const VkCommandBuffer& pCommandBuffer,
                _In_ const VkFramebuffer& pFrameBuffer)
            {
                VkRenderPassBeginInfo _render_pass_begin_info =
                {
                    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,               // Type
                    nullptr,                                                // Next
                    this->_render_pass,                                     // RenderPass
                    pFrameBuffer,                                           // Framebuffer
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

#pragma region Setters



#pragma endregion

        private:
            std::string                                     _name;
            std::shared_ptr<w_graphics_device>              _gDevice;
            VkRenderPass                                    _render_pass;
            w_viewport                                      _viewport;
            w_viewport_scissor                              _viewport_scissor;
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
                            _In_ const std::vector<VkAttachmentDescription>& pAttachmentDescriptions,
                            _In_ const std::vector<VkSubpassDescription>* pSubpassDescriptions,
                            _In_ const std::vector<VkSubpassDependency>* pSubpassDependencies)
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->load(pGDevice,
                             pViewPort,
                             pViewPortScissor,
                             pAttachmentDescriptions,
                             pSubpassDescriptions,
                             pSubpassDependencies);
}

void w_render_pass::begin(_In_ const VkCommandBuffer& pCommandBuffer,
           _In_ const VkFramebuffer& pFrameBuffer,
           _In_ const w_color& pClearColor,
           _In_ const float&   pClearDepth,
           _In_ const UINT&    pClearStencil,
           _In_ const VkSubpassContents& pSubpassContents)
{
    if(!this->_pimp) return;
    this->_pimp->begin(pCommandBuffer,
                              pFrameBuffer,
                              pClearColor,
                              pClearDepth,
                              pClearStencil,
                              pSubpassContents);
}

void w_render_pass::begin(_In_ const VkCommandBuffer& pCommandBuffer,
    _In_ const VkFramebuffer& pFrameBuffer)
{
    if (!this->_pimp) return;
    this->_pimp->begin(
        pCommandBuffer,
        pFrameBuffer);
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

#pragma endregion

#pragma region Setters



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
