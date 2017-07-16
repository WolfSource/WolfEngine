#include "pch.h"
#include "scene.h"
#include <w_graphics/w_pipeline.h>
#include <w_content_manager.h>
#include <w_framework/w_quad.h>
#include <imgui/imgui.h>
#include <w_graphics/w_imgui.h>
#include "masked_occlusion_culling/MaskedOcclusionCulling.h"
#include <cameras/w_camera.h>
#include <mutex>
#include <w_task.h>
#include <w_thread.h>
#include <w_timer.h>
#include <tbb/parallel_for_each.h>

//#define DEBUG_MASKED_OCCLUSION_CULLING
#define MAX_SEARCH_LENGHT 256

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::framework;
using namespace wolf::content_pipeline;

//forward declaration
static void TonemapDepth(_In_ float* pDepth, _In_ unsigned char* pImage, _In_ const int& pW, _In_ const int& pH);

static bool sSearching = false;
static bool sForceUpdateCamera = true;

static std::vector<search_item_struct> sSearchedItems;

static char sSearch[MAX_SEARCH_LENGHT];
static MaskedOcclusionCulling* sMOC;
static float* sMOCPerPixelZBuffer = nullptr; 
static uint8_t* sMOCTonemapDepthImage = nullptr;
static UINT32 sFPS = 0;
static std::string SVersion;


w_thread _t;

#if defined(__WIN32)
scene::scene(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName):
	w_game(pRunningDirectory, pAppName)

#elif defined(__UWP)
scene::scene(_In_z_ const std::wstring& pAppName):
	w_game(pAppName)
#else
scene::scene(_In_z_ const std::string& pRunningDirectory, _In_z_ const std::string& pAppName) :
    w_game(pRunningDirectory, pAppName)
#endif
{
#if defined(__WIN32) || defined(__UWP)
    auto _running_dir = pRunningDirectory;
    content_path = _running_dir + L"../../../../content/";
#elif defined(__APPLE__)
    auto _running_dir = wolf::system::convert::string_to_wstring(pRunningDirectory);
    content_path = _running_dir + L"/../../../../../content/";
#endif

    this->_show_gui = true;
    this->_draw_fence = 0;

    //we do not need fixed time step
    w_game::set_fixed_time_step(false);

    //enable/disable gpu debugging
    w_graphics_device_manager_configs _config;
    _config.debug_gpu = true;
    this->set_graphics_device_manager_configs(_config);

    SVersion =
        std::to_string(WOLF_MAJOR_VERSION) + "." + std::to_string(WOLF_MINOR_VERSION) +
        "." + std::to_string(WOLF_PATCH_VERSION) + "." + std::to_string(WOLF_DEBUG_VERSION);
}

scene::~scene()
{
    release();
}

void scene::initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
    this->on_device_features_fetched += [](w_graphics_device_manager::w_device_features_extensions& pDeviceFeaturesExtensions)
    {
        auto _fe = pDeviceFeaturesExtensions.device_features;
        //if (_fe && (*_fe).tessellationShader == VK_FALSE)
        //{
        //    logger.write("Tesselation not supported for graphics device:" +
        //        std::string(pDeviceFeaturesExtensions.get_device_name()) +
        //        " ID:" + std::to_string(pDeviceFeaturesExtensions.get_device_id()));
        //}
        // Fill mode non solid is required for wireframe display
        (*_fe).fillModeNonSolid = VK_TRUE;
    };
    // TODO: Add your pre-initialization logic here
    w_game::initialize(pOutputWindowsInfo);

    auto _numthreads = w_thread::get_number_of_hardware_thread_contexts();
    _thread_pool.allocate(_numthreads);
    logger.write("Size of thread pool is " + std::to_string(_numthreads));

    //create masked occlusion culling instnace
    sMOC = MaskedOcclusionCulling::Create();
    MaskedOcclusionCulling::Implementation _implementation = sMOC->GetImplementation();
    switch (_implementation)
    {
    case MaskedOcclusionCulling::SSE2:
        logger.write("Using SSE2 implementation of MaskedOcclusionCulling");
        break;
    case MaskedOcclusionCulling::SSE41:
        logger.write("Using SSE41 implementation of MaskedOcclusionCulling");
        break;
    case MaskedOcclusionCulling::AVX2:
        logger.write("Using AVX2 implementation of MaskedOcclusionCulling");
        break;
    }

    sprintf_s(sSearch, "Search");

}

