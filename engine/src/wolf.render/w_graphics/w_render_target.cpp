#include "w_render_pch.h"
#include "w_render_target.h"
#include "w_command_buffer.h"
#include "w_render_pass.h"
#include "w_frame_buffer.h"
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

            ~w_render_target_pimp()
            {
                release();
            }

            HRESULT load(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ wolf::graphics::w_viewport pViewPort,
                _In_ wolf::graphics::w_viewport_scissor pViewportScissor,
                _In_ std::vector<wolf::graphics::w_attachment_desc> pAttachmentsDescriptions,
                _In_ const VkMemoryPropertyFlags pMemoryPropertyFlags)
            {
                const std::string _trace_info = this->_name + "::load";

                this->_gDevice = pGDevice;

                HRESULT _hr = S_OK;
                int _depth_buffer_index = -1;
                //check for attachment and create texture buffer for them
                for (size_t i = 0; i < pAttachmentsDescriptions.size(); ++i)
                {
                    auto _attachment = pAttachmentsDescriptions[i];
                    auto _texture_buffer = new (std::nothrow) w_texture();
                    if (!_texture_buffer)
                    {
                        _hr = S_FALSE;
                        V(_hr, "allocating memory for texture", _trace_info, 3, false);
                        break;
                    }

                    if (_attachment.ref.layout == VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                    {
                        //create color
                        _texture_buffer->set_buffer_type(w_texture_buffer_type::W_TEXTURE_COLOR_BUFFER);
                    }
                    else if (_attachment.ref.layout == VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                    {
                        //create depth
                        _texture_buffer->set_buffer_type(w_texture_buffer_type::W_TEXTURE_DEPTH_BUFFER);
                        _depth_buffer_index = i;
                    }

                    auto _hr = _texture_buffer->load(pGDevice, pViewPort.width, pViewPort.height, pMemoryPropertyFlags);
                    if (_hr == S_FALSE)
                    {
                        V(S_FALSE, "loading texture", _trace_info, 3, false);
                        break;
                    }

                    this->_attachment_buffers.push_back(_texture_buffer);
                }

                if (_hr == S_FALSE)
                {
                    release();
                    return S_FALSE;
                }


                //create render pass
                _hr = this->_render_pass.load(pGDevice,
                    pViewPort,
                    pViewportScissor,
                    pAttachmentsDescriptions);
                if (_hr == S_FALSE)
                {
                    release();
                    V(S_FALSE, "loading render pass", _trace_info, 3, false);
                    return S_FALSE;
                }

                ////create frame buffers
                //auto _render_pass_handle = this->_render_pass.get_handle();
                //_hr = this->_frame_buffers.load(pGDevice,
                //	_render_pass_handle,
                //	{ this->_color_buffer.get_image_view() },
                //    _depth_buffer_index == -1 ? nullptr : &this->_depth_buffer.get_image_view(),
                //	//frame buffer size
                //	{
                //		pViewPort.width,
                //		pViewPort.height
                //	},
                //	1);

                //if (_hr == S_FALSE)
                //{
                //	release();
                //	V(S_FALSE, "creating frame buffers", _trace_info, 3, true);
                //}

                return S_OK;
            }

            HRESULT flush_staging_data(_In_ size_t pIndex)
            {
                if (pIndex >= this->_attachment_buffers.size()) return S_FALSE;
                
                auto _texture_buffer = this->_attachment_buffers.at(pIndex);
                if (!_texture_buffer) return S_FALSE;
                
                return _texture_buffer->flush_staging_data();
            }
            
            ULONG release()
            {
                for (auto _buffer : this->_attachment_buffers)
                {
                    SAFE_RELEASE(_buffer);
                }
                this->_gDevice = nullptr;
                
                return 0;
            }
                 
#pragma region Getters
            
           /* const uint32_t get_width() const
            {
                return this->_width;
            }
            
            const uint32_t get_height() const
            {
                return this->_height;
            }
            
            VkSampler get_sampler() const
            {
                return this->_sampler;
            }
           
            VkImage get_image() const
            {
                return this->_image;
            }
            
            VkImageView get_image_view() const
            {
                return this->_view;
            }
            
            VkImageType get_image_type() const
            {
                return this->_image_type;
            }
            
            VkImageViewType get_image_view_type() const
            {
                return this->_image_view_type;
            }
            
            VkFormat get_format() const
            {
                return this->_format;
            }
            
            void* get_pointer_to_staging_data()
            {
                return this->_is_staging ? this->_staging_buffer_memory_pointer : nullptr;
            }

			const VkDescriptorImageInfo get_descriptor_info() const
			{
				return this->_desc_image_info;
			}*/

#pragma endregion
              
        private:
            
            std::string													_name;
            std::shared_ptr<w_graphics_device>							_gDevice;
			wolf::graphics::w_render_pass                               _render_pass;
			wolf::graphics::w_frame_buffer                              _frame_buffers;
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

HRESULT w_render_target::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const uint32_t& pWidth,
	_In_ const uint32_t& pHeight,
	_In_ VkFormat pFormat,
	_In_ const bool& pIsStaging)
{
	if (!this->_pimp) return S_FALSE;
	/*return this->_pimp->load(pGDevice,
		pWidth,
		pHeight,
		pFormat,
		pIsStaging ? (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) : VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);*/
	return S_OK;
}

HRESULT w_render_target::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const uint32_t& pWidth,
	_In_ const uint32_t& pHeight,
	_In_ VkFormat pFormat,
	_In_ const VkMemoryPropertyFlags pMemoryPropertyFlags)
{
	if (!this->_pimp) return S_FALSE;
	//return this->_pimp->load(
	//	pGDevice,
	//	pWidth,
	//	pHeight,
	//	pFormat,
	//	pMemoryPropertyFlags);
	return S_OK;
}

HRESULT w_render_target::flush_staging_data(_In_ size_t pBufferIndex)
{
    if (!this->_pimp) return S_FALSE;
    return this->_pimp->flush_staging_data(pBufferIndex);
}

HRESULT w_render_target::save_to_file(_In_z_ const char* pFilename)
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

	return S_OK;
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
    if(!this->_pimp) return 0;
	return 0;// this->_pimp->get_width();
}

const uint32_t w_render_target::get_height() const
{
    if(!this->_pimp) return 0;
	return 0;// this->_pimp->get_height();
}

//get sampler of image
VkSampler w_render_target::get_sampler() const
{
    if(!this->_pimp) return 0;
	return 0;// this->_pimp->get_sampler();
}

//get image handle
VkImage w_render_target::get_image() const
{
    if(!this->_pimp) return 0;
	return 0;// this->_pimp->get_image();
}

//get image view resource
VkImageView w_render_target::get_image_view() const
{
    if(!this->_pimp) return 0;
	return 0;// this->_pimp->get_image_view();
}

//get image format
VkFormat w_render_target::get_format() const
{
    if(!this->_pimp) return VkFormat::VK_FORMAT_UNDEFINED;
	return VkFormat::VK_FORMAT_UNDEFINED;// this->_pimp->get_format();
}

const VkDescriptorImageInfo w_render_target::get_descriptor_info() const
{
    if(!this->_pimp) return VkDescriptorImageInfo();
	return VkDescriptorImageInfo();// this->_pimp->get_descriptor_info();
}

void* w_render_target::get_pointer_to_staging_data()
{
	if (!this->_pimp) return nullptr;
	return nullptr;// this->_pimp->get_pointer_to_staging_data();
}

#pragma endregion
