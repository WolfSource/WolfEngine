/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_texture_2d.h
	Description		 : create and handle 2D texture resource
	Comment          : Wolf used encoding texture with two channel based on http://devkk.net/index.php?tag=articles&id=24
*/

#ifndef __W_TEXTURE_2D_H__
#define __W_TEXTURE_2D_H__

#include "w_graphics_device_manager.h"
#include <w_logger.h>
#include <w_io.h>

namespace wolf
{
	namespace graphics
	{
        class w_texture_pimp;
		class w_texture : public system::w_object
		{
		public:
			W_EXP w_texture();
			W_EXP virtual ~w_texture();

            W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                               _In_ const VkMemoryPropertyFlags pMemoryPropertyFlags =
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            
			//Load texture2D from path
			W_EXP HRESULT initialize_texture_2D_from_file(_In_ std::wstring pPath, _In_ bool pIsAbsolutePath = false);
            
            //release all resources
            W_EXP virtual ULONG release() override;
            
            
#pragma region Getters

            //get width of image
            W_EXP const UINT get_width() const;
            //get height of image
            W_EXP const UINT get_height() const;
            //get sampler of image
            W_EXP VkSampler get_sampler() const;
            //get image handle
            W_EXP VkImage get_image() const;
            //get image view resource
            W_EXP VkImageView get_image_view() const;
            //get image type
            W_EXP VkImageType get_image_type() const;
            //get image view type
            W_EXP VkImageViewType get_image_view_type() const;
            //get image format
            W_EXP VkFormat get_format() const;
            //get is bgra
            W_EXP bool get_is_bgra() const;
            //get write descriptor image info
            W_EXP VkDescriptorImageInfo get_descriptor_info() const;
            
#pragma endregion

#pragma region Setters

            //Set image type
            W_EXP void set_image_type(_In_ VkImageType pImageViewType);
            //Set image view type
            W_EXP void set_image_view_type(_In_ VkImageViewType pImageViewType);
            //Set format
            W_EXP void set_format(_In_ VkFormat pFormat);

#pragma endregion

		private:
			typedef system::w_object						_super;
            w_texture_pimp*                                 _pimp;
        };
    }
}

#endif