void scene::load()
{
    const std::string _trace = this->name + "::load";

    auto _gDevice = this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    _screen_size.x = static_cast<uint32_t>(_output_window->width);
    _screen_size.y = static_cast<uint32_t>(_output_window->height);

    w_game::load();
       

#ifdef DEBUG_MASKED_OCCLUSION_CULLING
    {
        size_t _size = _screen_size.x * _screen_size.y;
        sMOCPerPixelZBuffer = (float*)malloc(_size * sizeof(float));
        sMOCTonemapDepthImage = (uint8_t*)malloc(_size * 3 * sizeof(uint8_t));
    }
#endif
       
    this->_viewport.y = 0;
    this->_viewport.width = static_cast<float>(_screen_size.x);
    this->_viewport.height = static_cast<float>(_screen_size.y);
    this->_viewport.minDepth = 0;
    this->_viewport.maxDepth = 1;

    this->_viewport_scissor.offset.x = 0;
    this->_viewport_scissor.offset.y = 0;
    this->_viewport_scissor.extent.width = _screen_size.x;
    this->_viewport_scissor.extent.height = _screen_size.y;

    auto _depth_attachment = w_graphics_device::w_render_pass_attachments::depth_attachment_description;
    _depth_attachment.format = _output_window->vk_depth_buffer_format;

    std::vector<VkAttachmentDescription> _attachments =
    {
        w_graphics_device::w_render_pass_attachments::color_attachment_description,
        _depth_attachment,
    };

    //create draw render pass
    auto _hr = this->_draw_render_pass.load(
        _gDevice,
        _viewport,
        _viewport_scissor,
        _attachments);

    if (_hr == S_FALSE)
    {
        logger.error("Error on creating render pass");
        release();
        w_game::exiting = true;
        return;
    }

    // Color attachments
    // Don't clear the framebuffer (like the renderpass from the example does)
    _attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;

    //Depth attachment
    _attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    _attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    _hr = this->_gui_render_pass.load(
        _gDevice,
        _viewport,
        _viewport_scissor,
        _attachments);

    if (_hr == S_FALSE)
    {
        logger.error("Error on creating render pass");
        release();
        w_game::exiting = true;
        return;
    }
    
    auto _render_pass_handle = this->_draw_render_pass.get_handle();

    //create pipeline_cache for drawing models
    std::string _pipeline_cache_name = "model_pipeline_cache";
    if (w_pipeline::create_pipeline_cache(_gDevice, _pipeline_cache_name) == S_FALSE)
    {
        logger.error("Could not create pipeline cache");
        _pipeline_cache_name.clear();
    }

    //create frame buffers
    _hr = this->_draw_frame_buffers.load(_gDevice,
        _render_pass_handle,
        _output_window->vk_swap_chain_image_views,
        &_output_window->vk_depth_buffer_image_view,
        _screen_size,
        1);
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating draw frame buffers");
        release();
        exit(1);
    }

    //create gui frame buffers
    auto _gui_render_pass_handle = this->_gui_render_pass.get_handle();
    _hr = this->_gui_frame_buffers.load(_gDevice,
        _gui_render_pass_handle,
        _output_window->vk_swap_chain_image_views,
        &_output_window->vk_depth_buffer_image_view,
        _screen_size,
        1);
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating gui frame buffers");
        release();
        exit(1);
    }

    VkSemaphoreCreateInfo _semaphore_create_info = {};
    _semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(_gDevice->vk_device,
        &_semaphore_create_info,
        nullptr,
        &gui_semaphore))
    {
        logger.error("Error on creating semaphore for gui command buffer");
        release();
        exit(1);
    }
    
    //Fence for render sync
    VkFenceCreateInfo _fence_create_info = {};
    _fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    _fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(
        _gDevice->vk_device,
        &_fence_create_info,
        nullptr,
        &this->_draw_fence))
    {
        V(S_FALSE, "creating draw fence", _trace);
        release();
        exit(1);
    }


    auto _swap_chain_image_size = _output_window->vk_swap_chain_image_views.size();

    //load primary command buffer
    _hr = this->_draw_command_buffers.load(_gDevice, _swap_chain_image_size);
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating command buffers");
        release();
        exit(1);
    }

    _hr = this->_threads_compute_commands_buffers.load(
        _gDevice,
        _thread_pool.get_pool_size(),
        VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY,
        true,
        &_gDevice->vk_compute_queue);
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating thread compute command buffers");
        release();
        exit(1);
    }

    _hr = this->_threads_draw_commands_buffers.load(
        _gDevice,
        _thread_pool.get_pool_size(),
        VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY);
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating thread draw command buffers");
        release();
        exit(1);
    }

    //load image texture
    w_texture* _gui_images = new w_texture();
    _gui_images->load(_gDevice);


    _gui_images->initialize_texture_2D_from_file(L"textures/gui/icons.png");

    //set images which will be used by imgui

