#include "w_render_pch.h"
#include "w_frame_buffer.h"

namespace wolf
{
    namespace graphics
    {
        class w_frame_buffer_pimp
        {
        public:
            w_frame_buffer_pimp() :
                _name("w_frame_buffer"),
                _layer_count(1)
            {
            }
            
            HRESULT load(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const VkRenderPass pRenderPass,
                _In_ const w_output_presentation_window* pPresentationWindow)
            {
                const std::string _trace_info = this->_name + "::load";

                if (!pRenderPass)
                {
                    V(S_FALSE, "inorder to create frame buffer, the render pass or presentation window can not be NULL", _trace_info, 3, false);
                    return S_FALSE;
                }

                this->_gDevice = pGDevice;

                VkFramebuffer _frame_buffer = 0;
                std::vector<VkImageView> _attachments;

                auto _size = pPresentationWindow->vk_swap_chain_image_views.size();
                for (size_t i = 0; i < _size; ++i)
                {
                    _attachments.clear();

                    //color
                    _attachments.push_back(pPresentationWindow->vk_swap_chain_image_views[i].view);
                    //depth
                    _attachments.push_back(pPresentationWindow->vk_depth_buffer_image_view.view);

                    VkFramebufferCreateInfo _framebuffer_create_info =
                    {
                        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // Type
                        nullptr,                                    // Next
                        0,                                          // Flags
                        pRenderPass,								// Render pass
                        static_cast<uint32_t>(_attachments.size()), // AttachmentCount
                        _attachments.data(),						// Attachments
                        pPresentationWindow->width,                 // Width
                        pPresentationWindow->height,                // Height
                        this->_layer_count						    // Layers
                    };

                    auto _hr = vkCreateFramebuffer(pGDevice->vk_device, &_framebuffer_create_info, nullptr, &_frame_buffer);
                    if (_hr != VK_SUCCESS)
                    {
                        V(S_FALSE, "creating frame buffer for graphics device: " + pGDevice->device_info->get_device_name() +
                            " ID:" + std::to_string(pGDevice->device_info->get_device_id()), _trace_info, 3, false);
                        return S_FALSE;
                    }

                    this->_buffers.push_back(_frame_buffer);

                    _attachments.clear();
                }

                return S_OK;
            }

			HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const VkRenderPass pRenderPass,
				_In_ std::vector<std::vector<w_texture*>> pBufferAttachmentsGroup)
			{
				const std::string _trace_info = this->_name + "::load";

				if (!pRenderPass)
				{
					V(S_FALSE, "inorder to create frame buffer, the render pass can not be NULL", _trace_info, 3, false);
					return S_FALSE;
				}

				this->_gDevice = pGDevice;

				HRESULT _hr = S_OK;
				auto _size = pBufferAttachmentsGroup.size();
				for (auto& _frame_buffer_iter : pBufferAttachmentsGroup)
				{
					VkFramebuffer _frame_buffer = 0;
					uint32_t _width = 0;
					uint32_t _height = 0;
					size_t _size = _frame_buffer_iter.size();
					if (!_size) continue;

					//get size of first width and height
					auto _t = _frame_buffer_iter[0];
					if (_t)
					{
						_width = _t->get_width();
						_height = _t->get_height();
					}

					//check size of these attachments
					for (size_t i = 1; i < _size; ++i)
					{
						_t = _frame_buffer_iter[0];
						if (_t && (_t->get_width() != _width || _t->get_height() != _height))
						{
							_hr = S_FALSE;
							V(_hr, "Size of frame buffer's attachments are not equal", _trace_info, 3, false);
							break;
						}
					}

					if (_hr == S_FALSE) break;

					std::vector<VkImageView> _attachments;
					for (auto& _attach_iter : _frame_buffer_iter)
					{
						if (!_attach_iter)
						{
							_hr = S_FALSE;
							V(_hr, "an attachment can not be NULL", _trace_info, 3, false);
							break;
						}

						_attachments.push_back(_attach_iter->get_image_view().view);
					}

					VkFramebufferCreateInfo _framebuffer_create_info =
					{
						VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // Type
						nullptr,                                    // Next
						0,                                          // Flags
						pRenderPass,								// Render pass
						static_cast<uint32_t>(_attachments.size()), // AttachmentCount
						_attachments.data(),						// Attachments
						_width,										// Width
						_height,									// Height
						this->_layer_count  						// Layers
					};

					auto _hr = vkCreateFramebuffer(this->_gDevice->vk_device, &_framebuffer_create_info, nullptr, &_frame_buffer);
					if (_hr != VK_SUCCESS)
					{
						V(S_FALSE, "creating frame buffer for graphics device: " + this->_gDevice->device_info->get_device_name() +
							" ID:" + std::to_string(this->_gDevice->device_info->get_device_id()), this->_name, 3, false);
						return S_FALSE;
					}

					_attachments.clear();

					this->_buffers.push_back(_frame_buffer);
				}

				return _hr;
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

            const size_t get_count() const
            {
                return this->_buffers.size();
            }

#pragma endregion
       
#pragma region Setters
            
            void set_layer_count(_In_ uint32_t pLayerCount)
            {
                this->_layer_count = pLayerCount;
            }
            
#pragma endregion
            
        private:
            std::string                                     _name;
            std::shared_ptr<w_graphics_device>              _gDevice;
            
            std::vector<VkFramebuffer>                      _buffers;
            uint32_t                                        _layer_count;
            
        };
    }
}

using namespace wolf::graphics;

w_frame_buffer::w_frame_buffer() : _pimp(new w_frame_buffer_pimp())
{
	_super::set_class_name("w_frame_buffer");
}

w_frame_buffer::~w_frame_buffer()
{
	release();
}

HRESULT w_frame_buffer::load(
    _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const VkRenderPass pRenderPass,
    _In_ const w_output_presentation_window* pPresentationWindow)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->load(
        pGDevice,
        pRenderPass,
        pPresentationWindow);
}

HRESULT w_frame_buffer::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const VkRenderPass pRenderPass,
	_In_ std::vector<std::vector<w_texture*>> pBufferAttachmentsGroup)
{
	if (!this->_pimp) return S_FALSE;
	return this->_pimp->load(
		pGDevice,
		pRenderPass,
		pBufferAttachmentsGroup);
}

ULONG w_frame_buffer::release()
{
	if (_super::get_is_released()) return 1;
    
    SAFE_RELEASE(this->_pimp);
    
	return _super::release();
}

#pragma region Getters

const VkFramebuffer w_frame_buffer::get_frame_buffer_at(_In_ size_t pIndex) const
{
    if(!this->_pimp) return 0;
    return this->_pimp->get_frame_buffer_at(pIndex);
}

const size_t w_frame_buffer::get_count() const
{
    if(!this->_pimp) return 0;
    return this->_pimp->get_count();
}

#pragma endregion

