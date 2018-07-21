#include "w_render_pch.h"
#include "w_graphics_device_manager.h"
#include "w_queue.h"

using namespace wolf::render::vulkan;

ULONG w_queue::release()
{
#ifdef __VULKAN__
    this->queue = 0;
#endif
    this->index = UINT32_MAX;
    return 0;
}
