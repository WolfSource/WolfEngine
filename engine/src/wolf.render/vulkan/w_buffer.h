/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_buffer
	Description		 : graphics buffer
	Comment          : 
*/

#pragma once

#include "w_graphics_device_manager.h"

namespace wolf::render::vulkan
{
	class w_buffer_pimp;
	class w_buffer
	{
	public:
		W_VK_EXP w_buffer();
		W_VK_EXP ~w_buffer();

		W_VK_EXP W_RESULT allocate_as_staging(
			_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
			_In_ const uint32_t pBufferSize,
			_In_ const bool pAllocateFromMemoryPool = true);

		W_VK_EXP W_RESULT allocate(
			_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
			_In_ const uint32_t pBufferSize,
			_In_ const uint32_t pUsage,
			_In_ const w_memory_usage_flag pMemoryFlag,
			_In_ const bool pAllocateFromMemoryPool = true);

		W_VK_EXP W_RESULT reallocate(_In_ const uint32_t pBufferSizeInBytes);

		W_VK_EXP W_RESULT bind();

		//copy between buffers, always run this method in main thread
		W_VK_EXP W_RESULT copy_to(_In_ w_buffer& pDestinationBuffer);

		W_VK_EXP void* map();
		W_VK_EXP void unmap();
		W_VK_EXP W_RESULT flush();
		W_VK_EXP W_RESULT free();

		W_VK_EXP ULONG release();

#pragma region Getters

		W_VK_EXP const uint32_t							get_offset() const;
		//W_VK_EXP const uint32_t						get_global_offset() const;
		W_VK_EXP const uint32_t							get_size() const;
		W_VK_EXP const uint32_t							get_usage_flags() const;
		W_VK_EXP const uint32_t							get_memory_flags() const;
		W_VK_EXP const w_buffer_handle					get_buffer_handle() const;
		W_VK_EXP const w_descriptor_buffer_info			get_descriptor_info() const;
		W_VK_EXP const w_device_memory					get_memory() const;

#pragma endregion

#pragma region Setters

		W_VK_EXP W_RESULT set_data(_In_ const void* const pData);

#pragma endregion

#ifdef __PYTHON__

		W_RESULT py_load(
			_In_ boost::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
			_In_ const uint32_t pBufferSize,
			_In_ const uint32_t pUsage,
			_In_ const uint32_t pMemoryFlags)
		{
			if (!pGDevice.get()) return W_FAILED;
			auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

			auto _hr = load(_gDevice, pBufferSize, pUsage, pMemoryFlags);

			_gDevice.reset();
			return _hr;
		}

#endif

	private:
		bool								_is_released;
		w_buffer_pimp* _pimp;
	};
}

#include "python_exporter/py_buffer.h"

