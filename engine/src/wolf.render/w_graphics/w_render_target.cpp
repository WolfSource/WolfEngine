#include "w_render_pch.h"
#include "w_render_target.h"
#include "w_render_pass.h"
#include "w_texture.h"

namespace wolf
{
    namespace graphics
    {
        class w_render_target_pimp
        {
        public:
			w_render_target_pimp() :
                _name("w_render_target"),
                _gDevice(nullptr)
            {
            }

			W_RESULT load(
				_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ w_viewport pViewPort,
				_In_ w_viewport_scissor pViewportScissor,
				_In_ std::vector<w_image_view> pAttachments,
				_In_ const size_t& pCount)
			{
				const std::string _trace_info = this->_name + "::load";

				this->_gDevice = pGDevice;
				this->_viewport = pViewPort;
				this->_viewport_scissor = pViewportScissor;

				W_RESULT _hr = W_PASSED;
				std::vector<std::vector<w_texture*>> _frame_buffers_attachments;
				for (size_t i = 0; i < pCount; ++i)
				{
					std::vector<w_texture*> __attachments;
					//check for attachment and create texture buffer for them
					for (size_t i = 0; i < pAttachments.size(); ++i)
					{
						auto _attachment = pAttachments[i];
						auto _texture_buffer = new (std::nothrow) w_texture();
						if (!_texture_buffer)
						{
							_hr = W_FAILED;
							V(_hr, "allocating memory for texture", _trace_info, 3, false);
							break;
						}

						if (_attachment.attachment_desc.ref.layout == VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
						{
							//color
							_attachment.attachment_desc.desc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
							_texture_buffer->set_buffer_type(w_texture_buffer_type::COLOR);
							_texture_buffer->set_usage_flags(w_image_usage_flag_bits::COLOR_ATTACHMENT_BIT | w_image_usage_flag_bits::SAMPLED_BIT);
						}
						else if (_attachment.attachment_desc.ref.layout == VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
						{
							//depth
							_texture_buffer->set_buffer_type(w_texture_buffer_type::DEPTH);
							_texture_buffer->set_usage_flags(w_image_usage_flag_bits::DEPTH_STENCIL_ATTACHMENT_BIT | w_image_usage_flag_bits::SAMPLED_BIT);
						}

						_texture_buffer->set_format((w_format)_attachment.attachment_desc.desc.format);
						auto _hr = _texture_buffer->initialize(pGDevice, pViewPort.width, pViewPort.height, _attachment.attachment_desc.memory_flag);
						if (_hr == W_FAILED)
						{
							V(W_FAILED, "loading texture", _trace_info, 3, false);
							break;
						}
						_hr = _texture_buffer->load();
						if (_hr == W_FAILED)
						{
							V(W_FAILED, "initializing texture", _trace_info, 3, false);
							break;
						}

						__attachments.push_back(_texture_buffer);
						//store in global vector
						this->_attachment_buffers.push_back(_texture_buffer);
					}
					_frame_buffers_attachments.push_back(__attachments);
				}

				if (_hr == W_FAILED)
				{
					release();
					return W_FAILED;
				}

				//create render pass
				_hr = this->_render_pass.load(pGDevice,
					pViewPort,
					pViewportScissor,
					{ pAttachments });
				if (_hr == W_FAILED)
				{
					release();
					V(W_FAILED, "loading render pass", _trace_info, 3, false);
					return W_FAILED;
				}

				_frame_buffers_attachments.clear();

				return W_PASSED;
			}

			W_RESULT record_command_buffer(
				_In_ wolf::graphics::w_command_buffer* pCommandBuffer,
				_In_ std::function<W_RESULT(void)> pDrawFunction,
				_In_ w_color pClearColor, 
				_In_ const float& pClearDepth, 
				_In_ const uint32_t&  pClearStencil)
			{
				const std::string _trace_info = this->_name + "::record_command_buffer";

				if (!pCommandBuffer) return W_FAILED;

				auto _cmd_size = pCommandBuffer->get_commands_size();
				if (_cmd_size != this->_render_pass.get_number_of_frame_buffers())
				{
					V(W_FAILED, "parameter count mismatch. Number of command buffers must equal to number of frame buffers", _trace_info, 3, false);
					return W_FAILED;
				}

				W_RESULT _hr = W_PASSED;
				for (uint32_t i = 0; i < _cmd_size; ++i)
				{
					pCommandBuffer->begin(i);
					{
						this->_render_pass.begin(
                            i,
							pCommandBuffer,
							pClearColor,
							pClearDepth,
							pClearStencil);
						{
							if (pDrawFunction)
							{
								_hr = pDrawFunction();
							}
						}
						this->_render_pass.end(pCommandBuffer);
					}
					pCommandBuffer->end(i);
				}
				return _hr;
			}
            
            ULONG release()
            {
				for (auto _buffer : this->_attachment_buffers)
				{
					SAFE_RELEASE(_buffer);
				}
				this->_attachment_buffers.clear();
				this->_render_pass.release();
                this->_gDevice = nullptr;
                
                return 0;
            }
                 
#pragma region Getters
            
            const uint32_t get_width() const
            {
                return this->_viewport.width;
            }
            
            const uint32_t get_height() const
            {
				return this->_viewport.height;
            }
            
			w_sampler get_sampler(_In_ size_t pBufferIndex) const
			{
				if (pBufferIndex >= this->_attachment_buffers.size()) return w_sampler();

				auto _t = this->_attachment_buffers.at(pBufferIndex);
				if (_t) return _t->get_sampler();

				return w_sampler();
			}
                       
            w_image_view get_image_view(_In_ size_t pBufferIndex) const
            {
				if (pBufferIndex >= this->_attachment_buffers.size()) return w_image_view();

				auto _t = this->_attachment_buffers.at(pBufferIndex);
				if (_t) return _t->get_image_view();

				return w_image_view();
            }
            
            w_image_type get_image_type(_In_ size_t pBufferIndex) const
            {
				if (pBufferIndex >= this->_attachment_buffers.size()) return w_image_type::W_IMAGE_TYPE_END_RANGE;

				auto _t = this->_attachment_buffers.at(pBufferIndex);
				if (_t) return _t->get_image_type();

				return w_image_type::W_IMAGE_TYPE_END_RANGE;
            }
            
			w_image_view_type get_image_view_type(_In_ size_t pBufferIndex) const
			{
				if (pBufferIndex >= this->_attachment_buffers.size()) return w_image_view_type::W_IMAGE_VIEW_TYPE_END_RANGE;

				auto _t = this->_attachment_buffers.at(pBufferIndex);
				if (_t) return _t->get_image_view_type();

				return w_image_view_type::W_IMAGE_VIEW_TYPE_END_RANGE;
			}

			const w_format get_attachment_format(_In_ size_t pBufferIndex) const
			{
				if (pBufferIndex >= this->_attachment_buffers.size()) return w_format::UNDEFINED;

				auto _t = this->_attachment_buffers.at(pBufferIndex);
				if (_t) return _t->get_format();

				return w_format::UNDEFINED;
			}

			const VkDescriptorImageInfo get_attachment_descriptor_info(_In_ size_t pBufferIndex) const
			{
				if (pBufferIndex >= this->_attachment_buffers.size()) return VkDescriptorImageInfo();

				auto _t = this->_attachment_buffers.at(pBufferIndex);
				if (_t) return _t->get_descriptor_info();

				return VkDescriptorImageInfo();
			}

#pragma endregion
              
        private:
            
            std::string													_name;
            std::shared_ptr<w_graphics_device>							_gDevice;
			w_viewport													_viewport;
			w_viewport_scissor											_viewport_scissor;
			w_render_pass												_render_pass;
			std::vector<wolf::graphics::w_texture*>						_attachment_buffers;	
        };
    }
}

using namespace wolf::graphics;

w_render_target::w_render_target() :
    _pimp(new w_render_target_pimp())
{
	_super::set_class_name("w_render_target");
}

w_render_target::~w_render_target()
{
	release();
}

W_RESULT w_render_target::load(
	_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ w_viewport pViewPort,
	_In_ w_viewport_scissor pViewportScissor,
	_In_ std::vector<w_image_view> pAttachments,
	_In_ const size_t& pCount)
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->load(
		pGDevice,
		pViewPort,
		pViewportScissor,
		pAttachments,
		pCount);
}

W_RESULT w_render_target::record_command_buffer(
	_In_ w_command_buffer* pCommandBuffer,
	_In_ std::function<W_RESULT(void)> pFunction,
	_In_ w_color pClearColor,
	_In_ const float& pClearDepth,
	_In_ const uint32_t&  pClearStencil)
{
	if (!this->_pimp) return W_FAILED;
	return this->_pimp->record_command_buffer(
		pCommandBuffer,
		pFunction,
		pClearColor,
		pClearDepth,
		pClearStencil);
}

W_RESULT w_render_target::save_to_file(_In_z_ const char* pFilename)
{
//    short header[] = { 0x4D42, 0, 0, 0, 0, 26, 0, 12, 0, (short)pWidth, (short)pHeight, 1, 24 };
//    FILE *f;
//    
//#ifdef __WIN32
//    if (!fopen_s(&f, pFilename, "wb"))
//#else
//    f = fopen(pFilename, "wb");
//    if (!f)
//#endif
//    {
//        fwrite(header, 1, sizeof(header), f);
//        fwrite(pData, 1, pWidth * pHeight * 3, f);
//        fclose(f);
//    }

	return W_PASSED;
}

ULONG w_render_target::release()
{
	if (_super::get_is_released()) return 1;
    
    SAFE_RELEASE(this->_pimp);
    
	return _super::release();
}

#pragma region Getters

const uint32_t w_render_target::get_width() const
{
	if (!this->_pimp) return 0;
	return this->_pimp->get_width();
}

const uint32_t w_render_target::get_height() const
{
	if (!this->_pimp) return 0;
	return this->_pimp->get_height();
}

w_sampler w_render_target::get_sampler(_In_ size_t pBufferIndex) const
{
	if (!this->_pimp) return w_sampler();
	return this->_pimp->get_sampler(pBufferIndex);
}

w_image_view w_render_target::get_image_view(_In_ size_t pBufferIndex) const
{
	if (!this->_pimp) return w_image_view();
	return this->_pimp->get_image_view(pBufferIndex);
}

w_image_type w_render_target::get_image_type(_In_ size_t pBufferIndex) const
{
	if (!this->_pimp) return w_image_type::W_IMAGE_TYPE_END_RANGE;
	return this->_pimp->get_image_type(pBufferIndex);
}

w_image_view_type w_render_target::get_image_view_type(_In_ size_t pBufferIndex) const
{
	if (!this->_pimp) return w_image_view_type::W_IMAGE_VIEW_TYPE_END_RANGE;
	return this->_pimp->get_image_view_type(pBufferIndex);
}

const w_format w_render_target::get_attachment_format(_In_ size_t pBufferIndex) const
{
	if (!this->_pimp) return w_format::UNDEFINED;
	return this->_pimp->get_attachment_format(pBufferIndex);
}

const VkDescriptorImageInfo w_render_target::get_attachment_descriptor_info(_In_ size_t pBufferIndex) const
{
	if (!this->_pimp) return VkDescriptorImageInfo();
	return this->_pimp->get_attachment_descriptor_info(pBufferIndex);
}

#pragma endregion
