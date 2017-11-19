/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_attachment_desc.h
	Description		 : Attachment buffer description
	Comment          : 
*/

#ifndef __W_ATTACHMENT_DESC_H__
#define __W_ATTACHMENT_DESC_H__

#include "w_graphics_device_manager.h"
#include "w_texture.h"

namespace wolf
{
	namespace graphics
	{
		/*
		For Vulkan
			attachment description contains:
				@param flags,	Flags
				@param format,  Format of an image used for the attachment
				@param samples, Number of samples of the image; The value greater than 1 means multisampling
				@param loadOp,  Specifies what to do with the image(s) contents at the beginning of a render pass.
				@param storeOp, Informs the driver what to do with the image(s) content(s) after the render pass.
				@param stencilLoadOp, The same as loadOp but for the stencil part of depth/stencil images, please note that for color attachments this parameter will be ignored
				@param stencilStoreOp, The same as storeOp but for the stencil part of depth/stencil images, please note that for color attachments this parameter will be ignored
				@param initialLayout, The layout of given attachment when the render pass starts
				@param finalLayout, The layout that driver will automatically transite the given image into at the end of a render pass
			attachment reference contains:
				@param attachment, index of attachment
				@param layout, The layout of given attachment
		*/
		struct w_attachment_desc
		{
			VkAttachmentDescription desc;
			VkAttachmentReference ref;

			w_attachment_desc(_In_ w_texture_buffer_type pType = w_texture_buffer_type::W_TEXTURE_COLOR_BUFFER)
			{
				switch (pType)
				{
				case w_texture_buffer_type::W_TEXTURE_COLOR_BUFFER:

					//init description
					this->desc.flags = 0;
					this->desc.format = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
					this->desc.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
					this->desc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
					this->desc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
					this->desc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					this->desc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
					this->desc.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
					this->desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

					//init reference
					this->ref.attachment = 0;
					this->ref.layout = VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

					break;
				case w_texture_buffer_type::W_TEXTURE_DEPTH_BUFFER:

					//init description
					this->desc.flags = 0;
					this->desc.format = VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT;
					this->desc.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
					this->desc.loadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR;
					this->desc.storeOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE;
					this->desc.stencilLoadOp = VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE;
					this->desc.stencilStoreOp = VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE;
					this->desc.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
					this->desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

					//init reference
					this->ref.attachment = 1;
					this->ref.layout = VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

					break;
				case w_texture_buffer_type::W_TEXTURE_STENCIL_BUFFER:
					break;
				}
			}
		};
	}
}

#endif