#ifdef DEBUG_MASKED_OCCLUSION_CULLING
    w_texture* _debug_image = new w_texture();
    _debug_image->load(_gDevice);
    _debug_image->initialize_texture_2D_from_file(L"C:\\Users\\DFM\\Desktop\\header.jpg", true);

    w_shader_binding_param _param;
    _param.index = 0;
    _param.stage = w_shader_stage::FRAGMENT_SHADER;
    _param.type = w_shader_binding_type::SAMPLER;
    _param.image_info = _debug_image->get_descriptor_info();

    //w_shader* _basic;
    //w_shader::load_shader(
    //    _gDevice, 
    //    "basic", 
    //    L"shaders/basic.vert.spv", 
    //    L"", 
    //    L"", 
    //    L"", 
    //    L"", 
    //    L"shaders/basic.frag.spv",
    //    { _param },
    //    true, 
    //    &_basic);
    //_q.load(_gDevice, _basic, &this->_draw_render_pass, "", false);

#endif

    auto _font_path = wolf::system::convert::wstring_to_string(content_path) + "fonts/arial.ttf";
    w_imgui::load(_gDevice, _output_window->hwnd, _screen_size, _render_pass_handle, _gui_images, _font_path.c_str());

    _hr = this->_gui_command_buffers.load(_gDevice, _output_window->vk_swap_chain_image_views.size());
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating gui command buffers");
        release();
        exit(1);
    }
    _load_areas();
}

HRESULT scene::_load_areas()
{
    _thread_pool.set_job_for_thread(0, [this]()
    {
        auto _gDevice = this->graphics_devices[0];
        const std::vector<std::wstring> _areas_on_thread_0 =
        {
            L"models/model.dae",
        };

        std::for_each(_areas_on_thread_0.begin(), _areas_on_thread_0.end(),
            [&](_In_ const std::wstring& pAreaPath)
        {

            auto _compute_cmd = this->_threads_compute_commands_buffers.get_command_at(0);
            auto _path = content_path + pAreaPath;
            auto _scene = w_content_manager::load<w_cpipeline_scene>(_path);
            if (_scene)
            {
                //just for converting
                //std::vector<w_cpipeline_scene> _scenes = { *_scene };
                //w_content_manager::save_wolf_scenes_to_file(_scenes, 
                //    content_path + 
                //    wolf::system::io::get_parent_directoryW(pAreaPath) + L"/" +
                //    wolf::system::io::get_base_file_nameW(pAreaPath) + L".wscene");
                //_scenes.clear();

                //get all models
                std::vector<w_cpipeline_model*> _cmodels;
                _scene->get_all_models(_cmodels);

                for (auto& _iter : _cmodels)
                {
                    if (!_iter) continue;

                    auto _model = new model();
                    if (_model->load(_gDevice, _iter, this->_draw_render_pass) == S_OK)
                    {
                        _model->build_compute_command_buffers(_compute_cmd);
                        this->_models.push_back(_model);
                    }
                    else
                    {
                        SAFE_DELETE(_model);
                        logger.error("Error on loading model " + _iter->get_name());
                    }
                }
                _scene->release();

                sForceUpdateCamera = true;
            }
            else
            {
                logger.write(L"Scene on following path not exists " + _path);
            }
        });
    });

    //load camera
    auto _scene = w_content_manager::load<w_cpipeline_scene>(content_path + L"models/camera.dae");
    if (_scene)
    {
        _scene->get_first_camera(this->_camera);

        float _near_plan = 0.1f, far_plan = 5000;

        this->_camera.set_near_plan(_near_plan);
        sMOC->SetNearClipPlane(_near_plan);

        this->_camera.set_far_plan(far_plan);

        auto _screen_width = (float)_screen_size.x;
        auto _screen_height = (float)_screen_size.y;

        this->_camera.set_aspect_ratio(_screen_width / _screen_height);
        sMOC->SetResolution(_screen_width, _screen_height);

        this->_camera.update_view();
        this->_camera.update_projection();
        this->_camera.update_frustum();
    }

    return S_OK;
}

