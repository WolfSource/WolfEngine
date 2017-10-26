/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_semaphore.h
	Description		 : Semaphore for graphics device
	Comment          :
*/

#ifndef __W_SWMAPHORE_H__
#define __W_SWMAPHORE_H__

#include <w_graphics_headers.h>
#include <w_render_export.h>

namespace wolf
{
	namespace graphics
	{
        class w_graphics_device;
        struct w_semaphore
        {
            //get pointer to semaphore
            W_EXP VkSemaphore* get();
            //initialize semaphore
            W_EXP HRESULT initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
            //release resources of semaphore
            W_EXP ULONG release(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
            
        private:
#ifdef __VULKAN__
            VkSemaphore     _semaphore = 0;
#elif defined(__DX12__)
            ComPtr<?>      _semaphore = nullptr;
#endif
        };
	}
}

#endif
