/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_gmemory_allocator
	Description		 : graphics memory allocator
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_MEMORY_ALLOCATOR_H__
#define __W_MEMORY_ALLOCATOR_H__

#include <w_graphics_headers.h>
#include "vk_mem_alloc.h"

namespace wolf
{
	namespace graphics
	{
		typedef enum w_memory_usage_flag
		{
			/** No intended memory usage specified.
			Use other members of VmaAllocationCreateInfo to specify your requirements.
			*/
			MEMORY_USAGE_UNKNOWN = VMA_MEMORY_USAGE_UNKNOWN,
			/** Memory will be used on device only, so fast access from the device is preferred.
			It usually means device-local GPU (video) memory.
			No need to be mappable on host.
			It is roughly equivalent of `D3D12_HEAP_TYPE_DEFAULT`.

			Usage:

			- Resources written and read by device, e.g. images used as attachments.
			- Resources transferred from host once (immutable) or infrequently and read by
			device multiple times, e.g. textures to be sampled, vertex buffers, uniform
			(constant) buffers, and majority of other types of resources used by device.

			Allocation may still end up in `HOST_VISIBLE` memory on some implementations.
			In such case, you are free to map it.
			You can use #VMA_ALLOCATION_CREATE_MAPPED_BIT with this usage type.
			*/
			MEMORY_USAGE_GPU_ONLY = VMA_MEMORY_USAGE_GPU_ONLY,
			/** Memory will be mappable on host.
			It usually means CPU (system) memory.
			Resources created in this pool may still be accessible to the device, but access to them can be slower.
			Guarantees to be `HOST_VISIBLE` and `HOST_COHERENT`.
			CPU read may be uncached.
			It is roughly equivalent of `D3D12_HEAP_TYPE_UPLOAD`.

			Usage: Staging copy of resources used as transfer source.
			*/
			MEMORY_USAGE_CPU_ONLY = VMA_MEMORY_USAGE_CPU_ONLY,
			/**
			Memory that is both mappable on host (guarantees to be `HOST_VISIBLE`) and preferably fast to access by GPU.
			CPU reads may be uncached and very slow.

			Usage: Resources written frequently by host (dynamic), read by device. E.g. textures, vertex buffers, uniform buffers updated every frame or every draw call.
			*/
			MEMORY_USAGE_CPU_TO_GPU = VMA_MEMORY_USAGE_CPU_TO_GPU,
			/** Memory mappable on host (guarantees to be `HOST_VISIBLE`) and cached.
			It is roughly equivalent of `D3D12_HEAP_TYPE_READBACK`.

			Usage:

			- Resources written by device, read by host - results of some computations, e.g. screen capture, average scene luminance for HDR tone mapping.
			- Any resources read or accessed randomly on host, e.g. CPU-side copy of vertex buffer used as source of transfer, but also used for collision detection.
			*/
			MEMORY_USAGE_GPU_TO_CPU = VMA_MEMORY_USAGE_GPU_TO_CPU,
			MEMORY_USAGE_MAX_ENUM = VMA_MEMORY_USAGE_MAX_ENUM
		} w_memory_usage_flag;

		class w_memory_allocator_pimp;
		class w_memory_allocator : public system::w_object
		{
		public:
			W_EXP w_memory_allocator();
			W_EXP ~w_memory_allocator();

			W_EXP W_RESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice, 
				_In_ const bool& pPreferredLargeHeapBlockSize = false,
				_In_ const bool& pUseCustomCpuAllocation_Callbacks = false);
			W_EXP VmaAllocation* allocate_buffer(_In_ VkBufferCreateInfo pCreateInfo, _In_ const w_memory_usage_flag& pMemoryUsage, 
				_Inout_ VkBuffer& pBufferHandle, _Inout_ VmaAllocationInfo& pAllocInfo);
			W_EXP VmaAllocation* allocate_image(_In_ VkImageCreateInfo pCreateInfo, _In_ const w_memory_usage_flag& pMemoryUsage, 
				_Inout_ VkImage& pImageHandle, _Inout_ VmaAllocationInfo& pAllocInfo);
			W_EXP void free_buffer(_In_ VmaAllocation* pAllocation, _Inout_ VkBuffer& pBufferHandle);
			W_EXP void free_image(_In_ VmaAllocation* pAllocation, _Inout_ VkImage& pImageHandle);
			W_EXP W_RESULT bind(_In_ VmaAllocation* pAllocation, _Inout_ VkBuffer& pBufferHandle);
			W_EXP W_RESULT bind(_In_ VmaAllocation* pAllocation, _Inout_ VkImage& pImageHandle);
			W_EXP W_RESULT map(_In_ VmaAllocation* pAllocation, _Inout_ void** pMappedDate);
			W_EXP void unmap(_In_ VmaAllocation* pAllocation);
			W_EXP ULONG release();

#pragma region Getters
			VkMemoryPropertyFlags get_memory_property_flags(_In_ VmaAllocationInfo& pAllocInfo) const;
#pragma endregion

		private:
			typedef system::w_object            _super;
			w_memory_allocator_pimp*            _pimp;
		};
	}
}

#endif