static std::vector<VkSemaphore> ss;
HRESULT scene::_build_draw_command_buffer(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    ss.clear();
    //record draw command buffers for every swap chain image
    std::vector<VkCommandBuffer> _secondary_commands;
    
    VkCommandBufferBeginInfo _cmd_buffer_begin_info{};
    _cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    for (uint32_t i = 0; i < this->_draw_command_buffers.get_commands_size(); ++i)
    {
        _secondary_commands.clear();
        this->_draw_command_buffers.begin(i);
        {
            auto _render_pass_handle = this->_draw_render_pass.get_handle();
            auto _frame_buffer_handle = this->_draw_frame_buffers.get_frame_buffer_at(i);

            VkCommandBufferInheritanceInfo _inheritance_info = {};
            _inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
            _inheritance_info.renderPass = _render_pass_handle;
            _inheritance_info.framebuffer = _frame_buffer_handle;

            auto _cmd = this->_draw_command_buffers.get_command_at(i);
            this->_draw_render_pass.begin(_cmd,
                _frame_buffer_handle,
                w_color(43, 43, 43, 255),
                1.0f,
                0.0f,
                VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
            {
                _thread_pool.set_job_for_thread(0, [&]()
                {
                    auto _compute_cmd = this->_threads_compute_commands_buffers.get_command_at(0);
                    auto _draw_cmd = this->_threads_draw_commands_buffers.get_command_at(0);

                    std::for_each(this->_models_to_be_render.begin(), _models_to_be_render.end(),
                        [&](_In_ model* pModel)
                    {
                        pModel->execute_compute_shader(_compute_cmd, &this->_camera);
                        ss.push_back(pModel->get_compute_semaphore());

                        vkBeginCommandBuffer(_draw_cmd, &_cmd_buffer_begin_info);
                        {
                            vkCmdSetViewport(_draw_cmd, 0, 1, &this->_viewport);
                            vkCmdSetScissor(_draw_cmd, 0, 1, &this->_viewport_scissor);
                            pModel->indirect_draw(_draw_cmd);
                        }
                        vkEndCommandBuffer(_draw_cmd);
                        _secondary_commands.push_back(_draw_cmd);
                    });
                });

                _thread_pool.wait_all();

                // Execute secondary commands buffer to primary command
                if (_secondary_commands.size())
                {
                    vkCmdExecuteCommands(_cmd, _secondary_commands.size(), _secondary_commands.data());
                }
            }
            this->_draw_render_pass.end(_cmd);
        }
        this->_draw_command_buffers.end(i);
    }

    return S_OK;
}

HRESULT scene::_build_gui_command_buffer(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const w_game_time& pGameTime)
{
    //record clear screen command buffer for every swap chain image
    for (uint32_t i = 0; i < this->_gui_command_buffers.get_commands_size(); ++i)
    {
        this->_gui_command_buffers.begin(i);
        {
            auto _cmd = this->_gui_command_buffers.get_command_at(i);
                        
            this->_gui_render_pass.begin(_cmd, this->_gui_frame_buffers.get_frame_buffer_at(i));
            {
                //make sure render all gui before loading gui_render
                w_imgui::update_buffers(this->_gui_render_pass);
                w_imgui::render(_cmd);
            }
            this->_gui_render_pass.end(_cmd);
        }
        this->_gui_command_buffers.end(i);
    }
    return S_OK;
}

void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
    if (w_game::exiting) return;
    defer(nullptr, [&](...)
    {
        w_game::update(pGameTime);
    });

    auto _gDevice = this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);

    bool _gui_proceeded = false;
    if (this->_show_gui)
    {
        w_imgui::new_frame(windows_frame_time_in_sec.at(0), [&_gui_proceeded, this]()
        {
            _gui_proceeded = _update_gui();
        });
    }

    //if gui proceeded or mouse captured by gui do not update view
    bool _camera_just_updated = false;
    if (!sForceUpdateCamera)
    {
        if (_gui_proceeded)
        {
            inputs_manager.reset();
            return;
        }
        if (ImGui::IsMouseHoveringAnyWindow()) return;

        _camera_just_updated = this->_camera.update(pGameTime, this->_screen_size);
    }
    
    //if camera updated, then update command buffers based on result of masked occlusion culling

    if (sForceUpdateCamera || _camera_just_updated)
    {
        sForceUpdateCamera = false;

        //order by distance to camera
        //std::sort(this->_models.begin(), this->_models.end(), [&_cam_pos](_In_ model* a, _In_ model* b)
        //{
        //    auto _a_distance = glm::distance(_cam_pos, a->get_position());
        //    auto _b_distance = glm::distance(_cam_pos, b->get_position());

        //    return _a_distance < _b_distance;
        //});  

        if (this->_models.size())
        {
            sMOC->ClearBuffer();
            this->_models_to_be_render.clear();

            std::for_each(this->_models.begin(), this->_models.end(), [this](_In_ model* pModel)
            {
                pModel->pre_update(this->_camera, &sMOC);
            });

            std::for_each(this->_models.begin(), this->_models.end(), [&](_In_ model* pModel)
            {
                if (pModel->post_update(sMOC))
                {
                    this->_models_to_be_render.push_back(pModel);
                }
            });
        }
        _build_draw_command_buffer(_gDevice);

#ifdef DEBUG_MASKED_OCCLUSION_CULLING
        //Compute a per pixel depth buffer from the hierarchical depth buffer, used for visualization.
        sMOC->ComputePixelDepthBuffer(sMOCPerPixelZBuffer);
        //Tonemap the depth image
        TonemapDepth(sMOCPerPixelZBuffer, sMOCTonemapDepthImage, _output_window->width, _output_window->height);
        w_texture::write_bitmap_to_file("F:\\MOC.bmp", sMOCTonemapDepthImage, _output_window->width, _output_window->height);
#endif
    }
}

HRESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
    const std::string _trace = this->name + "::render";

    HRESULT _hr = S_OK;

    auto _gDevice = this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    auto _frame_index = _output_window->vk_swap_chain_image_index;
    
    auto _cmd = this->_draw_command_buffers.get_command_at(_frame_index);

    const VkPipelineStageFlags _stage_flags[2] =
    {
        VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    };

    VkSubmitInfo _submit_info = {};
    _submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    _submit_info.commandBufferCount = 1;
    _submit_info.pCommandBuffers = &_cmd;
    _submit_info.pWaitDstStageMask = &_stage_flags[0];
    _submit_info.waitSemaphoreCount = 1;
    _submit_info.pWaitSemaphores = &_output_window->vk_swap_chain_image_is_available_semaphore;
    _submit_info.signalSemaphoreCount = 1;
    _submit_info.pSignalSemaphores = this->_show_gui ? &gui_semaphore : &_output_window->vk_rendering_done_semaphore; //signal to gui if gui is avaiable, otherwise end the render 

    // Submit to queue
    vkResetFences(_gDevice->vk_device, 1, &this->_draw_fence);
    if (vkQueueSubmit(_gDevice->vk_graphics_queue.queue, 1, &_submit_info, this->_draw_fence))
    {
        _hr = S_FALSE;
        V(_hr, "submiting queu for drawing gui", _trace, 3);
    }
    // Wait for fence to signal that all command buffers are ready
    vkWaitForFences(_gDevice->vk_device, 1, &this->_draw_fence, VK_TRUE, VK_TIMEOUT);
   
    if (this->_show_gui)
    {
        _build_gui_command_buffer(_gDevice, pGameTime);

        VkPipelineStageFlags _stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        _cmd = this->_gui_command_buffers.get_command_at(_frame_index);

        VkSubmitInfo _gui_submit_info = {};
        _gui_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        _gui_submit_info.commandBufferCount = 1;
        _gui_submit_info.pCommandBuffers = &_cmd;
        _gui_submit_info.pWaitDstStageMask = &_stage_flags;
        _gui_submit_info.waitSemaphoreCount = 1;
        _gui_submit_info.pWaitSemaphores = &gui_semaphore;
        _gui_submit_info.signalSemaphoreCount = 1;
        _gui_submit_info.pSignalSemaphores = &_output_window->vk_rendering_done_semaphore;

        // Submit to queue
        if (vkQueueSubmit(_gDevice->vk_graphics_queue.queue, 1, &_gui_submit_info, 0))
        {
            _hr = S_FALSE;
            V(_hr, "submiting queu for drawing gui", _trace, 3);
        }
    }
    _hr = w_game::render(pGameTime);

    sFPS = pGameTime.get_frames_per_second();

    return _hr;
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
    if (w_game::get_is_released()) return 0;

    auto _gDevice = get_graphics_device();
    auto _device = _gDevice->vk_device;
    
    this->_thread_pool.release();

    //release all models
    for (auto& _iter : this->_models)
    {
        SAFE_RELEASE(_iter);
    }

    this->_draw_render_pass.release();
    this->_draw_frame_buffers.release();
    this->_draw_command_buffers.release();

    this->_gui_render_pass.release();
    this->_gui_frame_buffers.release();
    this->_gui_command_buffers.release();
    if (this->gui_semaphore)
    {
        vkDestroySemaphore(_device, this->gui_semaphore, nullptr);
        this->gui_semaphore = 0;
    }
    
    if (this->_draw_fence)
    {
        vkDestroyFence(_device, this->_draw_fence, nullptr);
        this->_draw_fence = 0;
    }

    this->_threads_compute_commands_buffers.release();
    this->_threads_draw_commands_buffers.release();

