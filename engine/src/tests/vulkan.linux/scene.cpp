/*
    Project		: Wolf Engine (http://WolfSource.io). Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source		: https://github.com/PooyaEimandar/WolfEngine - Please direct any bug to contact@WolfSource.io or tweet @Wolf_Engine on twitter
    Name		: Scene.cpp
    Description         : This is simple test, you can remove this project, during build
    Comment             :
*/

#include "pch.h"
#include "scene.h"

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::framework;
   
scene::scene(const std::string pRootDirectory, const std::string pAppName) : w_game(pRootDirectory, pAppName)
{
    w_game::set_fixed_time_step(false);
}

scene::~scene()
{
    release();
}

void scene::initialize(std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
    // TODO: Add your pre-initialization logic here
    w_game::initialize(pOutputWindowsInfo);
}

void scene::load()
{
    w_game::load();
    
    auto _gDevice =  wolf::framework::w_game::graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);

    const std::vector<VkAttachmentDescription> _attachment_descriptions = { w_graphics_device::vk_default_attachment_description };
    const VkAttachmentReference _attachment_ref[] = { w_graphics_device::vk_default_color_attachment_reference };
    const std::vector<VkSubpassDescription> _subpass_descriptions =
    {
        {
            0,
            VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
            0,
            nullptr,
            1,
            _attachment_ref,
            nullptr,
            nullptr,
            0,
            nullptr
        }
    };
    const std::vector<VkSubpassDependency> _subpass_deps = {};

    _gDevice->create_render_pass("pass1",
        _attachment_descriptions,
	_subpass_descriptions,
	_subpass_deps);

    _gDevice->create_frame_buffers_collection("frame_buffers_swap_chain",
	"pass1",
	2,
	_output_window->vk_swap_chain_image_views.data(),
	_output_window->width,
	_output_window->height,
	1);
    
    std::vector<unsigned char> _data;
    int _file_state = -1;
    
    auto _hr = wolf::system::io::read_binary_file("/home/pooyaeimandar/Desktop/SPIR-V/shader.vert.spv", 
            _data,  
            _file_state);
    
    if (_hr == S_OK)
    {
        logger.write("Hello");
    }
}

void scene::update(const wolf::system::w_game_time& pGameTime)
{
    logger.write(std::to_string(pGameTime.get_frames_per_second()));
    w_game::update(pGameTime);
}

void scene::begin_render(const wolf::system::w_game_time& pGameTime)
{	
    w_game::begin_render(pGameTime);
}

void scene::render(const wolf::system::w_game_time& pGameTime)
{
    w_game::render(pGameTime);
}

void scene::end_render(const wolf::system::w_game_time& pGameTime)
{
    w_game::end_render(pGameTime);
}

void scene::on_window_resized(UINT pIndex)
{
    w_game::on_window_resized(pIndex);
}

void scene::on_device_lost()
{
    w_game::on_device_lost();
}

ULONG scene::release()
{
    if (w_game::get_is_released()) return 0;

    return w_game::release();
}

