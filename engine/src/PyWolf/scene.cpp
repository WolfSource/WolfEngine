#include "pch.h"
#include "scene.h"
#include <w_graphics/w_pipeline.h>
#include <w_content_manager.h>
#include <imgui/imgui.h>
#include <w_graphics/w_imgui.h>
#include "masked_occlusion_culling/CullingThreadpool.h"
#include <cameras/w_camera.h>
#include <mutex>
#include <w_task.h>
#include <w_thread.h>
#include <w_timer.h>
#include <glm/gtx/quaternion.hpp>

//#define DEBUG_MASKED_OCCLUSION_CULLING
#define MAX_SEARCH_LENGHT 256

using namespace wolf;
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

static MaskedOcclusionCulling* sMOC = nullptr;
static CullingThreadpool* sMOCThreadPool = nullptr;

static uint32_t sFPS = 0;
static uint32_t sRenderingThreads = 0;
static uint32_t sTotalModels = 0;
static uint32_t sVisibleModels = 0;
static uint32_t sVisibleSubModels = 0;
static std::string SVersion;

#ifdef DEBUG_MASKED_OCCLUSION_CULLING
static bool                     sShowDebugMaskedOcclusionCulling = false;
static float*                   sMOCPerPixelZBuffer = nullptr;
static uint8_t*                 sMOCTonemapDepthImage = nullptr;
#endif // DEBUG_MASKED_OCCLUSION_CULLING

scene::scene(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName):
	w_game(pRunningDirectory, pAppName)
{
    this->_show_gui = true;
    this->_draw_fence = 0;

    //we do not need fixed time step
    w_game::set_fixed_time_step(false);

    //enable/disable gpu debugging
    w_graphics_device_manager_configs _config;
    _config.debug_gpu = false;
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
    case MaskedOcclusionCulling::AVX512:
        logger.write("Using AVX512 implementation of MaskedOcclusionCulling");
        break;
    }

    auto _thread_pool_size = 2;
    sMOCThreadPool = new CullingThreadpool(4, 2, 2);
    sMOCThreadPool->SetBuffer(sMOC);
    sMOCThreadPool->SuspendThreads();
    
#ifdef __WIN32
    sprintf_s(sSearch, "Search");
#else
    sprintf(sSearch, "Search");
#endif

}

void scene::load()
{
    const std::string _trace = this->name + "::load";

    auto _gDevice = this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    _screen_size.x = _output_window->width;
    _screen_size.y = _output_window->height;

    w_game::load();

#ifdef DEBUG_MASKED_OCCLUSION_CULLING
    {
        size_t _size = sMediaPlayerWidth * sMediaPlayerHeight;
        sMOCPerPixelZBuffer = (float*)malloc(_size * sizeof(float));
        sMOCTonemapDepthImage = (uint8_t*)malloc(_size * 4 * sizeof(uint8_t));
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


    auto _thread_pool_size = w_thread::get_number_of_hardware_thread_contexts();
    this->_render_thread_pool.resize(_thread_pool_size);
    logger.write("Size of thread pool is " + std::to_string(_thread_pool_size));
    sRenderingThreads = _thread_pool_size;

    for (size_t i = 0; i < _thread_pool_size; ++i)
    {
        this->_render_thread_pool[i] = new (std::nothrow) render_thread_context();
        if (!this->_render_thread_pool[i])
        {
            logger.error("Could not allocate memory for thread context: " + std::to_string(i));
            continue;
        }
        _hr = this->_render_thread_pool[i]->secondary_command_buffers.load(
            _gDevice,
            _swap_chain_image_size,
            VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY,
            true,
            &_gDevice->vk_present_queue);
        if (_hr == S_FALSE)
        {
            logger.error("Error on creating draw command buffer for thread " + std::to_string(i));
            release();
            exit(1);
        }
    }

    //load icon's texture
    w_texture* _icon_images = new w_texture();
    _icon_images->load(_gDevice);
    _icon_images->initialize_texture_2D_from_file(L"textures/gui/icons.png");

    auto _font_path = wolf::system::convert::wstring_to_string(content_path) + "fonts/arial.ttf";
    w_imgui::load(_gDevice, _output_window->hwnd, _screen_size, _render_pass_handle, _icon_images, nullptr, _font_path.c_str());

    _hr = this->_gui_command_buffers.load(_gDevice, _output_window->vk_swap_chain_image_views.size());
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating gui command buffers");
        release();
        exit(1);
    }

    float _near_plan = 0.1f, far_plan = 5000;
    this->_camera.set_near_plan(_near_plan);
    sMOCThreadPool->SetNearClipPlane(_near_plan);

    this->_camera.set_aspect_ratio((float)_screen_size.x / (float)_screen_size.y);
    sMOCThreadPool->SetResolution(_screen_size.x, _screen_size.y);

    this->_camera.update_view();
    this->_camera.update_projection();
}