#ifdef DEBUG_MASKED_OCCLUSION_CULLING
    free(sMOCPerPixelZBuffer);
    free(sMOCTonemapDepthImage);
#endif

    MaskedOcclusionCulling::Destroy(sMOC);

    w_imgui::release();

    w_pipeline::release_all_pipeline_caches(_gDevice);
    w_texture::release_shared_textures();

    //wait one sec before closing to ensure all threads destroyed successfully
    w_thread::sleep_current_thread(1000);

    return w_game::release();
}

bool scene::_update_gui()
{
    bool _proceeded = false;

    //ImGui::ShowTestWindow();

#pragma region Setting Style
    ImGuiStyle& _style = ImGui::GetStyle();
    _style.WindowPadding = ImVec2(3, 2);
    _style.WindowRounding = 0;
    _style.GrabRounding = 4;
    _style.GrabMinSize = 20;
    _style.FramePadding = ImVec2(5, 5);

    _style.Colors[ImGuiCol_Text].x = 1.0f;
    _style.Colors[ImGuiCol_Text].y = 1.0f;
    _style.Colors[ImGuiCol_Text].z = 1.0f;
    _style.Colors[ImGuiCol_Text].w = 1.0f;
    
    _style.Colors[ImGuiCol_WindowBg].x = 0.9098039215686275f;
    _style.Colors[ImGuiCol_WindowBg].y = 0.4431372549019608f;
    _style.Colors[ImGuiCol_WindowBg].z = 0.3176470588235294f;
    _style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    
    _style.Colors[ImGuiCol_FrameBg].x = 1.0f;
    _style.Colors[ImGuiCol_FrameBg].y = 1.0f;
    _style.Colors[ImGuiCol_FrameBg].z = 1.0f;
    _style.Colors[ImGuiCol_FrameBg].w = 1.0f;

    _style.Colors[ImGuiCol_TextSelectedBg].x = 0.9098039215686275f;
    _style.Colors[ImGuiCol_TextSelectedBg].y = 0.4431372549019608f;
    _style.Colors[ImGuiCol_TextSelectedBg].z = 0.3176470588235294f;
    _style.Colors[ImGuiCol_TextSelectedBg].w = 0.6f;
#pragma endregion

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    //window_flags |= ImGuiWindowFlags_ShowBorders;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    //window_flags |= ImGuiWindowFlags_MenuBar;

#pragma region Left Buttons
    if (!ImGui::Begin("Buttons", 0, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return _proceeded;
    }

    ImGui::SetWindowPos(ImVec2(0, 0));

    for (int i = 0; i < 6; i++)
    {
        ImTextureID tex_id = (void*)("#");
        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_ImageActive, ImColor(0.0f, 0.0f, 255.0f, 255.0f));
        ImGui::PushStyleColor(ImGuiCol_ImageHovered, ImColor(0.0f, 0.0f, 255.0f, 155.0f));
        if (ImGui::ImageButton(tex_id, ImVec2(32, 32), ImVec2(i * 0.1, 0.0), ImVec2(i * 0.1 + 0.1f, 0.1), 0, ImColor(232, 113, 83, 255)))
        {
            logger.write("Button");
            _proceeded = true;
        }
        ImGui::PopStyleColor(2);
        ImGui::PopID();
        ImGui::Spacing();
        ImGui::Spacing();
    }
    ImGui::End();

#pragma endregion

#pragma region Search
    _style.WindowPadding = ImVec2(1, 4);

    _style.Colors[ImGuiCol_Text].x = 0.1f;
    _style.Colors[ImGuiCol_Text].y = 0.1f;
    _style.Colors[ImGuiCol_Text].z = 0.1f;
    _style.Colors[ImGuiCol_Text].w = 1.0f;

    _style.Colors[ImGuiCol_WindowBg].x = 1.0f;
    _style.Colors[ImGuiCol_WindowBg].y = 1.0f;
    _style.Colors[ImGuiCol_WindowBg].z = 1.0f;
    _style.Colors[ImGuiCol_WindowBg].w = 1.0f; 

    ImGui::SetNextWindowContentSize(ImVec2(300, 20));
    if (!ImGui::Begin("Search", 0, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return _proceeded;
    }

    ImGui::SetWindowPos(ImVec2(38, 0));
    ImGui::PushItemWidth(299);
    if (ImGui::InputText("", sSearch, MAX_SEARCH_LENGHT, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        _proceeded = true;

        sSearchedItems.clear();
        if (!sSearching)
        {
            if (sSearch[0] != '\0')
            {
                sSearching = true;
                std::string _lower_str(sSearch);
                std::transform(_lower_str.begin(), _lower_str.end(), _lower_str.begin(), ::tolower);
                w_task::execute_async_ppl([this, _lower_str]()
                {
                    //start seraching models
                    std::for_each(this->_models.begin(), this->_models.end(), [_lower_str](_In_ model* pModel)
                    {
                        pModel->search_for_name(_lower_str, sSearchedItems);
                    });
                }, [this]()
                {
                    //on callback
                    sSearching = false;
                    if (sSearchedItems.size() == 0)
                    {
                        //switch all model's transparency to 1
                        std::for_each(this->_models.begin(), this->_models.end(), [](_In_ model* pModel)
                        {
                            pModel->set_color(glm::vec4(1.0f));
                        });
                    }
                });
            }
            else
            {
                //switch all model's transparency to 1
                std::for_each(this->_models.begin(), this->_models.end(), [](_In_ model* pModel)
                {
                    pModel->set_color(glm::vec4(1.0f));
                });
            }
        }
    }
    
    ImGui::PopItemWidth();
    ImGui::End();
#pragma endregion

    if (sSearchedItems.size())
    {
#pragma region Show Search Results
        _style.Colors[ImGuiCol_Text].x = 0.1f;
        _style.Colors[ImGuiCol_Text].y = 0.1f;
        _style.Colors[ImGuiCol_Text].z = 0.1f;
        _style.Colors[ImGuiCol_Text].w = 1.0f;

        _style.Colors[ImGuiCol_WindowBg].x = 1.0f;
        _style.Colors[ImGuiCol_WindowBg].y = 1.0f;
        _style.Colors[ImGuiCol_WindowBg].z = 1.0f;
        _style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        window_flags = 0;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoMove;

        ImGui::SetNextWindowSizeConstraints(ImVec2(300, 229), ImVec2(300, 600));
        ImGui::SetNextWindowContentWidth(300);

        if (!ImGui::Begin("Search Results", 0, window_flags))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return _proceeded;
        }

        ImGui::SetWindowPos(ImVec2(38, 35));
        ImGui::PushItemWidth(299);
        
        if (ImGui::CollapsingHeader("Area 120"))
        {
            if (ImGui::TreeNode("Water Treatment"))
            {
                for (int i = 0; i < sSearchedItems.size(); ++i)
                {
                    ImGuiTreeNodeFlags _node_flags =
                        ImGuiTreeNodeFlags_OpenOnArrow | 
                        ImGuiTreeNodeFlags_OpenOnDoubleClick |
                        ImGuiTreeNodeFlags_Selected | 
                        ImGuiTreeNodeFlags_Leaf | 
                        ImGuiTreeNodeFlags_NoTreePushOnOpen;
                    ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, sSearchedItems[i].name.c_str());
                    if (ImGui::IsItemClicked())
                    {
                        this->_camera.set_interest(
                            sSearchedItems[i].bounding_sphere.center[0],
                            sSearchedItems[i].bounding_sphere.center[1],
                            sSearchedItems[i].bounding_sphere.center[2]);

                        auto _radius = +sSearchedItems[i].bounding_sphere.radius * 3.0f;
                        this->_camera.set_translate(
                            sSearchedItems[i].bounding_sphere.center[0],
                            sSearchedItems[i].bounding_sphere.center[1] + _radius,
                            sSearchedItems[i].bounding_sphere.center[2] + _radius);

                        this->_camera.update_view();
                        this->_camera.update_projection();
                        this->_camera.update_frustum();

                        sForceUpdateCamera = true;
                    }
                }
                ImGui::TreePop();
            }
        }

        ImGui::PopItemWidth();
        ImGui::End();
#pragma endregion
    }
    
#pragma region Debug Window

    _style.WindowPadding = ImVec2(3, 2);

    _style.Colors[ImGuiCol_Text].x = 1.0f;
    _style.Colors[ImGuiCol_Text].y = 1.0f;
    _style.Colors[ImGuiCol_Text].z = 1.0f;
    _style.Colors[ImGuiCol_Text].w = 1.0f;

    _style.Colors[ImGuiCol_WindowBg].x = 0.9098039215686275f;
    _style.Colors[ImGuiCol_WindowBg].y = 0.4431372549019608f;
    _style.Colors[ImGuiCol_WindowBg].z = 0.3176470588235294f;
    _style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    
    ImGui::SetNextWindowSize(ImVec2(50, 50), ImGuiSetCond_FirstUseEver);
    ImGui::Begin(("Wolf.Engine " + SVersion).c_str());
    ImGui::Text("FPS:%d", sFPS);
    ImGui::Text("FrameTime:%f", windows_frame_time_in_sec.at(0));
        
    ImGui::End();
    
#pragma endregion

    return _proceeded;
}

static void TonemapDepth(_In_ float* pDepth, _In_ unsigned char* pImage, _In_ const int& pW, _In_ const int& pH)
{
    // Find min/max w coordinate (discard cleared pixels)
    float _minW = FLT_MAX, _maxW = 0.0f;
    for (int i = 0; i < pW * pH; ++i)
    {
        if (pDepth[i] > 0.0f)
        {
            _minW = std::min(_minW, pDepth[i]);
            _maxW = std::max(_maxW, pDepth[i]);
        }
    }

    // Tonemap depth values
    for (int i = 0; i < pW * pH; ++i)
    {
        int intensity = 0;
        if (pDepth[i] > 0)
        {
            intensity = (unsigned char)(223.0*(pDepth[i] - _minW) / (_maxW - _minW) + 32.0);
        }

        pImage[i * 3 + 0] = intensity;
        pImage[i * 3 + 1] = intensity;
        pImage[i * 3 + 2] = intensity;
    }
}

