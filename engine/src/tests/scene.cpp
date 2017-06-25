#include "pch.h"
#include "scene.h"
#include <w_graphics/w_pipeline.h>
#include <w_content_manager.h>
#include <w_framework/w_quad.h>
#include <imgui/imgui.h>
#include <w_graphics/w_imgui.h>
#include "masked_occlusion_culling/MaskedOcclusionCulling.h"
#include <cameras/w_camera.h>
using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::framework;
using namespace wolf::content_pipeline;

//forward declaration
static void make_gui();

static bool sForceUpdate = true;
static MaskedOcclusionCulling* sMOC;
static std::vector<model*> sModelsToBeRender;
static uint32_t sVisible = 0;
static uint32_t sOcclued = 0;
static uint32_t sViewCulled = 0;
static UINT32 sFPS = 0;

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

    this->_compute_fence = nullptr;

    //we do not need fixed time step
    w_game::set_fixed_time_step(false);

    //enable/disable gpu debugging
    w_graphics_device_manager_configs _config;
    _config.debug_gpu = false;
    this->set_graphics_device_manager_configs(_config);
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
}

void scene::load()
{
    const std::string _trace = this->name + "::load";

    auto _gDevice = this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    _screen_size.x = static_cast<uint32_t>(_output_window->width);
    _screen_size.y = static_cast<uint32_t>(_output_window->height);

    w_game::load();
    
    //Fence for compute Command Buffer sync
    VkFenceCreateInfo _fence_create_info = {};
    _fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    _fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if(vkCreateFence(
        _gDevice->vk_device,
        &_fence_create_info,
        nullptr,
        &this->_compute_fence))
    {
        V(S_FALSE, "creating compute fence", _trace);
        release();
        w_game::exiting = true;
        return;
    }

    w_viewport _viewport;
    _viewport.y = 0;
    _viewport.width = static_cast<float>(_screen_size.x);
    _viewport.height = static_cast<float>(_screen_size.y);
    _viewport.minDepth = 0;
    _viewport.maxDepth = 1;

    w_viewport_scissor _viewport_scissor;
    _viewport_scissor.offset.x = 0;
    _viewport_scissor.offset.y = 0;
    _viewport_scissor.extent.width = _screen_size.x;
    _viewport_scissor.extent.height = _screen_size.y;

    auto _depth_attachment = w_graphics_device::w_render_pass_attachments::depth_attachment_description;
    _depth_attachment.format = _output_window->vk_depth_buffer_format;

    //create draw render pass
    auto _hr = this->_draw_render_pass.load(
        _gDevice,
        _viewport,
        _viewport_scissor,
        {
            w_graphics_device::w_render_pass_attachments::color_attachment_description,
            _depth_attachment,
        });

    if (_hr == S_FALSE)
    {
        logger.error("Error on creating render pass");
        release();
        w_game::exiting = true;
        return;
    }

    std::vector<VkAttachmentDescription> _attachments(2);
    // Color attachment
    _attachments[0].format = VkFormat::VK_FORMAT_B8G8R8A8_UNORM;
    _attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    // Don't clear the framebuffer (like the renderpass from the example does)
    _attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    _attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    _attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    _attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    _attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    _attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    //Depth attachment
    _attachments[1].format = _output_window->vk_depth_buffer_format;
    _attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    _attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    _attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    _attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    _attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    _attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    _attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.flags = 0;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = NULL;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pResolveAttachments = NULL;
    subpassDescription.pDepthStencilAttachment = nullptr;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = NULL;

    VkSubpassDependency subpassDependencies[2] = {};

    // Transition from final to initial (VK_SUBPASS_EXTERNAL refers to all commmands executed outside of the actual renderpass)
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Transition from initial to final
    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    std::vector<VkSubpassDescription> _subpass_description = { subpassDescription };
    std::vector<VkSubpassDependency> _subpassDependencies = { subpassDependencies[0], subpassDependencies[1] };

    _hr = this->_gui_render_pass.load(
        _gDevice,
        _viewport,
        _viewport_scissor,
        _attachments,
        &_subpass_description,
        &_subpassDependencies);

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

    //load scene
    auto _scene = w_content_manager::load<w_cpipeline_scene>(content_path + L"models/test.dae");// A_120_Water - Treatment_v1_16_4.DAE");
    if (_scene)
    {
        //just for converting
        //std::vector<w_cpipeline_scene> _scenes = { *_scene };
        //w_content_manager::save_wolf_scenes_to_file(_scenes, content_path + L"models/test.wscene");
        //_scenes.clear();

        //get all models
        std::vector<w_cpipeline_model*> _cmodels;
        _scene->get_all_models(_cmodels);
        
        int kkk = 0;
        for (auto& _iter : _cmodels)
        {
            if (!_iter) continue;

            auto _model = new model();
            if (_model->load(_gDevice, _iter, this->_draw_render_pass) == S_OK)
            {
                this->_models.push_back(_model);
            }
            else
            {
                SAFE_DELETE(_model);
                logger.error("Error on loading model " + _iter->get_name());
            }
        }

        _scene->get_first_camera(this->_camera);

        float _near_plan = 0.01f, far_plan = 5000;
        
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

        _scene->release();
    }

    VkSemaphoreCreateInfo _semaphore_create_info = {};
    _semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if(vkCreateSemaphore(_gDevice->vk_device,
        &_semaphore_create_info,
        nullptr,
        &gui_semaphore))
    {
        logger.error("Error on creating semaphore for gui command buffer");
        release();
        exit(1);
    }

    //now assign new command buffers
    _hr = this->_draw_command_buffers.load(_gDevice, _output_window->vk_swap_chain_image_views.size());
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating command buffers");
        release();
        exit(1);
    }

    //load image texture
    w_texture* _gui_images = new w_texture();
    _gui_images->load(_gDevice);
    _gui_images->initialize_texture_2D_from_file(content_path + L"textures/gui/icons.png", &_gui_images);
    w_imgui::load(_gDevice, _output_window->hwnd, _screen_size, _render_pass_handle, _gui_images);

    _hr = this->_gui_command_buffers.load(_gDevice, _output_window->vk_swap_chain_image_views.size());
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating gui command buffers");
        release();
        exit(1);
    }
}