HRESULT scene::load_scene(_In_z_ const std::wstring& pScenePath)
{
    HRESULT _hr = S_OK;
    auto _gDevice = this->graphics_devices[0];

    //load sequential, because of amd tootle optimizing
    auto _scene = w_content_manager::load<w_cpipeline_scene>(pScenePath);
    if (_scene)
    {
        //get all models
        std::vector<w_cpipeline_model*> _cmodels;
        _scene->get_all_models(_cmodels);

		if (_cmodels.size())
		{
			for (auto& _iter : _cmodels)
			{
				if (!_iter) continue;

				auto _model = new model();
				if (_model->pre_load(_gDevice, _iter) == S_OK)
				{
					if (_model->post_load(this->_draw_render_pass) == S_OK)
					{
						this->models.push_back(_model);
					}
					else
					{
						logger.error("Error on post-loading model " + _iter->get_name());
						SAFE_DELETE(_model);
						_hr = S_FALSE;
					}
				}
				else
				{
					logger.error("Error on pre-loading model " + _iter->get_name());
					SAFE_DELETE(_model);
					_hr = S_FALSE;
				}
			}
		}

		_scene->get_first_camera(this->_camera);
		this->_camera.update_view();

        _scene->release();
    }
    else
    {
        logger.write(L"Scene on following path not exists " + pScenePath);
        _hr = S_FALSE;
    }
    return _hr;
}

