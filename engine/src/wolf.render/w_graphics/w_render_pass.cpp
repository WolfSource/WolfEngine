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
                         _In_ const std::vector<VkAttachmentDescription>* pAttachmentDescriptions,
                         _In_ const std::vector<VkSubpassDescription>* pSubpassDescriptions,
                         _In_ const std::vector<VkSubpassDependency>* pSubpassDependencies)
            {
                this->_gDevice = pGDevice;
                
                std::vector<VkAttachmentDescription> _attachment_descriptions;
                std::vector<VkSubpassDescription> _subpass_descriptions;
                std::vector<VkSubpassDependency> _subpass_dependencies;
                
                if (!pAttachmentDescriptions)
                {
                    _attachment_descriptions.push_back(w_graphics_device::defaults::vk_default_attachment_description);
                }
                else
                {
                    _attachment_descriptions = *pAttachmentDescriptions;
                }
                
                if (!pSubpassDescriptions)
                {
                    const VkAttachmentReference _attachment_ref[] =
                    {
                        w_graphics_device::defaults::vk_default_color_attachment_reference
                    };
                    
                    VkSubpassDescription _subpass_description =
                    {
                        0,
                        VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
                        0,
                        nullptr,
                        1,
                        _attachment_ref,
                        nullptr,
                        nullptr,
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
                
                auto _hr = vkCreateRenderPass(this->_gDevice ->vk_device,
                                              &_render_pass_create_info,
                                              nullptr,
                                              &this->_render_pass);
                V(_hr, L"creating render pass for graphics device: " +
                  std::wstring(this->_gDevice ->device_name.begin(), this->_gDevice ->device_name.end()) +
                  L" ID:" + std::to_wstring(this->_gDevice ->device_id), this->_name, 3, false, true);
                
                return _hr ? S_FALSE : S_OK;
            }
            
            void begin(_In_ const VkCommandBuffer pCommandBuffer,
                          _In_ const VkFramebuffer pFrameBuffer,
                          _In_ const w_color pClearColor,
                          _In_ const INT32 pStartOffsetX,
                          _In_ const INT32 pStartOffsetY,
                          _In_ const UINT  pWidth,
                          _In_ const UINT  pHeight)
            {
                VkClearValue _vk_clear_color =
                {
                    {
                        pClearColor.r / 255.0f,
                        pClearColor.g / 255.0f,
                        pClearColor.b / 255.0f,
                        pClearColor.a / 255.0f
                    }
                };
                
                VkRenderPassBeginInfo _render_pass_begin_info =
                {
                    VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,    // Type
                    nullptr,                                     // Next
                    this->_render_pass,                          // RenderPass
                    pFrameBuffer,                                // Framebuffer
                    {                                            // RenderArea
                        {
                            pStartOffsetX,                       // X
                            pStartOffsetY                        // Y
                        },
                        {
                            pWidth,                              // Width
                            pHeight,                             // Height
                        }
                    },
                    1,                                           // ClearValueCount
                    &_vk_clear_color                             // ClearValues
                };
                vkCmdBeginRenderPass( pCommandBuffer, &_render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE );
            }
            
            void end(_In_ const VkCommandBuffer pCommandBuffer)
            {
                vkCmdEndRenderPass(pCommandBuffer);
            }
            
            ULONG release()
            {
                if(this->_render_pass)
                {
                    vkDestroyRenderPass(this->_gDevice->vk_device,
                                        this->_render_pass,
                                        nullptr);
                    this->_render_pass = nullptr;
                }
                this->_gDevice = nullptr;
                
                return 1;
            }
            
            
#pragma region Getters
            
            const VkRenderPass get_handle() const
            {
                return this->_render_pass;
            }
            
#pragma endregion
       
#pragma region Setters
            
        
            
#pragma endregion
            
        private:
            std::string                                     _name;
            std::shared_ptr<w_graphics_device>              _gDevice;
            VkRenderPass                                    _render_pass;
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
                            _In_ const std::vector<VkAttachmentDescription>* pAttachmentDescriptions,
                            _In_ const std::vector<VkSubpassDescription>* pSubpassDescriptions,
                            _In_ const std::vector<VkSubpassDependency>* pSubpassDependencies)
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->load(pGDevice,
                             pAttachmentDescriptions,
                             pSubpassDescriptions,
                             pSubpassDependencies);
}

void w_render_pass::begin(_In_ const VkCommandBuffer pCommandBuffer,
           _In_ const VkFramebuffer pFrameBuffer,
           _In_ const w_color pClearColor,
           _In_ const INT32 pStartOffsetX,
           _In_ const INT32 pStartOffsetY,
           _In_ const UINT  pWidth,
           _In_ const UINT  pHeight)
{
    if(!this->_pimp) return;
    this->_pimp->begin(pCommandBuffer,
                              pFrameBuffer,
                              pClearColor,
                              pStartOffsetX,
                              pStartOffsetY,
                              pWidth,
                              pHeight);
}

void w_render_pass::end(_In_ VkCommandBuffer pCommandBuffer)
{
    if(!this->_pimp) return;
    this->_pimp->end(pCommandBuffer);
}

ULONG w_render_pass::release()
{
	if (_super::get_is_released()) return 0;
    
    SAFE_RELEASE(this->_pimp);
    
	return _super::release();
}

#pragma region Getters

const VkRenderPass w_render_pass::get_handle() const
{
    if(!this->_pimp) return 0;
    return this->_pimp->get_handle();
}

#pragma endregion

#pragma region Setters



#pragma endregion
