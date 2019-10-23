/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_fences.h
	Description		 : Fence for graphics device
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_FENCES_H__
#define __W_FENCES_H__

#include <w_graphics_headers.h>
#include <w_render_export.h>

namespace wolf
{
	namespace render
	{
		namespace vulkan
		{
			class w_fences
			{
			public:
				//initialize fence
				W_VK_EXP W_RESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice, _In_ const uint32_t pNumberOfFences = 1);
				//wait for all fence for the timeout period in units of nanoseconds
				W_VK_EXP W_RESULT wait(_In_ uint64_t pTimeOut = UINT64_MAX - 1);
				//reset all fences
				W_VK_EXP W_RESULT reset();
				//get pointer to the first fence
				W_VK_EXP VkFence* get();
				//get all fences
				W_VK_EXP VkFence* get_all();
				//get number of fences
				W_VK_EXP uint32_t get_count();
				//release resources of all fences
				W_VK_EXP ULONG release();

#ifdef __PYTHON__
				W_RESULT py_initialize(_In_ boost::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ const uint32_t& pNumberOfFences = 1)
				{
					//create render pass attchaments
					if (!pGDevice.get()) return W_FAILED;

					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);
					auto _hr = initialize(_gDevice, pNumberOfFences);
					_gDevice.reset();

					return _hr;
				}
#endif
			private:

				std::shared_ptr<w_graphics_device> _gDevice;

#ifdef __VULKAN__
				std::vector<VkFence>        _fences;
#elif defined(__DX12__)
				ComPtr<ID3D12Fence>
#endif
			};
		}
	}
}

#include "python_exporter/py_fences.h"

#endif
