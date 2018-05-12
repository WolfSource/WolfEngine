#include "w_render_pch.h"
#include "w_graphics_device_manager.h"
#include "w_memory_allocator.h"

#ifndef __WIN32
#include <w_aligned_malloc.h>
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef VMA_IMPLEMENTATION
#define VMA_IMPLEMENTATION
#endif
#include "vk_mem_alloc.h"

#include <assert.h>

static void* const CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA = (void*)(intptr_t)43564544;
static void* custom_cpu_allocation(
	_In_ void* pUserData,
	_In_ size_t pSize,
	_In_ size_t pAlignment,
	_In_ VkSystemAllocationScope pAllocationScope)
{
	assert(pUserData == CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA);

#ifdef __WIN32
	return _aligned_malloc(pSize, pAlignment);
#else
	return aligned_malloc(pSize, pAlignment);
#endif
}

static void* custom_cpu_reallocation(
	void* pUserData, void* pOriginal, size_t pSize, size_t pAlignment,
	VkSystemAllocationScope allocationScope)
{
	assert(pUserData == CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA);

#ifdef __WIN32
	return _aligned_realloc(pOriginal, pSize, pAlignment);
#else
	aligned_free(pOriginal);
	return aligned_alloc(pSize, pAlignment);
#endif
}

static void custom_cpu_free(void* pUserData, void* pMemory)
{
	assert(pUserData == CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA);

#ifdef __WIN32
	_aligned_free(pMemory);
#else
	aligned_free(pMemory);
#endif
}

namespace wolf
{
    namespace graphics
    {
        class w_memory_allocator_pimp
        {
        public:
			w_memory_allocator_pimp() :
                _name("w_memory_allocator::")
            {
            }
            
			W_RESULT initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice, 
				_In_ const bool& pPreferredLargeHeapBlockSize,
				_In_ const bool& pUseCustomCpuAllocation_Callbacks)
			{
				VmaAllocatorCreateInfo _allocator_info = {};
				_allocator_info.physicalDevice = pGDevice->vk_physical_device;
				_allocator_info.device = pGDevice->vk_device;
				_allocator_info.preferredLargeHeapBlockSize = pPreferredLargeHeapBlockSize ? static_cast<VkDeviceSize>(VMA_DEFAULT_LARGE_HEAP_BLOCK_SIZE) : 0;

				if (pUseCustomCpuAllocation_Callbacks)
				{
					VkAllocationCallbacks _cpu_allocation_callbacks = {};
					_cpu_allocation_callbacks.pUserData = CUSTOM_CPU_ALLOCATION_CALLBACK_USER_DATA;
					_cpu_allocation_callbacks.pfnAllocation = &custom_cpu_allocation;
					_cpu_allocation_callbacks.pfnReallocation = &custom_cpu_reallocation;
					_cpu_allocation_callbacks.pfnFree = &custom_cpu_free;
					_allocator_info.pAllocationCallbacks = &_cpu_allocation_callbacks;
				}

				return vmaCreateAllocator(&_allocator_info, &this->_allocator) == VkResult::VK_SUCCESS ? W_PASSED : W_FAILED;
			}
			
			VmaAllocation* allocate_buffer(_In_ VkBufferCreateInfo pCreateInfo, _In_ const VmaMemoryUsage& pMemoryUsage,
				_Inout_ VkBuffer& pBufferHandle,
				_Inout_ VmaAllocationInfo& pAllocInfo)
			{
				const std::string _trace_info = this->_name + "::allocate_buffer";

				VmaAllocationCreateInfo _alloc_info = {};
				_alloc_info.usage = pMemoryUsage;
				if (pMemoryUsage == VmaMemoryUsage::VMA_MEMORY_USAGE_CPU_ONLY)
				{
					_alloc_info.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
				}

				auto _allocation = new (std::nothrow) VmaAllocation();
				if (!_allocation)
				{
					V(W_FAILED, "allocating memory for VmaAllocation", _trace_info, 3);
					return nullptr;
				}

				if (vmaCreateBuffer(this->_allocator, &pCreateInfo, &_alloc_info, &pBufferHandle, _allocation, &pAllocInfo))
				{
					delete _allocation;
					return nullptr;
				}

				return _allocation;
			}

			VmaAllocation* allocate_image(_In_ VkImageCreateInfo pCreateInfo, _In_ const VmaMemoryUsage& pMemoryUsage, 
				_Inout_ VkImage& pImageHandle,
				_Inout_ VmaAllocationInfo& pAllocInfo)
			{
				const std::string _trace_info = this->_name + "::alloc";

				VmaAllocationCreateInfo _alloc_info = {};
				_alloc_info.usage = pMemoryUsage;

				auto _allocation = new (std::nothrow) VmaAllocation();
				if (!_allocation)
				{
					V(W_FAILED, "allocating memory for VmaAllocation", _trace_info, 3);
					return nullptr;
				}

				if (vmaCreateImage(this->_allocator, &pCreateInfo, &_alloc_info, &pImageHandle, _allocation, &pAllocInfo))
				{
					delete _allocation;
					return nullptr;
				}

				return _allocation;
			}

			void free_buffer(_In_ VmaAllocation* pAllocation, _Inout_ VkBuffer& pBufferHandle)
			{
				if (!pAllocation) return;
				vmaDestroyBuffer(this->_allocator, pBufferHandle, *pAllocation);
			}

