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
		class w_texture_2D : public system::w_object
		{
		public:
			W_EXP w_texture_2d();
			W_EXP virtual ~w_texture_2d();

			//release all resources
			W_EXP virtual ULONG release() override;
            
            W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice);
            
			//Load texture2D from path
			W_EXP HRESULT load_texture_2D_from_file(
#if defined(__WIN32) || defined(__UWP)
                                                    _In_ std::wstring pPath,
#else
                                                     _In_ std::string pPath,
#endif
                                                    _In_ bool pIsAbsolutePath = false);

#pragma region Getters


#pragma endregion

#pragma region Setters


#pragma endregion

		private:
            HRESULT                                         _create_image();
            
			typedef system::w_object						_super;
            
            std::shared_ptr<w_graphics_device>              _gDevice;
            UINT                                            _width;
            UINT                                            _height;
            
            VkImage                                         _image;
            VkImageView                                     _view;
            VkSampler                                       _sampler;
            VkDeviceMemory                                  _memory;
		};
	}
}

#endif
