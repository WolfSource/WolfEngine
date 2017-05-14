#include "pch.h"
#include "scene.h"
#include <w_graphics/w_shader.h>
#include <w_graphics/w_command_buffers.h>
#include <w_graphics/w_texture.h>
#include <w_graphics/w_uniform.h>
#include <glm/glm.hpp>
#include <glm_extention.h>
#include <w_content_manager.h>
#include <w_scene.h>


using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::framework;
using namespace wolf::content_pipeline;

#define VERTEX_BUFFER_BIND_ID 0
#define INSTANCE_BUFFER_BIND_ID 1

#if defined(__WIN32)
scene::scene(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName):
	w_game(pRunningDirectory, pAppName)
#elif defined(__UWP)
scene::scene(_In_z_ const std::wstring& pAppName):
	w_game(pAppName)
#else
scene::scene(_In_z_ const std::string& pRunningDirectory, _In_z_ const std::string& pAppName):
	w_game(pRunningDirectory, pAppName)
#endif
{
    w_game::set_fixed_time_step(false);
    
#if defined(__WIN32) || defined(__UWP)
    auto _running_dir = pRunningDirectory;
    content_path = _running_dir + L"../../../../content/";
#elif defined(__APPLE__)
    auto _running_dir = wolf::system::convert::string_to_wstring(pRunningDirectory);
    content_path = _running_dir + L"/../../../../../content/";
#endif
}

scene::~scene()
{
    release();
}

void scene::initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
    // TODO: Add your pre-initialization logic here
    w_game::initialize(pOutputWindowsInfo);
}

w_scene* _scene;

void scene::load()
{
    auto _gDevice =  this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    float _width = static_cast<float>(_output_window->width);
    float _height = static_cast<float>(_output_window->height);

    w_game::load();
    
    w_viewport _viewport;
    _viewport.y = 0;
    _viewport.width = _width;
    _viewport.height = _height;
    _viewport.minDepth = 0;
    _viewport.maxDepth = 1;

    w_viewport_scissor _viewport_scissor;
    _viewport_scissor.offset.x = 0;
    _viewport_scissor.offset.y = 0;
    _viewport_scissor.extent.width = _width;
    _viewport_scissor.extent.height = _height;

    
    std::string _pipeline_cache_name = "pipeline_cache_0";
    if (w_pipeline::create_pipeline_cache(_gDevice, _pipeline_cache_name) == S_FALSE)
    {
        logger.error("Could not create pipeline cache 0");
        _pipeline_cache_name.clear();
    }

    //create render pass
    auto _hr = this->_render_pass.load(_gDevice, _viewport, _viewport_scissor);
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating render pass");
        release();
        exit(1);
    }
    
    //auto _scene = w_content_manager::load<w_scene>(content_path + L"models/inst_max_oc.dae");
    _scene = w_content_manager::load<w_scene>(content_path + L"models/test.dae");
    this->_renderable_scene = new w_renderable_scene(_scene);
    this->_renderable_scene->load(_gDevice, &this->_render_pass, _pipeline_cache_name);
    this->_renderable_scene->get_first_or_default_camera(&this->_camera);
      
    this->_camera->set_aspect_ratio(_width / _height);
    this->_camera->update_view();
    this->_camera->update_projection();
        
    //create frame buffers
    auto _render_pass_handle = this->_render_pass.get_handle();
    _hr = this->_frame_buffers.load(_gDevice,
                              _render_pass_handle,
                              _output_window->vk_swap_chain_image_views.size(),
                              _output_window->vk_swap_chain_image_views.data(),
                              _width,
                              _height,
                              1);
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating frame buffers");
        release();
        exit(1);
    }
   
    
    //now assign new command buffers
    this->_command_buffers = new w_command_buffers();
    this->_command_buffers->set_enable(true);
    this->_command_buffers->set_order(1);
    _hr = this->_command_buffers->load(_gDevice, _output_window->vk_swap_chain_image_views.size());
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating command buffers");
        release();
        exit(1);
    }

    _hr = _gDevice->store_to_global_command_buffers("render_quad_with_texture",
                                                  this->_command_buffers,
                                                  _output_window->index);
    if (_hr == S_FALSE)
    {
        logger.error("Error creating command buffer");
        return;
    }

    _output_window->command_buffers.at("clear_color_screen")->set_enable(false);
}

void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
    if (w_game::exiting) return;
    
     w_game::update(pGameTime);
}

HRESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
    auto _gDevice = this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    
    VkImageSubresourceRange _sub_resource_range = {};
    _sub_resource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    _sub_resource_range.baseMipLevel = 0;
    _sub_resource_range.levelCount = 1;
    _sub_resource_range.baseArrayLayer = 0;
    _sub_resource_range.layerCount = 1;

    //record clear screen command buffer for every swap chain image
    for (uint32_t i = 0; i < this->_command_buffers->get_commands_size(); ++i)
    {
        this->_command_buffers->begin(i);
        {
            auto _command_buffer = this->_command_buffers->get_command_at(i);

            VkImageMemoryBarrier _present_to_render_barrier =
            {
                VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,                                         // Type
                nullptr,                                                                        // Next
                VK_ACCESS_MEMORY_READ_BIT,                                                      // SrcAccessMask
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,                                           // DstAccessMask
                VK_IMAGE_LAYOUT_UNDEFINED,                                                      // OldLayout
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                                       // NewLayout
                _gDevice->vk_present_queue_family_index,                                        // SrcQueueFamilyIndex
                _gDevice->vk_graphics_queue_family_index,                                       // DstQueueFamilyIndex
                _output_window->vk_swap_chain_image_views[i].image,                             // Image
                _sub_resource_range                                                             // subresourceRange
            };

            vkCmdPipelineBarrier(_command_buffer,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &_present_to_render_barrier);


            this->_render_pass.begin(_command_buffer,
                this->_frame_buffers.get_frame_buffer_at(i));
            {
                this->_renderable_scene->update(this->_camera->get_projection() * this->_camera->get_view());
                this->_renderable_scene->render(_command_buffer);
            }
            this->_render_pass.end(_command_buffer);

            VkImageMemoryBarrier _barrier_from_render_to_present =
            {
                VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,                     // Type
                nullptr,                                                    // Next
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,                       // SrcAccessMask
                VK_ACCESS_MEMORY_READ_BIT,                                  // DstAccessMask
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                   // OldLayout
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                            // NewLayout
                _gDevice->vk_graphics_queue_family_index,                    // SrcQueueFamilyIndex
                _gDevice->vk_present_queue_family_index,                   // DstQueueFamilyIndex
                _output_window->vk_swap_chain_image_views[i].image,         // Image
                _sub_resource_range                                         // SubresourceRange
            };

            vkCmdPipelineBarrier(_command_buffer,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                0,
                0,
                nullptr,
                0,
                nullptr,
                1,
                &_barrier_from_render_to_present);
        }
        this->_command_buffers->end(i);
    }

    logger.write(std::to_string(pGameTime.get_frames_per_second()));
    return w_game::render(pGameTime);
}

void scene::on_window_resized(_In_ UINT pIndex)
{
    w_game::on_window_resized(pIndex);
}

void scene::on_device_lost()
{
    w_game::on_device_lost();
}

ULONG scene::release()
{
    if (this->get_is_released()) return 0;

    auto _gDevice = get_graphics_device();

    this->_render_pass.release();
    this->_frame_buffers.release();

    w_pipeline::release_all_pipeline_caches(_gDevice);

    return w_game::release();
}