HRESULT scene::_build_draw_command_buffer(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    auto _models_count = this->_models_to_be_render.size();
    auto _threads_count = this->_render_thread_pool.size();

    //reset all batch size
    std::for_each(this->_render_thread_pool.begin(), this->_render_thread_pool.end(),
        [](_In_ render_thread_context* pThread)
    {
        pThread->batch_size = 0;
    });

    if (_models_count < _threads_count)
    {
        for (size_t i = 0; i < _models_count; i++)
        {
            this->_render_thread_pool[i]->batch_size++;
        }
    }
    else
    {
        auto _size = this->_models_to_be_render.size() / this->_render_thread_pool.size();
        for (auto& _t : this->_render_thread_pool)
        {
            _t->batch_size = _size;
        }
        _size = this->_models_to_be_render.size() % this->_render_thread_pool.size();
        for (size_t i = 0; i < _size; i++)
        {
            this->_render_thread_pool[i]->batch_size++;
        }
    }

    for (uint32_t i = 0; i < this->_draw_command_buffers.get_commands_size(); ++i)
    {
        this->_draw_command_buffers.begin(i);
        {
            auto _render_pass_handle = this->_draw_render_pass.get_handle();
            auto _frame_buffer_handle = this->_draw_frame_buffers.get_frame_buffer_at(i);

            VkCommandBufferInheritanceInfo _inheritance_info = {};
            _inheritance_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
            _inheritance_info.renderPass = _render_pass_handle;
            _inheritance_info.framebuffer = _frame_buffer_handle;

            VkCommandBufferBeginInfo _sec_cmd_buffer_begin_info{};
            _sec_cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            _sec_cmd_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
            _sec_cmd_buffer_begin_info.pInheritanceInfo = &_inheritance_info;


            auto _primary_draw_cmd = this->_draw_command_buffers.get_command_at(i);
            this->_draw_render_pass.begin(_primary_draw_cmd,
                _frame_buffer_handle,
                w_color(43, 43, 43, 255),
                1.0f,
                0.0f,
                VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
            {
                std::vector<VkCommandBuffer> _sec_cmd_buffers;
                //add job for each thread
                size_t _start_index = 0;
                for (auto& _thread_context : this->_render_thread_pool)
                {
                    //if thread does not have any models then skip it
                    if (_thread_context->batch_size == 0) continue;

                    auto _thread_cmd = _thread_context->secondary_command_buffers.get_command_at(i);
                    _sec_cmd_buffers.push_back(_thread_cmd);

                    _thread_context->thread.add_job([&, _thread_cmd, _sec_cmd_buffer_begin_info, _start_index]()
                    {
                        vkBeginCommandBuffer(_thread_cmd, &_sec_cmd_buffer_begin_info);
                        {
                            vkCmdSetViewport(_thread_cmd, 0, 1, &this->_viewport);
                            vkCmdSetScissor(_thread_cmd, 0, 1, &this->_viewport_scissor);

                            //draw models
                            auto _begin_iter = this->_models_to_be_render.begin() + _start_index;
                            auto _end_iter = this->_models_to_be_render.begin() + _start_index + _thread_context->batch_size;
                            std::for_each(
                                _begin_iter,
                                _end_iter,
                                [&](_In_ model* pModel)
                            {
                                pModel->indirect_draw(_thread_cmd);
                            });
                        }
                        vkEndCommandBuffer(_thread_cmd);
                    });
                    _start_index += _thread_context->batch_size;
                }

                //wait for all threads
                for (auto& _thread : this->_render_thread_pool)
                {
                    _thread->thread.wait();
                }

                //Execute secondary commands buffer to primary command
                if (_sec_cmd_buffers.size())
                {
                    vkCmdExecuteCommands(_primary_draw_cmd, _sec_cmd_buffers.size(), _sec_cmd_buffers.data());
                    _sec_cmd_buffers.clear();
                }
            }
            this->_draw_render_pass.end(_primary_draw_cmd);
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

bool _record_draw_command_buffer = true;
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
        w_imgui::new_frame(pGameTime.get_elapsed_seconds(), [&_gui_proceeded, this]()
        {
            _gui_proceeded = _update_gui();
        });
    }


    sTotalModels = this->models.size();

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

       /* _camera_just_updated = this->_camera.update(pGameTime, this->_screen_size);*/
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

        auto _cam_pos = this->_camera.get_translate();
        sVisibleSubModels = 0;
        this->_models_to_be_render.clear();

        //clear buffers and wake up masked occlusion threads
        sMOCThreadPool->ClearBuffer();
        sMOCThreadPool->WakeThreads();

        bool _need_flush_moc = false;
        sTotalModels += this->models.size();

        std::for_each(this->models.begin(), this->models.end(), [&](_In_ model* pModel)
        {
            if (pModel->pre_update(this->_camera, sMOCThreadPool))
            {
                _need_flush_moc = true;
            }
        });

        if (!_need_flush_moc)
        {
            sMOCThreadPool->SuspendThreads();
            return;
        }

        //apply flush for all masked occlusion threads and force them to finish the jobs
        sMOCThreadPool->Flush();

        //post update stage
        std::for_each(this->models.begin(), this->models.end(), [&](_In_ model* pModel)
        {
            if (pModel->post_update(sMOCThreadPool, sVisibleSubModels))
            {
                this->_models_to_be_render.push_back(pModel);
            }
        });

        sMOCThreadPool->SuspendThreads();

        _record_draw_command_buffer = true;

#ifdef DEBUG_MASKED_OCCLUSION_CULLING

        if (sShowDebugMaskedOcclusionCulling)
        {
            //Compute a per pixel depth buffer from the hierarchical depth buffer, used for visualization.
            sMOCThreadPool->ComputePixelDepthBuffer(sMOCPerPixelZBuffer);
            //Tonemap the depth image
            TonemapDepth(sMOCPerPixelZBuffer, sMOCTonemapDepthImage, sMediaPlayerWidth, sMediaPlayerHeight);

            if (sStagingMediaData)
            {
                tbb::parallel_for(tbb::blocked_range<size_t>(0, sMediaPlayerWidth * sMediaPlayerHeight * 4), [&](const tbb::blocked_range<size_t>& pRange)
                {
                    for (size_t i = pRange.begin(); i < pRange.end(); ++i)
                    {
                        sStagingMediaData[i] = sMOCTonemapDepthImage[i];
                    }
                });
                sStagingMediaTexture->copy_data_to_texture_2D(sStagingMediaData);
            }
        }
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

    vkResetFences(_gDevice->vk_device, 1, &this->_draw_fence);

    auto _image_is_avaiable = _output_window->vk_swap_chain_image_is_available_semaphore.get();
    std::vector<VkSemaphore> _wait_semaphors = { *_image_is_avaiable };

    if (_record_draw_command_buffer)
    {
        //submit compute shader for all visible models
        std::for_each(this->_models_to_be_render.begin(), this->_models_to_be_render.end(),
            [&](_In_ model* pModel)
        {
            if (pModel->submit_compute_shader(&this->_camera) == S_OK)
            {
                _wait_semaphors.push_back(pModel->get_compute_semaphore());
            }
        });
        _build_draw_command_buffer(_gDevice);
        _record_draw_command_buffer = false;
    }

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
    _submit_info.waitSemaphoreCount = _wait_semaphors.size();
    _submit_info.pWaitSemaphores = _wait_semaphors.data();
    _submit_info.signalSemaphoreCount = 1;
    _submit_info.pSignalSemaphores = this->_show_gui ? &gui_semaphore : _output_window->vk_rendering_done_semaphore.get(); //signal to gui if gui is avaiable, otherwise end the render 

    // Submit to queue
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
        _gui_submit_info.pSignalSemaphores = _output_window->vk_rendering_done_semaphore.get();

        // Submit to queue
        if (vkQueueSubmit(_gDevice->vk_graphics_queue.queue, 1, &_gui_submit_info, 0))
        {
            _hr = S_FALSE;
            V(_hr, "submiting queu for drawing gui", _trace, 3);
        }
    }
    _hr = w_game::render(pGameTime);

    sFPS = pGameTime.get_frames_per_second();
    sVisibleModels = this->_models_to_be_render.size();

    _wait_semaphors.clear();

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

    //release all rendering threads
    for (auto _thread_context : this->_render_thread_pool)
    {
        SAFE_RELEASE(_thread_context);
    }
    this->_render_thread_pool.clear();

    this->_models_to_be_render.clear();
    //release all models
    for (auto _model : this->models)
    {
        SAFE_RELEASE(_model);
    }
    this->models.clear();

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
    
#ifdef DEBUG_MASKED_OCCLUSION_CULLING
    free(sMOCPerPixelZBuffer);
    free(sMOCTonemapDepthImage);
#endif

    sMOCThreadPool->SuspendThreads();
    MaskedOcclusionCulling::Destroy(sMOC);
    SAFE_DELETE(sMOCThreadPool);

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


    window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    //window_flags |= ImGuiWindowFlags_ShowBorders;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    //window_flags |= ImGuiWindowFlags_MenuBar;

//#pragma region Left Buttons
//    if (!ImGui::Begin("Buttons", 0, window_flags))
//    {
//        // Early out if the window is collapsed, as an optimization.
//        ImGui::End();
//        return _proceeded;
//    }
//
//    ImGui::SetWindowPos(ImVec2(0, 0));
//
//    for (int i = 0; i < 1; i++)
//    {
//        ImTextureID tex_id = (void*)("#i");//this texture used for icons
//        ImGui::PushID(i);
//        ImGui::PushStyleColor(ImGuiCol_ImageActive, ImColor(0.0f, 0.0f, 255.0f, 255.0f));
//        ImGui::PushStyleColor(ImGuiCol_ImageHovered, ImColor(0.0f, 0.0f, 255.0f, 155.0f));
//        if (ImGui::ImageButton(tex_id, ImVec2(32, 32), ImVec2(i * 0.1, 0.0), ImVec2(i * 0.1 + 0.1f, 0.1), 0, ImColor(232, 113, 83, 255)))
//        {
//            //show debugger
//            if (i == 0)
//            {
//#ifdef DEBUG_MASKED_OCCLUSION_CULLING
//                sShowDebugMaskedOcclusionCulling = !sShowDebugMaskedOcclusionCulling;
//#endif
//            }
//            _proceeded = true;
//        }
//        ImGui::PopStyleColor(2);
//        ImGui::PopID();
//        ImGui::Spacing();
//        ImGui::Spacing();
//    }
//    ImGui::End();
//
//#pragma endregion

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

    ImGui::SetWindowPos(ImVec2(0, 360));
    ImGui::SetNextWindowSize(ImVec2(200, 120), ImGuiSetCond_Always);
    ImGui::Begin(("Wolf.Engine " + SVersion).c_str());
    ImGui::Text("FPS:%d\r\nThread pool rendering size: %d\r\nTotal Ref Models: %d\r\nTotal Visible Models: %d\r\nTotal Visible Ref Models: %d",
        sFPS, sRenderingThreads, sTotalModels, sVisibleSubModels, sVisibleModels);

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

        auto _index = i * 4;
        pImage[_index + 0] = intensity;
        pImage[_index + 1] = intensity;
        pImage[_index + 2] = intensity;
        pImage[_index + 3] = 255;
    }
}

#pragma region Setters

void scene::set_camera_position(float X, float Y, float Z)
{
	//change name to position
	this->_camera.set_translate(X, Y, Z);
	this->_camera.update_view();
	sForceUpdateCamera = true;
}

void scene::set_camera_lookat(float X, float Y, float Z)
{
	//change name to lookat
	this->_camera.set_interest(X, Y, Z);
	this->_camera.update_view();
	sForceUpdateCamera = true;
}

#pragma endregion