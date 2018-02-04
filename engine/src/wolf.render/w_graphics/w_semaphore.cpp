#include "w_render_pch.h"
#include "w_graphics_device_manager.h"
#include "w_semaphore.h"

using namespace wolf::graphics;

VkSemaphore* w_semaphore::get()
{
    return &this->_semaphore;
}

HRESULT w_semaphore::initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    this->_gDevice = pGDevice;

#ifdef __VULKAN__
    //create semaphore create info
    VkSemaphoreCreateInfo _semaphore_create_info = {};
    _semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    if (vkCreateSemaphore(pGDevice->vk_device,
                          &_semaphore_create_info,
                          nullptr,
                          &this->_semaphore))
    {
        V(S_FALSE, "creating semaphore", "w_semaphore", 3, false);
        return S_FALSE;
    }
#else
    
#endif
    return S_OK;
}

ULONG w_semaphore::release()
{
#ifdef __VULKAN__
    vkDestroySemaphore(this->_gDevice->vk_device, this->_semaphore, nullptr);
    this->_semaphore = 0;
#else
    
#endif

    this->_gDevice = nullptr;

    return 0;
}

#ifdef __PYTHON__

//Must declare here 
using namespace pywolf;

py_semaphore::py_semaphore() : _graphics_device_index(-1)
{
}

bool py_semaphore::py_initialize(_In_ const uint32_t& pGDeviceIndex)
{
	if (pGDeviceIndex >= pywolf::py_graphics_devices.size()) return false;
	this->_graphics_device_index = pGDeviceIndex;
	return this->initialize(pywolf::py_graphics_devices[pGDeviceIndex]) == S_OK;
}

#endif