			void free_image(_In_ VmaAllocation* pAllocation, _Inout_ VkImage& pImageHandle)
			{
				if (!pAllocation) return;
				vmaDestroyImage(this->_allocator, pImageHandle, *pAllocation);
			}

			W_RESULT bind(_In_ VmaAllocation* pAllocation, _Inout_ VkBuffer& pBufferHandle)
			{
				return vmaBindBufferMemory(this->_allocator, *pAllocation, pBufferHandle) ? W_FAILED : W_PASSED;
			}

			W_RESULT bind(_In_ VmaAllocation* pAllocation, _Inout_ VkImage& pImageHandle)
			{
				return vmaBindImageMemory(this->_allocator, *pAllocation, pImageHandle) ? W_FAILED : W_PASSED;
			}

			W_RESULT map(_In_ VmaAllocation* pAllocation, _Inout_ void** pMappedDate)
			{
				return vmaMapMemory(this->_allocator, *pAllocation, pMappedDate) ? W_FAILED : W_PASSED;
			}

			void unmap(_In_ VmaAllocation* pAllocation)
			{
				vmaUnmapMemory(this->_allocator, *pAllocation);
			}
			
			ULONG release()
			{
				vmaDestroyAllocator(this->_allocator);
				this->_allocator = nullptr;
				return 0;
			}

#pragma region Getters

			VkMemoryPropertyFlags get_memory_property_flags(_In_ VmaAllocationInfo& pAllocInfo) const
			{
				VkMemoryPropertyFlags _mem_flags;
				vmaGetMemoryTypeProperties(this->_allocator, pAllocInfo.memoryType, &_mem_flags);
				return _mem_flags;
			}

#pragma endregion

        private:
            std::string                                         _name;
			VmaAllocator										_allocator;
        };
    }
}

using namespace wolf::graphics;

w_memory_allocator::w_memory_allocator() : _pimp(new w_memory_allocator_pimp())
{
    _super::set_class_name("w_memory_allocator");
}

w_memory_allocator::~w_memory_allocator()
{
    release();
}

W_RESULT w_memory_allocator::initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const bool& pPreferredLargeHeapBlockSize,
	_In_ const bool& pUseCustomCpuAllocation_Callbacks)
{
	return this->_pimp ? this->_pimp->initialize(pGDevice, pPreferredLargeHeapBlockSize, pUseCustomCpuAllocation_Callbacks) : W_FAILED;
}

VmaAllocation* w_memory_allocator::allocate_buffer(_In_ VkBufferCreateInfo pCreateInfo, _In_ const w_memory_usage_flag& pMemoryUsage, 
	_Inout_ VkBuffer& pBufferHandle, _Inout_ VmaAllocationInfo& pAllocInfo)
{
	return this->_pimp ? this->_pimp->allocate_buffer(pCreateInfo, (VmaMemoryUsage)pMemoryUsage, pBufferHandle, pAllocInfo) : nullptr;
}

VmaAllocation* w_memory_allocator::allocate_image(_In_ VkImageCreateInfo pCreateInfo, _In_ const w_memory_usage_flag& pMemoryUsage, 
	_Inout_ VkImage& pImageHandle, _Inout_ VmaAllocationInfo& pAllocInfo)
{
	return this->_pimp ? this->_pimp->allocate_image(pCreateInfo, (VmaMemoryUsage)pMemoryUsage, pImageHandle, pAllocInfo) : nullptr;
}

void w_memory_allocator::free_buffer(_In_ VmaAllocation* pAllocation, _Inout_ VkBuffer& pBufferHandle)
{
	if(this->_pimp)
    {
        this->_pimp->free_buffer(pAllocation, pBufferHandle);
    }
}

void w_memory_allocator::free_image(_In_ VmaAllocation* pAllocation, _Inout_ VkImage& pImageHandle)
{
	if(this->_pimp)
    {
        this->_pimp->free_image(pAllocation, pImageHandle);
    }
}

W_RESULT w_memory_allocator::bind(_In_ VmaAllocation* pAllocation, _Inout_ VkBuffer& pBufferHandle)
{
	return this->_pimp ? this->_pimp->bind(pAllocation, pBufferHandle) : W_FAILED;
}

W_RESULT w_memory_allocator::bind(_In_ VmaAllocation* pAllocation, _Inout_ VkImage& pImageHandle)
{
	return this->_pimp ? this->_pimp->bind(pAllocation, pImageHandle) : W_FAILED;
}

W_RESULT w_memory_allocator::map(_In_ VmaAllocation* pAllocation, _Inout_ void** pMappedDate)
{
	return this->_pimp ? this->_pimp->map(pAllocation, pMappedDate) : W_FAILED;
}

void w_memory_allocator::unmap(_In_ VmaAllocation* pAllocation)
{
	if (this->_pimp)
	{
		this->_pimp->unmap(pAllocation);
	}
}

ULONG w_memory_allocator::release()
{
	if (this->get_is_released()) return 1;

	SAFE_RELEASE(this->_pimp);

	return _super::release();
}
#pragma region Getters

VkMemoryPropertyFlags w_memory_allocator::get_memory_property_flags(_In_ VmaAllocationInfo& pAllocInfo) const
{
	return this->_pimp ? this->_pimp->get_memory_property_flags(pAllocInfo) : VkMemoryPropertyFlags();
}

#pragma endregion
