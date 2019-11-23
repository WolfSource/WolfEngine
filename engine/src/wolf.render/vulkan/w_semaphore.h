/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_semaphore.h
	Description		 : Semaphore for graphics device
	Comment          :
*/

#pragma once

#include <w_graphics_headers.h>
#include <w_render_export.h>

namespace wolf::render::vulkan
{
	class w_graphics_device;
	struct w_semaphore
	{
		//get pointer to semaphore
		W_VK_EXP VkSemaphore* get();
		//initialize semaphore
		W_VK_EXP W_RESULT initialize(const std::shared_ptr<wolf::render::vulkan::w_graphics_device>& pGDevice);
		//release resources of semaphore
		W_VK_EXP ULONG release();

#ifdef __PYTHON__
		W_RESULT py_initialize(_In_ boost::shared_ptr<w_graphics_device>& pGDevice)
		{
			if (!pGDevice.get()) return W_FAILED;
			auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

			auto _hr = initialize(_gDevice);
			//reset local shared_ptr
			_gDevice.reset();

			return _hr;
		}
#endif

	private:
		std::shared_ptr<wolf::render::vulkan::w_graphics_device>      _gDevice = nullptr;
#ifdef __VULKAN__
		VkSemaphore     _semaphore = 0;
#elif defined(__DX12__)
		//ComPtr<?>      _semaphore = nullptr;
#endif
		};
}

#include "python_exporter/py_semaphore.h"
