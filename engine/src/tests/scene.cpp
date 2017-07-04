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

#define DEBUG_MASKED_OCCLUSION_CULLING
#define MAX_SEARCH_LENGHT 256

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::framework;
using namespace wolf::content_pipeline;

//forward declaration
static bool update_gui();
static void TonemapDepth(_In_ float* pDepth, _In_ unsigned char* pImage, _In_ const int& pW, _In_ const int& pH);

static bool sShowGui = true;
static char sSearch[MAX_SEARCH_LENGHT];
static std::once_flag sFirstTime;
static MaskedOcclusionCulling* sMOC;
static float* sMOCPerPixelZBuffer = nullptr; 
static uint8_t* sMOCTonemapDepthImage = nullptr;
static std::vector<model*> sModelsToBeRender;
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
    _config.debug_gpu = true;
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

    //load scene
    auto _scene = w_content_manager::load<w_cpipeline_scene>(content_path + L"models/A_120_Water-Treatment_v1_16_4.dae");// ");// ");
    if (_scene)
    {
        //just for converting
        //std::vector<w_cpipeline_scene> _scenes = { *_scene };
        //w_content_manager::save_wolf_scenes_to_file(_scenes, content_path + L"models/A_120_Water-Treatment_v1_16_4.wscene");
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
    if (sShowGui)
    {
        w_imgui::new_frame(windows_frame_time_in_sec.at(0), [&_gui_proceeded]()
        {
            _gui_proceeded = update_gui();
        });
    }

    //if gui proceeded or mouse captured by gui do not update view
    if (_gui_proceeded)
    {
        inputs_manager.reset();
        return;
    }
    if (ImGui::IsMouseHoveringAnyWindow()) return;

    //if camera updated, then update command buffers based on result of masked occlusion culling
    auto _camera_just_updated = this->_camera.update(pGameTime, this->_screen_size);
    auto _cam_pos = this->_camera.get_translate();

    //allow first time to build draw command buffer
    std::call_once(sFirstTime, [&]()
    {
        _camera_just_updated = true;
    });

    if (_camera_just_updated)
    {
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
            sModelsToBeRender.clear();

            std::for_each(this->_models.begin(), this->_models.end(), [&](_In_ model* pModel)
            {
                pModel->pre_update(this->_camera, &sMOC);
            });

            std::for_each(this->_models.begin(), this->_models.end(), [&](_In_ model* pModel)
            {
                if (pModel->post_update(sMOC))
                {
                    sModelsToBeRender.push_back(pModel);
                }
            });

            _build_draw_command_buffer(_gDevice);
        }

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
    std::for_each(sModelsToBeRender.begin(), sModelsToBeRender.end(), [&](_In_ model* pModel)
    {
        if (pModel->render(_gDevice, &this->_camera) == S_OK)
        {
            _wait_for_pre_render_semaphores.push_back(pModel->get_semaphore());
        }
    });

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
    _submit_info.pSignalSemaphores = sShowGui ? &gui_semaphore : &_output_window->vk_rendering_done_semaphore;

    // Submit to queue
    if (vkQueueSubmit(_gDevice->vk_graphics_queue.queue, 1, &_submit_info, this->_compute_fence))
    {
        _hr = S_FALSE;
        V(_hr, "submiting queu for drawing models", _trace, 3);
    }
    
    if (sShowGui)
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
    _hr = w_game::render(pGameTime);

    //post render which run compute shaders
    for (auto& _model : this->_models)
    {
        _model->post_render(_gDevice);
    }
  
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
    
#ifdef DEBUG_MASKED_OCCLUSION_CULLING
    free(sMOCPerPixelZBuffer);
    free(sMOCTonemapDepthImage);
#endif

    MaskedOcclusionCulling::Destroy(sMOC);

    w_pipeline::release_all_pipeline_caches(_gDevice);

    return w_game::release();
}

static bool update_gui()
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
    if (!ImGui::Begin("Left_Widget", 0, window_flags))
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
    if (!ImGui::Begin("Search_Widget", 0, window_flags))
    {
        // Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return _proceeded;
    }

    ImGui::SetWindowPos(ImVec2(38, 0));
    ImGui::PushItemWidth(299);
    if (ImGui::InputText("", sSearch, MAX_SEARCH_LENGHT))
    {
        _proceeded = true;
    }
    ImGui::PopItemWidth();
    ImGui::End();
#pragma endregion
    
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
    
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Debug");
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

