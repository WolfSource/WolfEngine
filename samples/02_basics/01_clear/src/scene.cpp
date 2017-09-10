#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf::system;
using namespace wolf::graphics;

scene::scene(_In_z_ std::wstring pRootDirectory) :
w_game(pRootDirectory)
{
    auto _root_dir = pRootDirectory;
    
#if defined(__WIN32) || defined(__UWP)
    content_path = _root_dir + L"../../../../content/";
#elif defined(__APPLE__)
    content_path = _root_dir + L"/../../../../../content/";
#elif defined(__linux)
    error not tested
#elif defined(__ANDROID)
    error not tested
#endif
    
    w_game::set_fixed_time_step(false);
}

scene::~scene()
{
	//release all resources
	release();
}

void scene::initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
	// TODO: Add your pre-initialization logic here

	_super::initialize(pOutputWindowsInfo);
}

void scene::load()
{
    const std::string _trace = this->name + "::load";
    
    auto _gDevice = this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    
    w_point_t _screen_size;
    _screen_size.x =  _output_window->width;
    _screen_size.y = _output_window->height;
    
    //initialize viewport
    this->_viewport.y = 0;
    this->_viewport.width = static_cast<float>(_screen_size.x);
    this->_viewport.height = static_cast<float>(_screen_size.y);
    this->_viewport.minDepth = 0;
    this->_viewport.maxDepth = 1;
    
    //initialize scissor of viewport
    this->_viewport_scissor.offset.x = 0;
    this->_viewport_scissor.offset.y = 0;
    this->_viewport_scissor.extent.width = _screen_size.x;
    this->_viewport_scissor.extent.height = _screen_size.y;
    
    //initialize depth attachment
    auto _depth_attachment = w_graphics_device::w_render_pass_attachments::depth_attachment_description;
    _depth_attachment.format = _output_window->vk_depth_buffer_format;
   
    //define attachments which has color and depth for render pass
    std::vector<VkAttachmentDescription> _attachment_descriptions =
    {
        w_graphics_device::w_render_pass_attachments::color_attachment_description,
        _depth_attachment,
    };
    
    //create render pass
    auto _hr = this->_draw_render_pass.load(_gDevice,
                                            _viewport,
                                            _viewport_scissor,
                                            _attachment_descriptions);
    if (_hr == S_FALSE)
    {
        V(S_FALSE, "creating render pass", _trace, 3, true, true);
    }
    
    //create frame buffers
     auto _render_pass_handle = this->_draw_render_pass.get_handle();
    _hr = this->_draw_frame_buffers.load(_gDevice,
                                         _render_pass_handle,
                                         _output_window->vk_swap_chain_image_views,
                                         &_output_window->vk_depth_buffer_image_view,
                                         _screen_size,
                                         1);
    if (_hr == S_FALSE)
    {
        V(S_FALSE, "creating frame buffers", _trace, 3, true, true);
    }
    
    //create pipeline_cache
    std::string _pipeline_cache_name = "model_pipeline_cache";
    if (w_pipeline::create_pipeline_cache(_gDevice, _pipeline_cache_name) == S_FALSE)
    {
        logger.error("could not create pipeline cache");
        _pipeline_cache_name.clear();
    }
    
    //create semaphore create info
    VkSemaphoreCreateInfo _semaphore_create_info = {};
    _semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    if (vkCreateSemaphore(_gDevice->vk_device,
                          &_semaphore_create_info,
                          nullptr,
                          &this->_draw_semaphore.semaphore))
    {
        V(S_FALSE, "creating semaphore for draw command buffer", _trace, 3, true, true);
    }
    
    //Fence for render sync
    VkFenceCreateInfo _fence_create_info = {};
    _fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    _fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    if (vkCreateFence(_gDevice->vk_device,
                      &_fence_create_info,
                      nullptr,
                      &this->_draw_fence.fence))
    {
        V(S_FALSE, "creating draw fence", _trace, 3, true, true);
    }
    
//
//    auto _swap_chain_image_size = _output_window->vk_swap_chain_image_views.size();
//    
//    //load primary command buffer
//    _hr = this->_draw_command_buffers.load(_gDevice, _swap_chain_image_size);
//    if (_hr == S_FALSE)
//    {
//        logger.error("Error on creating command buffers");
//        release();
//        exit(1);
//    }
    
	_super::load();
}

void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
	// TODO: add your update logic code here

	_super::update(pGameTime);
}

HRESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
    // TODO: add your drawing code here
    return _super::render(pGameTime);
}

void scene::on_window_resized(_In_ UINT pIndex)
{
    w_game::on_window_resized(pIndex);
}

void scene::on_device_lost()
{
    w_game::on_device_lost();
}

#ifdef __WIN32
HRESULT scene::on_msg_proc(HWND pHWND, UINT pMessage, WPARAM pWParam, LPARAM pLParam)
{
	// TODO: add your window message code here

	return S_FALSE;
}
#endif

ULONG scene::release()
{
	if (this->get_is_released()) return 0;
	
	// TODO: release your assets here

	return _super::release();
}
