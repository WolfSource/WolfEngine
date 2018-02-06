#include "w_render_pch.h"

namespace wolf
{
    namespace graphics
    {
        class w_frame_buffer_pimp
        {

            
            

//            HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
//                _In_ const VkRenderPass pRenderPass,
//                _In_ std::vector<std::vector<w_texture*>> pBufferAttachmentsGroup)
//            {
//                const std::string _trace_info = this->_name + "::load";
//
//                if (!pRenderPass)
//                {
//                    V(S_FALSE, "inorder to create frame buffer, the render pass can not be NULL", _trace_info, 3, false);
//                    return S_FALSE;
//                }
//
//                this->_gDevice = pGDevice;
//
//                HRESULT _hr = S_OK;
//                for (auto& _frame_buffer_iter : pBufferAttachmentsGroup)
//                {
//                    VkFramebuffer _frame_buffer = 0;
//                    uint32_t _width = 0;
//                    uint32_t _height = 0;
//                    size_t _size = _frame_buffer_iter.size();
//                    if (!_size) continue;
//
//                    //get size of first width and height
//                    auto _t = _frame_buffer_iter[0];
//                    if (_t)
//                    {
//                        _width = _t->get_width();
//                        _height = _t->get_height();
//                    }
//
//                    //check size of these attachments
//                    for (size_t i = 1; i < _size; ++i)
//                    {
//                        _t = _frame_buffer_iter[0];
//                        if (_t && (_t->get_width() != _width || _t->get_height() != _height))
//                        {
//                            _hr = S_FALSE;
//                            V(_hr, "Size of frame buffer's attachments are not equal", _trace_info, 3, false);
//                            break;
//                        }
//                    }
//
//                    if (_hr == S_FALSE) break;
//
//                    std::vector<VkImageView> _attachments;
//                    for (auto& _attach_iter : _frame_buffer_iter)
//                    {
//                        if (!_attach_iter)
//                        {
//                            _hr = S_FALSE;
//                            V(_hr, "an attachment can not be NULL", _trace_info, 3, false);
//                            break;
//                        }
//
//                        _attachments.push_back(_attach_iter->get_image_view().view);
//                    }
//
//                    VkFramebufferCreateInfo _framebuffer_create_info =
//                    {
//                        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // Type
//                        nullptr,                                    // Next
//                        0,                                          // Flags
//                        pRenderPass,                                // Render pass
//                        static_cast<uint32_t>(_attachments.size()), // AttachmentCount
//                        _attachments.data(),                        // Attachments
//                        _width,                                        // Width
//                        _height,                                    // Height
//                        this->_layer_count                          // Layers
//                    };
//
//                    auto _hr = vkCreateFramebuffer(this->_gDevice->vk_device, &_framebuffer_create_info, nullptr, &_frame_buffer);
//                    if (_hr != VK_SUCCESS)
//                    {
//                        V(S_FALSE, "creating frame buffer for graphics device: " + this->_gDevice->device_info->get_device_name() +
//                            " ID:" + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
//                        return S_FALSE;
//                    }
//
//                    _attachments.clear();
//
//                    this->_buffers.push_back(_frame_buffer);
//                }
//
//                return _hr;
//            }

            
        private:
 
            
        };
    }
}


