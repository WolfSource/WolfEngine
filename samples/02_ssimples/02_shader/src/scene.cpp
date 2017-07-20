#include "scene.h"

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::framework;

scene::scene(_In_z_ std::string pRootDirectory, _In_z_ std::string pAppName) :
    w_game(pRootDirectory, pAppName)
{
    w_game::set_fixed_time_step(false);
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

void scene::load()
{
    w_game::load();
    
    auto _gDevice =  this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    auto _width = _output_window->width;
    auto _height = _output_window->height;
    auto _content_path_dir = wolf::system::io::get_current_directory() + "/../../../../../samples/02_Simples/02_shader/src/content/";
    
    //load shaders
    auto _hr = _shader.load(_gDevice, (_content_path_dir + "shaders/shader.vs.spv").c_str(), w_shader_stage::VERTEX_SHADER);
    if(_hr == S_FALSE)
    {
        logger.error("Error loading vertex shader.");
        return;
    }
    _hr = _shader.load(_gDevice, (_content_path_dir + "shaders/shader.fs.spv").c_str(), w_shader_stage::FRAGMENT_SHADER);
    if(_hr == S_FALSE)
    {
        logger.error("Error loading fragment shader.");
        return;
    }
    
    //create render pass
    _gDevice->create_render_pass("pass1");
    //create frame buffers
    _gDevice->create_frame_buffers_collection("frames",
                                              "pass1",
                                              _output_window->vk_swap_chain_image_views.size(),
                                              _output_window->vk_swap_chain_image_views.data(),
                                              _output_window->width,
                                              _output_window->height,
                                              1,
                                              _output_window->index);
    
    //create view port
    w_viewport _wp;
    _wp.x = 0.0f;
    _wp.y = 0.0f;
    _wp.width = _width;
    _wp.height =  _height;
    _wp.minDepth = 0.0f;
    _wp.maxDepth = 1.0f;
    
    w_viewport_scissor _wp_sc;
    _wp_sc.offset.x = 0;
    _wp_sc.offset.y = 0;
    _wp_sc.extent.width = _width;
    _wp_sc.extent.height = _height;
    
    auto _hr = _gDevice->create_pipeline("pipeline1",
                                         "pass1",
                                         _shader.get_shader_stages(),
                                         { _wp },
                                         {_wp_sc});
    if (_hr)
    {
        logger.error("Error creating pipeline");
        return;
    }
    
    this->_command_buffers = new w_command_buffers();
    _hr = this->_command_buffers->load(_gDevice, 2);
    if (_hr)
    {
        logger.error("Error loading command buffers");
        return;
    }
    
    _hr = _gDevice->store_to_global_command_buffers("render_triangle",
                                                    this->_command_buffers,
                                                    _output_window->index);
    if (_hr)
    {
        logger.error("Error creating command buffer");
        return;
    }
    
    _output_window->command_buffers.at("clear_color_screen")->set_enable(false);
    
    VkImageSubresourceRange _sub_resource_range = {};
    _sub_resource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    _sub_resource_range.baseMipLevel = 0;
    _sub_resource_range.levelCount = 1;
    _sub_resource_range.baseArrayLayer = 0;
    _sub_resource_range.layerCount = 1;
    
    VkClearValue _vk_clear_color =
    {
        { 0,1,0,1 }
    };
    
    uint32_t _present_queue_family_index = _gDevice->vk_present_queue_family_index;
    uint32_t _graphics_queue_family_index = _gDevice->vk_graphics_queue_family_index;
    
    //record clear screen command buffer for every swap chain image
    for (uint32_t i = 0; i < this->_command_buffers->get_commands_size(); ++i)
    {
        if(this->_command_buffers->begin(i) == S_FALSE)
        {
            logger.error("Error on beginning command buffer.");
            continue;
        }
        auto _cmd = this->_command_buffers->get_command_at(static_cast<size_t>(i));
        
        VkImageMemoryBarrier _present_to_render_barrier = {};
        _present_to_render_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        _present_to_render_barrier.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        _present_to_render_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        _present_to_render_barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        _present_to_render_barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        _present_to_render_barrier.srcQueueFamilyIndex = _present_queue_family_index;
        _present_to_render_barrier.dstQueueFamilyIndex = _graphics_queue_family_index;
        _present_to_render_barrier.image = _output_window->vk_swap_chain_image_views[i].image;
        _present_to_render_barrier.subresourceRange = _sub_resource_range;
        
        vkCmdPipelineBarrier(_cmd,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &_present_to_render_barrier);
        
        VkRenderPassBeginInfo _render_pass_begin_info =
        {
            VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,     // VkStructureType                sType
            nullptr,                                      // const void                    *pNext
            _gDevice->vk_render_passes["pass1"],                            // VkRenderPass                   renderPass
            _gDevice->vk_frame_buffers["frames"].at(i),                       // VkFramebuffer                  framebuffer
            {                                             // VkRect2D                       renderArea
                {                                           // VkOffset2D                     offset
                    0,                                          // int32_t                        x
                    0                                           // int32_t                        y
                },
                {                                           // VkExtent2D                     extent
                    _output_window->width,                                        // int32_t                        width
                    _output_window->height,                                        // int32_t                        height
                }
            },
            1,                                            // uint32_t                       clearValueCount
            &_vk_clear_color                                  // const VkClearValue            *pClearValues
        };
        
        vkCmdBeginRenderPass( _cmd, &_render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE );
        vkCmdBindPipeline( _cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, _gDevice->vk_pipelines["pipeline1"].pipeline);
        vkCmdDraw( _cmd, 3, 1, 0, 0 );
        vkCmdEndRenderPass( _cmd );
        
        VkImageMemoryBarrier _barrier_from_render_to_present = {
            VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,       // VkStructureType              sType
            nullptr,                                      // const void                  *pNext
            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,         // VkAccessFlags                srcAccessMask
            VK_ACCESS_MEMORY_READ_BIT,                    // VkAccessFlags                dstAccessMask
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,              // VkImageLayout                oldLayout
            VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,              // VkImageLayout                newLayout
            _graphics_queue_family_index,               // uint32_t                     srcQueueFamilyIndex
            _present_queue_family_index,                // uint32_t                     dstQueueFamilyIndex
           _output_window->vk_swap_chain_image_views[i].image,                  // VkImage                      image
            _sub_resource_range                       // VkImageSubresourceRange      subresourceRange
        };
        vkCmdPipelineBarrier(_cmd,
                             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                             VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                             0,
                             0,
                             nullptr,
                             0,
                             nullptr,
                             1,
                             &_barrier_from_render_to_present );

        
        if(this->_command_buffers->end(i) == S_FALSE)
        {
            logger.error("Error on ending command buffer.");
        }
    }
}

void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
    if (w_game::exiting) return;
    //logger.write(std::to_string(pGameTime.get_frames_per_second()));
    w_game::update(pGameTime);
}

HRESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
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
    
    this->_shader.release();
    //_command_buffers set to the w_graphics_device, so it will be delete automatically
    return w_game::release();
}

