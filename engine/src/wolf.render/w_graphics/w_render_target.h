/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_render_target.h
	Description		 : render to texture
	Comment          : 
*/

#ifndef __W_RENDER_TARGET_H__
#define __W_RENDER_TARGET_H__

#include "w_graphics_device_manager.h"

namespace wolf
{
	namespace graphics
	{			
        class w_render_target_pimp;
		class w_render_target : public system::w_object
		{
		public:
			W_EXP w_render_target();
			W_EXP virtual ~w_render_target();

            W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const uint32_t& pWidth,
                _In_ const uint32_t& pHeight,
				_In_ VkFormat pFormat,
				_In_ const bool& pIsStaging);
            
            W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const uint32_t& pWidth,
                _In_ const uint32_t& pHeight,
				_In_ VkFormat pFormat,
				_In_ const VkMemoryPropertyFlags pMemoryPropertyFlags);
            
            //flush staging buffer
            W_EXP HRESULT flush_staging_data();
      
            //save texture as bitmap file
            W_EXP HRESULT save_to_file(_In_z_ const char* pFileName);

			//release all resources
			W_EXP virtual ULONG release() override;

#pragma region Getters

            //get width of image
            W_EXP const uint32_t get_width() const;
            //get height of image
            W_EXP const uint32_t get_height() const;
            //get sampler of image
            W_EXP VkSampler get_sampler() const;
            //get image handle
            W_EXP VkImage get_image() const;
            //get image view resource
            W_EXP VkImageView get_image_view() const;
            //get image format
            W_EXP VkFormat get_format() const;
            //get write descriptor image info
            W_EXP const VkDescriptorImageInfo get_descriptor_info() const;
            //get pointer to the staging data
            W_EXP void* get_pointer_to_staging_data();
            
#pragma endregion
			
		private:
			typedef system::w_object						_super;
            w_render_target_pimp*                           _pimp;
        };
    }
}

#endif
