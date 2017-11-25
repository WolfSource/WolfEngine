/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_texture.h
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
		enum w_texture_buffer_type
		{
			W_TEXTURE_COLOR_BUFFER = 1,
			W_TEXTURE_DEPTH_BUFFER = 2,
			W_TEXTURE_STENCIL_BUFFER = 4,
		};

		enum w_texture_view_type 
		{
			W_TEXTURE_VIEW_TYPE_1D = 0,
			W_TEXTURE_VIEW_TYPE_2D = 1,
			W_TEXTURE_VIEW_TYPE_3D = 2,
			W_TEXTURE_VIEW_TYPE_CUBE = 3,
			W_TEXTURE_VIEW_TYPE_1D_ARRAY = 4,
			W_TEXTURE_VIEW_TYPE_2D_ARRAY = 5,
			W_TEXTURE_VIEW_TYPE_CUBE_ARRAY = 6,
		};
				
        class w_texture_pimp;
		class w_texture : public system::w_object
		{
		public:
			W_EXP w_texture();
			W_EXP virtual ~w_texture();

            W_EXP HRESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const uint32_t& pWidth = 32,
                _In_ const uint32_t& pHeight = 32,
				_In_ const bool& pIsStaging = false);
            
            W_EXP HRESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const uint32_t& pWidth,
                _In_ const uint32_t& pHeight,
				_In_ const VkMemoryPropertyFlags pMemoryPropertyFlags);
            
			//Load texture
			W_EXP HRESULT load();

			//Load texture from file
			W_EXP HRESULT load_texture_2D_from_file(_In_ std::wstring pPath, _In_ bool pIsAbsolutePath = false);
            //Load texture from memory in format of RGBA
            W_EXP HRESULT load_texture_from_memory_rgba(_In_ uint8_t* pRGBAData);
            //Load texture from memory in format of RGB
            W_EXP HRESULT load_texture_from_memory_rgb(_In_ uint8_t* pRGBAData);
            //Load texture from memory, all channels have same byte
            W_EXP HRESULT load_texture_from_memory_all_channels_same(_In_ uint8_t pData);
            //Load texture from w_color
            W_EXP HRESULT load_texture_from_memory_color(_In_ w_color pColor);
            /*
                copy data to texture
                if this is a staging buffer, do not use this function because it will cause memory leaks,
                instead use "get_pointer_to_staging_data" function
            */
            W_EXP HRESULT copy_data_to_texture_2D(_In_ const uint8_t* pRGBA);
			
			//read texture's data
			W_EXP void* read_data_of_texture();

            //flush staging buffer
            W_EXP HRESULT flush_staging_data();

            //release all resources
            W_EXP virtual ULONG release() override;

            //load texture and store it into the shared
            W_EXP static HRESULT load_to_shared_textures(_In_ const std::shared_ptr<w_graphics_device>& pGDevice, 
                                                         _In_z_ std::wstring pPath,
                                                        _Inout_ w_texture** pPointerToTexture);            
            //save texture as bitmap file
            W_EXP static void write_bitmap_to_file(
                _In_z_ const char* pFilename,
                _In_ const uint8_t* pData,
                _In_ const int& pWidth, const int& pHeight);

			/*
				save png image file
				@param pFileName, path of file
				@param pSize, size of file (width, height)
				@param pData, pointer to rgba data
				@param pCompCount, number of channels(RGBA = 4, RGB = 3)
				@param pStrideInBytes, stride of pixel's structure inf bytes
			*/
			W_EXP static HRESULT save_png_to_file(_In_z_ const char* pFileName, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData, _In_ int pComp, _In_ int pStrideInBytes = 4 * sizeof(uint8_t));
			/*
				save bmp image file
				@param pFileName, path of file
				@param pSize, size of file (width, height)
				@param pData, pointer to rgba data
				@param pCompCount, number of channels(RGBA = 4, RGB = 3)
			*/
			W_EXP static HRESULT save_bmp_to_file(_In_z_ const char* pFileName, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData,_In_ int pComp);
			/*
				save tga image file
				@param pFileName, path of file
				@param pSize, size of file (width, height)
				@param pData, pointer to rgba data
				@param pCompCount, number of channels(RGBA = 4, RGB = 3)
				@param pQuality, Quality(1 - 100)
			*/
			W_EXP static HRESULT save_tga_to_file(_In_z_ const char* pFileName, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData,_In_ int pComp);
			/*
				save hdr image file
				@param pFileName, path of file
				@param pSize, size of file (width, height)
				@param pData, pointer to rgba data
				@param pCompCount, number of channels(RGBA = 4, RGB = 3)
			*/
			W_EXP static HRESULT save_hdr_to_file(_In_z_ const char* pFileName, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const float* pData ,_In_ int pCompCount);
			/*
				save jpg image file
				@param pFileName, path of file
				@param pSize, size of file (width, height)
				@param pData, pointer to rgba data
				@param pCompCount, number of channels(RGBA = 4, RGB = 3)
				@param pQuality, Quality(1 - 100)
			*/
			W_EXP static HRESULT save_jpg_to_file(_In_z_ const char* pFileName, _In_ uint32_t pWidth, _In_ uint32_t pHeight, _In_ const void* pData,_In_ int pCompCount, _In_ int pQuality);

            //release all shared textures
            W_EXP ULONG static release_shared_textures();

#pragma region Getters

            //get width of image
            W_EXP const uint32_t get_width() const;
            //get height of image
            W_EXP const uint32_t get_height() const;
			//get image usage
			W_EXP const VkImageUsageFlags get_usage() const;
			//get sampler of image
            W_EXP VkSampler get_sampler() const;
            //get image and view resources
            W_EXP w_image_view get_image_view() const;
            //get image type
            W_EXP VkImageType get_image_type() const;
            //get image view type
            W_EXP VkImageViewType get_image_view_type() const;
            //get image format
            W_EXP VkFormat get_format() const;
            //get write descriptor image info
            W_EXP const VkDescriptorImageInfo get_descriptor_info() const;

#pragma endregion

#pragma region Setters

			//set image format
			W_EXP void set_format(_In_ VkFormat pFormat);
			//set image usage
			W_EXP void set_usage(_In_ VkImageUsageFlags pUsage);
			//set buffer type
			W_EXP void set_buffer_type(_In_ w_texture_buffer_type pBufferType);
			//set image view type
			W_EXP void set_view_type(_In_ w_texture_view_type pViewType);

#pragma region

            W_EXP static w_texture*                               default_texture;

		private:
			typedef system::w_object						_super;
            w_texture_pimp*                                 _pimp;

            static std::map<std::wstring, w_texture*>       _shared;
        };
    }
}

#endif
