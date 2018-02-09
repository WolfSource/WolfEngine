/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
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
	namespace graphics
	{
        class w_fences
        {
		public:
            //initialize fence
            W_EXP W_RESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice, _In_ const uint32_t pNumberOfFences = 1);
            //wait for all fence for the timeout period in units of nanoseconds
            W_EXP W_RESULT wait(_In_ uint64_t pTimeOut = UINT64_MAX - 1);
            //reset all fences
            W_EXP W_RESULT reset();
            //get pointer to the first fence
            W_EXP VkFence* get();
            //get all fences
            W_EXP VkFence* get_all();
            //get number of fences
            W_EXP uint32_t get_count();
            //release resources of all fences
            W_EXP ULONG release();

#ifdef __PYTHON__
			W_EXP bool py_initialize(_In_ boost::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, _In_ const uint32_t& pNumberOfFences);
			W_EXP bool py_wait_for(_In_ uint64_t& pTimeOut);
			W_EXP bool py_wait();
			W_EXP bool py_reset();
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

#include "python_exporter/py_fences.h"

#endif
