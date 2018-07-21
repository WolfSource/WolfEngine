#include "w_render_pch.h"
#include "w_occlusion_query.h"

namespace wolf
{
    namespace graphics
    {
        class w_occlusion_query_pimp
        {
        public:
            W_RESULT initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
            {
                VkQueryPoolCreateInfo _query_pool_info = {};
                _query_pool_info.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
                _query_pool_info.queryType = VK_QUERY_TYPE_OCCLUSION;
                _query_pool_info.queryCount = 2;
                vkCreateQueryPool(pGDevice->vk_device, &_query_pool_info, NULL, &this->_query_pool);
                
                return W_PASSED;
            }
            
            ULONG release()
            {
                return 0;
            }
            
        private:
            VkQueryPool _query_pool;
        };
    }
}

using namespace wolf::graphics;

W_RESULT w_occlusion_query::initialize(const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{

    
    return W_PASSED;
}

ULONG w_occlusion_query::release()
{
    if (_super::get_is_released()) return 1;
    
    
    return 0;
}