HRESULT scene::_build_draw_command_buffer(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    //record clear screen command buffer for every swap chain image
    for (uint32_t i = 0; i < this->_draw_command_buffers.get_commands_size(); ++i)
    {
        this->_draw_command_buffers.begin(i);
        {
            auto _cmd = this->_draw_command_buffers.get_command_at(i);

            this->_draw_render_pass.begin(_cmd,
                this->_draw_frame_buffers.get_frame_buffer_at(i),
                w_color(43));
            {
                for (auto& _iter : sModelsToBeRender)
                {
                    _iter->indirect_draw(pGDevice, _cmd);
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

            this->_gui_render_pass.begin(_cmd,
                this->_gui_frame_buffers.get_frame_buffer_at(i));
            {
                //make sure render all gui before loading gui_render
                w_imgui::new_frame((float)pGameTime.get_elapsed_seconds(), []()
                {
                    make_gui();
                });
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
    
    auto _gDevice = this->graphics_devices[0];

    //if camera updated, then update command buffers based on result of masked occlusion culling
    auto _camera_just_updated = this->_camera.update(pGameTime, this->_screen_size);
    
    //order by distance to camera
    auto _cam_pos = this->_camera.get_translate();
    std::sort(sModelsToBeRender.begin(), sModelsToBeRender.end(), [&_cam_pos](_In_ model* a, _In_ model* b)
    {
        auto _a_distance = glm::distance(_cam_pos, a->get_position());
        auto _b_distance = glm::distance(_cam_pos, b->get_position());

        return _a_distance < _b_distance;
    });   
    
    if(sForceUpdate || _camera_just_updated)
    {
        sForceUpdate = false;

       // auto _view_projection = this->_camera.get_projection() * this->_camera.get_view();

        sVisible = 0;
        sOcclued = 0;
        sViewCulled = 0;

        sMOC->ClearBuffer();
        sModelsToBeRender.clear();
        
        for (auto _model : this->_models)
        {
            _model->pre_update(this->_camera, &sMOC);
        }
        
        for (auto _model : this->_models)
        {
            auto _old_value = sVisible;
            _model->post_update(sMOC, sVisible, sOcclued, sViewCulled);
            if (sVisible > _old_value)
            {
                sModelsToBeRender.push_back(_model);
            }
        }

        _build_draw_command_buffer(_gDevice);
    }

    w_game::update(pGameTime);
}

static bool show_gui = false;
HRESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
    const std::string _trace = this->name + "::render";

    HRESULT _hr = S_OK;
    
    auto _gDevice = this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);

    vkWaitForFences(_gDevice->vk_device, 1, &this->_compute_fence, VK_TRUE, UINT64_MAX);
    vkResetFences(_gDevice->vk_device, 1, &this->_compute_fence);

    std::vector<VkSemaphore> _wait_for_pre_render_semaphores =
    {
        _output_window->vk_swap_chain_image_is_available_semaphore
    };
    defer _(nullptr, [&](...)
    {
        _wait_for_pre_render_semaphores.clear();
    });

    //pre render which run compute shaders
    for (auto& _model : sModelsToBeRender)
    {
        if (_model->render(_gDevice, &this->_camera) == S_OK)
        {
            _wait_for_pre_render_semaphores.push_back(_model->get_semaphore());
        }
    }

    // Submit graphics command buffer
    auto _compute_cmd_buffer = this->_draw_command_buffers.get_command_at(_output_window->vk_swap_chain_image_index);

    VkSubmitInfo _submit_info = {};
    _submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    _submit_info.commandBufferCount = 1;
    _submit_info.pCommandBuffers = &_compute_cmd_buffer;

    // Wait on present and compute semaphores
    VkPipelineStageFlags _stage_flags[2] =
    {
        VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    };

    _submit_info.pWaitSemaphores = _wait_for_pre_render_semaphores.data();
    _submit_info.waitSemaphoreCount = static_cast<uint32_t>(_wait_for_pre_render_semaphores.size());
    _submit_info.pWaitDstStageMask = &_stage_flags[0];
    _submit_info.signalSemaphoreCount = 1;
    _submit_info.pSignalSemaphores = show_gui ? &gui_semaphore : &_output_window->vk_rendering_done_semaphore;

    // Submit to queue
    if (vkQueueSubmit(_gDevice->vk_graphics_queue.queue, 1, &_submit_info, this->_compute_fence))
    {
        _hr = S_FALSE;
        V(_hr, "submiting queu for drawing models", _trace, 3);
    }

    logger.write(std::to_string(sVisible));
    if (show_gui)
    {
        _build_gui_command_buffer(_gDevice, pGameTime);

        VkPipelineStageFlags _stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        auto _gui_cmd_buffer = this->_gui_command_buffers.get_command_at(_output_window->vk_swap_chain_image_index);

        VkSubmitInfo _gui_submit_info = {};
        _gui_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        _gui_submit_info.commandBufferCount = 1;
        _gui_submit_info.pCommandBuffers = &_gui_cmd_buffer;
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
    auto __hr = w_game::render(pGameTime);

    //post render which run compute shaders
    for (auto& _model : this->_models)
    {
        _model->post_render(_gDevice);
    }

  
    sFPS = pGameTime.get_frames_per_second();
    return S_OK;
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

    this->_draw_render_pass.release();
    this->_draw_frame_buffers.release();

    this->_gui_render_pass.release();
    this->_gui_frame_buffers.release();

    //release all models
    for (auto& _iter : this->_models)
    {
        SAFE_RELEASE(_iter);
    }

    w_imgui::release();
   
    w_pipeline::release_all_pipeline_caches(_gDevice);

    return w_game::release();
}

static void make_gui()
{
    ImGuiStyle& _style = ImGui::GetStyle();
    _style.WindowPadding = ImVec2(3, 2);
    _style.WindowRounding = 0;
    _style.GrabRounding = 4;
    _style.GrabMinSize = 20;
    _style.FramePadding = ImVec2(5, 5);
    _style.Colors[2].x = 0.9098039215686275f;
    _style.Colors[2].y = 0.4431372549019608f;
    _style.Colors[2].z = 0.3176470588235294f;
    _style.Colors[2].w = 1.0f;

    static bool no_titlebar = true;
    static bool no_border = true;
    static bool no_resize = true;
    static bool no_move = true;
    static bool no_scrollbar = true;
    static bool no_collapse = true;
    static bool no_menu = true;

    ImGuiWindowFlags window_flags = 0;
    if (no_titlebar)  window_flags |= ImGuiWindowFlags_NoTitleBar;
    if (!no_border)   window_flags |= ImGuiWindowFlags_ShowBorders;
    if (no_resize)    window_flags |= ImGuiWindowFlags_NoResize;
    if (no_move)      window_flags |= ImGuiWindowFlags_NoMove;
    if (no_scrollbar) window_flags |= ImGuiWindowFlags_NoScrollbar;
    if (no_collapse)  window_flags |= ImGuiWindowFlags_NoCollapse;
    if (!no_menu)     window_flags |= ImGuiWindowFlags_MenuBar;
    if (!ImGui::Begin("ImGui Demo", 0, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return;
    }

    ImGui::SetWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(320, 320), ImGuiSetCond_FirstUseEver);
    for (int i = 0; i < 6; i++)
    {
        ImTextureID tex_id = (void*)("#image");
        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_ImageActive, ImColor(0.0f, 0.0f, 255.0f, 255.0f));
        ImGui::PushStyleColor(ImGuiCol_ImageHovered, ImColor(0.0f, 0.0f, 255.0f, 155.0f));
        if (ImGui::ImageButton(tex_id, ImVec2(32, 32), ImVec2(i * 0.1, 0.0), ImVec2(i * 0.1 + 0.1f, 0.1), 0, ImColor(232, 113, 83, 255)))
        {
         
        }
        ImGui::PopStyleColor(2);
        ImGui::PopID();
        ImGui::Spacing();
        ImGui::Spacing();
    }

    std::string text = 
        "FPS: " + std::to_string(sFPS) + "\r\n" +
        "Visible: " + std::to_string(sVisible) + "\r\n" +
        "Occlued: " + std::to_string(sOcclued) + "\r\n" + 
        "ViewCulled: " + std::to_string(sViewCulled);
    ImGui::Text(text.c_str());
    
    //ImGui::ShowTestWindow();

    ImGui::End();
}
