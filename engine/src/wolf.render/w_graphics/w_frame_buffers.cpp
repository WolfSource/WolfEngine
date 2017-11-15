#include "w_render_pch.h"
#include "w_frame_buffers.h"

namespace wolf
{
    namespace graphics
    {
        class w_frame_buffers_pimp
        {
        public:
            w_frame_buffers_pimp() :
            _name("w_frame_buffers")
            {
                
            }
            
            HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                         _In_ const VkRenderPass pRenderPass,
                         _In_ std::vector<w_image_view> pAttachments,
                         _In_ w_image_view* pDepthAttachment,
                         _In_ const w_point_t& pFrameSize,
                         _In_ uint32_t pNumberOfLayers)
            {
                if (!pRenderPass)
                {
                    logger.error("Render Pass can not be nullptr.");
                    return S_FALSE;
                }
                
                this->_gDevice = pGDevice;
                
                VkFramebuffer _frame_buffer = 0;
                for (size_t i = 0; i < pAttachments.size(); ++i)
                {
                    std::vector<VkImageView> _attachments;
                    _attachments.push_back(pAttachments[i].view);
                    if (pDepthAttachment)
                    {
                        _attachments.push_back(pDepthAttachment->view);//depth
                    }

                    VkFramebufferCreateInfo _framebuffer_create_info =
                    {
                        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // Type
                        nullptr,                                    // Next
                        0,                                          // Flags
                        pRenderPass,								// Render pass
                        static_cast<uint32_t>(_attachments.size()), // AttachmentCount
                        _attachments.data(),						// Attachments
                        pFrameSize.x,                               // Width
                        pFrameSize.y,                               // Height
                        pNumberOfLayers								// Layers
                    };
                    
                    auto _hr = vkCreateFramebuffer(this->_gDevice->vk_device, &_framebuffer_create_info, nullptr, &_frame_buffer);
                    if (_hr != VK_SUCCESS)
                    {
                        V(S_FALSE, "creating frame buffer for graphics device: " + this->_gDevice->device_name +
                          " ID:" + std::to_string(this->_gDevice->device_id), this->_name, 3, false, true);
                        return S_FALSE;
                    }
                    
                    this->_buffers.push_back(_frame_buffer);

                    _attachments.clear();
                }
                
                return S_OK;
            }
            
            ULONG release()
            {
                for ( auto _iter :  this->_buffers )
                {
                    vkDestroyFramebuffer(this->_gDevice->vk_device, _iter, nullptr);
                    _iter = 0;
                }
                this->_gDevice = nullptr;
                
                return 0;
            }
            
            
#pragma region Getters
            
            const VkFramebuffer get_frame_buffer_at(_In_ size_t pIndex) const
            {
                return pIndex >= this->_buffers.size() ? 0 : this->_buffers.at(pIndex);
            }

            const size_t get_frame_buffers_size() const
            {
                return this->_buffers.size();
            }
            
#pragma endregion
       
#pragma region Setters
            
        
            
#pragma endregion
            
        private:
            std::string                                     _name;
            std::shared_ptr<w_graphics_device>              _gDevice;
            
            std::vector<VkFramebuffer>                      _buffers;
            
        };
    }
}

using namespace wolf::graphics;

w_frame_buffers::w_frame_buffers() : _pimp(new w_frame_buffers_pimp())
{
	_super::set_class_name("w_frame_buffers");
}

w_frame_buffers::~w_frame_buffers()
{
	release();
}

HRESULT w_frame_buffers::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                              _In_ const VkRenderPass pRenderPass,
                              _In_ std::vector<w_image_view> pAttachments,
                              _In_ w_image_view* pDepthAttachment,
                              _In_ const w_point_t& pFrameSize,
                              _In_ uint32_t pNumberOfLayers)
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->load(pGDevice,
                             pRenderPass,
                             pAttachments,
                             pDepthAttachment,
                             pFrameSize,
                             pNumberOfLayers);
}

ULONG w_frame_buffers::release()
{
	if (_super::get_is_released()) return 1;
    
    SAFE_RELEASE(this->_pimp);
    
	return _super::release();
}

#pragma region Getters

const VkFramebuffer w_frame_buffers::get_frame_buffer_at(_In_ size_t pIndex) const
{
    if(!this->_pimp) return 0;
    return this->_pimp->get_frame_buffer_at(pIndex);
}

const size_t w_frame_buffers::get_frame_buffers_size() const
{
    if(!this->_pimp) return 0;
    return this->_pimp->get_frame_buffers_size();
}

#pragma endregion

#pragma region Setters



#pragma endregion
