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
            _name("w_frame_buffer")
            {
                
            }
            
			HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const VkRenderPass pRenderPass,
				_In_ std::vector<w_image_view> pImageViewAttachments,
				_In_ w_image_view* pDepthAttachment,
				_In_ const w_point_t& pFrameSize,
				_In_ uint32_t pNumberOfLayers,
				_In_ bool pLoadAsRenderTarget)
			{
				if (!pRenderPass)
				{
					logger.error("Render Pass can not be nullptr.");
					return S_FALSE;
				}

				this->_gDevice = pGDevice;

				// Create sampler to sample from the attachment in the fragment shader
				VkSamplerCreateInfo _sampler_create_info = {};
				_sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
				_sampler_create_info.magFilter = VK_FILTER_LINEAR;
				_sampler_create_info.minFilter = VK_FILTER_LINEAR;
				_sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
				_sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				_sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				_sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
				_sampler_create_info.mipLodBias = 0.0f;
				_sampler_create_info.maxAnisotropy = 0;
				_sampler_create_info.minLod = 0.0f;
				_sampler_create_info.maxLod = 1.0f;
				_sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

				VkFramebuffer _frame_buffer = 0;
				auto _size = pImageViewAttachments.size();
				_image_view_decriptors.resize(_size);

				for (size_t i = 0; i < _size; ++i)
				{
					std::vector<VkImageView> _attachments;
					_attachments.push_back(pImageViewAttachments[i].view);

					//if need depth add depth
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
							" ID:" + std::to_string(this->_gDevice->device_id), this->_name, 3, false);
						return S_FALSE;
					}

					this->_buffers.push_back(_frame_buffer);

					_attachments.clear();

					if (pLoadAsRenderTarget)
					{
						this->_image_view_decriptors[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
						this->_image_view_decriptors[i].imageView = pImageViewAttachments[i].view;
						_hr = vkCreateSampler(
							this->_gDevice->vk_device,
							&_sampler_create_info,
							nullptr,
							&this->_image_view_decriptors[i].sampler);
						if (_hr != VK_SUCCESS)
						{
							V(S_FALSE, "creating sampler of frame buffer for graphics device: " + this->_gDevice->device_name +
								" ID:" + std::to_string(this->_gDevice->device_id), this->_name, 3, false);
							return S_FALSE;
						}
					}
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

			const VkDescriptorImageInfo get_descriptor_info(_In_ size_t pIndex) const
			{
				if (pIndex < this->_image_view_decriptors.size())
				{
					return this->_image_view_decriptors.at(pIndex);
				}
				return VkDescriptorImageInfo();
			}
            
#pragma endregion
       
#pragma region Setters
            
        
            
#pragma endregion
            
        private:
            std::string                                     _name;
            std::shared_ptr<w_graphics_device>              _gDevice;
            
            std::vector<VkFramebuffer>                      _buffers;
			std::vector<VkDescriptorImageInfo>				_image_view_decriptors;
            
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

HRESULT w_frame_buffer::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const VkRenderPass pRenderPass,
	_In_ std::vector<w_image_view> pImageViewAttachments,
	_In_ w_image_view* pDepthAttachment,
	_In_ const w_point_t& pFrameSize,
	_In_ uint32_t pNumberOfLayers,
	_In_ bool pLoadAsRenderTarget)
{
	if (!this->_pimp) return S_FALSE;
	return this->_pimp->load(
		pGDevice,
		pRenderPass,
		pImageViewAttachments,
		pDepthAttachment,
		pFrameSize,
		pNumberOfLayers,
		pLoadAsRenderTarget);
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

const size_t w_frame_buffer::get_frame_buffers_size() const
{
    if(!this->_pimp) return 0;
    return this->_pimp->get_frame_buffers_size();
}

const VkDescriptorImageInfo w_frame_buffer::get_descriptor_info(_In_ size_t pIndex) const
{
	if (!this->_pimp) return VkDescriptorImageInfo();
	return this->_pimp->get_descriptor_info(pIndex);
}

#pragma endregion

