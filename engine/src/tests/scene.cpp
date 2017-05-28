#include "pch.h"
#include "scene.h"
#include <w_graphics/w_pipeline.h>
#include <w_content_manager.h>
#include <w_framework/w_quad.h>

#include <imgui/imgui_impl.h>

imgui_imp* imGui = nullptr;

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::framework;
using namespace wolf::content_pipeline;

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

//auto _quad = new w_quad<vertex_declaration_structs::vertex_position_color_uv>();
void scene::load()
{
    auto _gDevice = this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    float _width = static_cast<float>(_output_window->width);
    float _height = static_cast<float>(_output_window->height);

    w_game::load();

    //optional: create pipeline cache    
    std::string _pipeline_cache_name = "pipeline_cache_0";
    if (w_pipeline::create_pipeline_cache(_gDevice, _pipeline_cache_name) == S_FALSE)
    {
        logger.error("Could not create pipeline cache 0");
        _pipeline_cache_name.clear();
    }

    //create render pass
    w_viewport _viewport;
    _viewport.y = 0;
    _viewport.width = _width;
    _viewport.height = _height;
    _viewport.minDepth = 0;
    _viewport.maxDepth = 1;

    //we need depth
    //VkSubpassDescription _sub_pass = {};
    //_sub_pass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    //_sub_pass.colorAttachmentCount = 1;
    //_sub_pass.pColorAttachments = &w_graphics_device::w_render_pass_attachments::color_attachment_reference;
    //_sub_pass.pDepthStencilAttachment = &w_graphics_device::w_render_pass_attachments::depth_attachment_reference;
    //auto _sub_passes = new std::vector<VkSubpassDescription>{ _sub_pass };

    w_viewport_scissor _viewport_scissor;
    _viewport_scissor.offset.x = 0;
    _viewport_scissor.offset.y = 0;
    _viewport_scissor.extent.width = _width;
    _viewport_scissor.extent.height = _height;


    auto _hr = this->_render_pass.load(
        _gDevice,
        _viewport,
        _viewport_scissor,
        nullptr);
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating render pass");
        release();
        exit(1);
    }

    auto _render_pass_handle = this->_render_pass.get_handle();

    //load scene
    auto _scene = w_content_manager::load<w_cpipeline_scene>(content_path + L"models/test.dae");
    if (_scene)
    {
        //get all models
        std::vector<w_cpipeline_model*> _cmodels;
        _scene->get_all_models(_cmodels);

        if (_cmodels.size())
        {
            auto _z_up = _scene->get_z_up();
            auto _size = _cmodels.size();

            for (size_t i = 0; i < _size; ++i)
            {
                auto _model = new w_model<vertex_declaration_structs::vertex_position_uv>(_cmodels[i], _z_up);
                if (_cmodels[i]->get_instnaces_count())
                {
                    //load shader uniforms
                    auto _wvp = new w_uniform<world_view_projection_unifrom>();
                    auto _hr = _wvp->load(_gDevice);
                    if (_hr == S_FALSE)
                    {
                        logger.error("Error on loading world_view_projection uniform for instance model");
                        continue;
                    }
                    this->_instance_wvp_unifrom.push_back(_wvp);

                    auto _color = new w_uniform<color_unifrom>();
                    _hr = _color->load(_gDevice);
                    if (_hr == S_FALSE)
                    {
                        logger.error("Error on loading world_view_projection uniform for instance model");
                        continue;
                    }
                    this->_instance_color_unifrom.push_back(_color);

                    std::vector<w_shader_binding_param> _shader_params;

                    w_shader_binding_param _param;
                    _param.index = 0;
                    _param.type = w_shader_binding_type::UNIFORM;
                    _param.stage = w_shader_stage::VERTEX_SHADER;
                    _param.uniform_info = _wvp->get_descriptor_info();
                    _shader_params.push_back(_param);


                    _param.index = 1;
                    _param.type = w_shader_binding_type::SAMPLER;
                    _param.stage = w_shader_stage::FRAGMENT_SHADER;
                    _param.sampler_info = w_texture::default_texture->get_descriptor_info();
                    _shader_params.push_back(_param);

                    _param.index = 2;
                    _param.type = w_shader_binding_type::UNIFORM;
                    _param.stage = w_shader_stage::FRAGMENT_SHADER;
                    _param.uniform_info = _color->get_descriptor_info();
                    _shader_params.push_back(_param);

                    _hr = S_OK;
                    if (this->_basic_instance_shader)
                    {
                        this->_basic_instance_shader->set_shader_binding_params(_shader_params);
                    }
                    else
                    {
                        //load shader
                        w_shader* _shader = nullptr;
                        w_shader::load_to_shared_shaders(_gDevice,
                            "basic_instancing",
                            content_path + L"shaders/static_instancing_z_up.vert.spv",
                            content_path + L"shaders/basic.frag.spv",
                            _shader_params,
                            &_shader);
                        if (!_shader || _hr == S_FALSE)
                        {
                            logger.error("Could not load instance shader");
                            return;
                        }
                        this->_basic_instance_shader.reset(_shader);
                    }
                    if (_hr == S_OK)
                    {
                        if (_model->load(_gDevice, this->_basic_instance_shader.get(), &this->_render_pass, _pipeline_cache_name) == S_FALSE)
                        {
                            SAFE_RELEASE(_model);
                            continue;
                        }
                        this->_models.push_back(_model);
                    }
                }
                else
                {
                    //auto _hr = S_OK;
                    //std::call_once(_basic_shader_load_flag, [this, &_gDevice, &_z_up, &_hr]()
                    //{
                    //    //load shader
                    //    w_shader* _shader = nullptr;
                    //    _hr = w_shader::load_to_shared_shaders(_gDevice,
                    //        "basic_instancing",
                    //        content_path + (_z_up ? L"shaders/basic_instancing_z_up.vert.spv" : L"shaders/basic_instancing_y_up.vert.spv"),
                    //        content_path + L"shaders/basic.frag.spv",
                    //        w_shader_type::BASIC_SHADER,
                    //        {


                    //        },
                    //        &_shader);
                    //    if (!_shader || _hr == S_FALSE)
                    //    {
                    //        logger.error("Could not load instance shader");
                    //        return;
                    //    }
                    //    this->_basic_instance_shader.reset(_shader);
                    //});
                    //if (_hr == S_OK)
                    //{
                    //    if (_model->load(&this->_render_pass, _pipeline_cache_name) == S_FALSE)
                    //    {
                    //        SAFE_RELEASE(_model);
                    //        continue;
                    //    }
                    //    this->_models.push_back(_model);

                    //    //load shader uniforms
                    //    auto _vwp = new w_uniform<world_view_projection_unifrom>();
                    //    _hr = _vwp->load(_gDevice);
                    //    if (_hr == S_FALSE)
                    //    {
                    //        logger.error("Error on loading world_view_projection uniform for instance model");
                    //        release();
                    //        exit(1);
                    //    }
                    //    this->_basic_wvp_unifrom.push_back(_vwp);

                    //    auto _color = new w_uniform<color_unifrom>();
                    //    _hr = _color->load(_gDevice);
                    //    if (_hr == S_FALSE)
                    //    {
                    //        logger.error("Error on loading world_view_projection uniform for instance model");
                    //        release();
                    //        exit(1);
                    //    }
                    //    this->_basic_color_unifrom.push_back(_color);
                    //}
                }
            }
        }

        _scene->get_first_camera(this->_camera);
        this->_camera.set_aspect_ratio(_width / _height);
        this->_camera.update_view();
        this->_camera.update_projection();

        _scene->release();
    }

    //create frame buffers
    _hr = this->_frame_buffers.load(_gDevice,
        _render_pass_handle,
        _output_window->vk_swap_chain_image_views,
        &_output_window->vk_depth_buffer_image_view,
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
    this->_command_buffers.set_enable(true);
    this->_command_buffers.set_order(1);
    _hr = this->_command_buffers.load(_gDevice, _output_window->vk_swap_chain_image_views.size());
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating command buffers");
        release();
        exit(1);
    }

    _hr = _gDevice->store_to_global_command_buffers("render_quad_with_texture",
        &this->_command_buffers,
        _output_window->index);
    if (_hr == S_FALSE)
    {
        logger.error("Error creating command buffer");
        return;
    }

    _output_window->command_buffers.at("clear_color_screen")->set_enable(false);

    imGui = new imgui_imp();
    imGui->load(_gDevice, 800, 600, _render_pass_handle);
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
    for (uint32_t i = 0; i < this->_command_buffers.get_commands_size(); ++i)
    {
        this->_command_buffers.begin(i);
        {
            auto _cmd = this->_command_buffers.get_command_at(i);

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


            this->_render_pass.begin(_cmd,
                this->_frame_buffers.get_frame_buffer_at(i));
            {
                for (size_t i = 0; i < this->_models.size(); ++i)
                {
                    if (this->_models[i]->get_instances_count())
                    {
                        using namespace glm;

                        auto _transform = this->_models[i]->get_transform();
                        auto _translate = translate(mat4(1.0f),
                            vec3(_transform.position[0], _transform.position[1], _transform.position[2]));
                        mat4 _scale = scale(mat4x4(1.0f),
                            vec3(_transform.scale[0], _transform.scale[1], _transform.scale[2]));

                        auto _world = _translate *
                            rotate(_transform.rotation[0], true ? vec3(-1.0f, 0.0f, 0.0f) : vec3(1.0f, 0.0f, 0.0f)) *
                            rotate(_transform.rotation[1], vec3(0.0f, 1.0f, 0.0f)) *
                            rotate(_transform.rotation[2], vec3(0.0f, 0.0f, 1.0f)) *
                            _scale;

                        this->_instance_wvp_unifrom[i]->data.world = _world;
                        this->_instance_wvp_unifrom[i]->data.view_projection = this->_camera.get_projection() * this->_camera.get_view();
                        this->_instance_wvp_unifrom[i]->update();

                        this->_instance_color_unifrom[i]->data.color = glm::vec4(1);
                        this->_instance_color_unifrom[i]->update();

                        this->_models[i]->render(_cmd);

                    }
                }
                //make sure render all gui before loading gui_render
                imGui->new_frame();
                imGui->update_buffers(this->_render_pass);
                imGui->render(_cmd, (float)pGameTime.get_total_seconds());
            }
            this->_render_pass.end(_cmd);

            VkImageMemoryBarrier _barrier_from_render_to_present =
            {
                VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,                     // Type
                nullptr,                                                    // Next
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,                       // SrcAccessMask
                VK_ACCESS_MEMORY_READ_BIT,                                  // DstAccessMask
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                   // OldLayout
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,                            // NewLayout
                _gDevice->vk_graphics_queue_family_index,                   // SrcQueueFamilyIndex
                _gDevice->vk_present_queue_family_index,                    // DstQueueFamilyIndex
                _output_window->vk_swap_chain_image_views[i].image,         // Image
                _sub_resource_range                                         // SubresourceRange
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
                &_barrier_from_render_to_present);
        }
        this->_command_buffers.end(i);
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


#ifdef __WIN32

HRESULT scene::on_msg_proc(
    _In_ const HWND pHWND,
    _In_ const UINT pMessage,
    _In_ const WPARAM pWParam,
    _In_ const LPARAM pLParam)
{
    //switch (pMessage)
    //{
    //case WM_MOUSEMOVE:
    //    //this->_global_mouse_point.x = short(LOWORD(pLParam));
    //    //this->_global_mouse_point.y = short(HIWORD(pLParam));
    //    break;
    //}

    //if not procced yet
    return S_FALSE;
}

#endif

ULONG scene::release()
{
    if (this->get_is_released()) return 0;

    auto _gDevice = get_graphics_device();

    this->_render_pass.release();
    this->_frame_buffers.release();

    UNIQUE_RELEASE(this->_basic_shader);
    UNIQUE_RELEASE(this->_basic_instance_shader);

    w_pipeline::release_all_pipeline_caches(_gDevice);

    return w_game::release();
}

