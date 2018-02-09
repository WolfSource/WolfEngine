#include "w_render_pch.h"
#include "w_graphics_device_manager.h"
#include "w_semaphore.h"

using namespace wolf::graphics;

VkSemaphore* w_semaphore::get()
{
    return &this->_semaphore;
}

W_RESULT w_semaphore::initialize(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
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
        V(W_FALSE, "creating semaphore", "w_semaphore", 3, false);
        return W_FALSE;
    }
#else
    
#endif
    return W_OK;
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

bool w_semaphore::py_initialize(_In_ boost::shared_ptr<w_graphics_device>& pGDevice)
{
	if (!pGDevice.get()) return false;
	auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

	auto _hr = initialize(_gDevice);
	//reset local shared_ptr
	_gDevice.reset();

	return _hr == W_OK;
}

#endif