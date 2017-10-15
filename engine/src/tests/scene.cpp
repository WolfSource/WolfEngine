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

#define DEBUG_MASKED_OCCLUSION_CULLING
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

static MaskedOcclusionCulling* sMOC = nullptr;
static CullingThreadpool* sMOCThreadPool = nullptr;

static uint32_t sFPS = 0;
static uint32_t sRenderingThreads = 0;
static uint32_t sTotalModels = 0;
static uint32_t sVisibleModels = 0;
static uint32_t sVisibleSubModels = 0;
static std::string SVersion;

static uint32_t sMediaPlayerWidth = 1280, sMediaPlayerHeight = 720;
static w_texture* sStagingMediaTexture = nullptr;
static uint8_t* sStagingMediaData = nullptr;

//first loading
static tbb::atomic<bool>        sLoading = true;
static bool                     sShowingLiveCamera = false;

#ifdef DEBUG_MASKED_OCCLUSION_CULLING
static bool                     sShowDebugMaskedOcclusionCulling = false;
static float*                   sMOCPerPixelZBuffer = nullptr;
static uint8_t*                 sMOCTonemapDepthImage = nullptr;
#endif // DEBUG_MASKED_OCCLUSION_CULLING

scene::scene(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName):
	w_game(pRunningDirectory, pAppName)
{
     auto _running_dir = pRunningDirectory;
    
#if defined(__WIN32) || defined(__UWP)
    content_path = _running_dir + L"../../../../content/";
#elif defined(__APPLE__)
    content_path = _running_dir + L"/../../../../../content/";
#elif defined(__linux)
    error
#elif defined(__ANDROID)
    error
#endif

    this->_show_gui = true;
    this->_draw_fence = 0;
    this->_media_width = 1280;
    this->_media_height = 720;
    this->_media_fps = 29.0f;
    this->_current_frame = 0;
    this->_video_frame_address = 0;
    this->_video_time.set_fixed_time_step(true);
    this->_total_loaded_areas = 0;
    this->_total_areas = 0;

    //we do not need fixed time step
    w_game::set_fixed_time_step(false);

    //enable/disable gpu debugging
    w_graphics_device_manager_configs _config;
    _config.debug_gpu = false;
    this->set_graphics_device_manager_configs(_config);

    SVersion =
        std::to_string(WOLF_MAJOR_VERSION) + "." + std::to_string(WOLF_MINOR_VERSION) +
        "." + std::to_string(WOLF_PATCH_VERSION) + "." + std::to_string(WOLF_DEBUG_VERSION);

    w_ffmpeg::initialize_MF();
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

    auto _thread_pool_size = w_thread::get_number_of_hardware_thread_contexts();
    auto _bin_size = _thread_pool_size / 2;
    sMOCThreadPool = new CullingThreadpool(_thread_pool_size, _bin_size, _bin_size);
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
    _screen_size.x = static_cast<uint32_t>(_output_window->width);
    _screen_size.y = static_cast<uint32_t>(_output_window->height);

    w_game::load();
    
    if (_open_media(content_path + L"media/falcon.avi", 0) == S_OK)
    {
        //Run video buffering task
        wolf::system::w_task::execute_async_ppl([this]()
        {
            _video_buffering_thread();
        });
    }
    else
    {
        logger.warning("could not load intro \"media/falcon.avi\"");
    }

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

    //create staging texture
    if (sStagingMediaTexture == nullptr)
    {
        sStagingMediaTexture = new w_texture();
        sStagingMediaTexture->load(_gDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        sStagingMediaTexture->initialize_texture_from_memory_all_channels_same(0, sMediaPlayerWidth, sMediaPlayerHeight);

        if (sStagingMediaData)
        {
            free(sStagingMediaData);
        }

        auto _length = sMediaPlayerWidth * sMediaPlayerHeight * 4;
        sStagingMediaData = (uint8_t*)malloc(_length * sizeof(uint8_t));
        if (!sStagingMediaData)
        {
            logger.error("could not allocate memory for media player");
        }
    }

    auto _font_path = wolf::system::convert::wstring_to_string(content_path) + "fonts/arial.ttf";
    w_imgui::load(_gDevice, _output_window->hwnd, _screen_size, _render_pass_handle, _icon_images, &sStagingMediaTexture, _font_path.c_str());

    _hr = this->_gui_command_buffers.load(_gDevice, _output_window->vk_swap_chain_image_views.size());
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating gui command buffers");
        release();
        exit(1);
    }

    _load_areas();
}

void scene::_load_area(_In_z_ const std::wstring& pArea, _In_ const bool pLoadCollada)
{
    auto _gDevice = this->graphics_devices[0];
    auto _full_path = content_path + L"models/" + pArea + L"/";

    area _area;
    _area.name = pArea;
    
    //load models in seperated threads
    if (pLoadCollada)
    {
        //load sequential, because of amd tootle optimizing
#pragma region Load Outer

        auto _scene_path = _full_path + (pLoadCollada ? L"outer.dae" : L"outer.wscene");
        auto _scene = w_content_manager::load<w_cpipeline_scene>(_scene_path);
        if (_scene)
        {
            if (pLoadCollada)
            {
                //convert to wscene
                std::vector<w_cpipeline_scene> _scenes = { *_scene };
                w_content_manager::save_wolf_scenes_to_file(_scenes,
                    wolf::system::io::get_parent_directoryW(_scene_path) + L"/" +
                    wolf::system::io::get_base_file_nameW(_scene_path) + L".wscene");
                _scenes.clear();
            }

            //get all models
            std::vector<w_cpipeline_model*> _cmodels;
            _scene->get_all_models(_cmodels);

            for (auto& _iter : _cmodels)
            {
                if (!_iter) continue;

                auto _model = new model();
                auto _hr = _model->pre_load(_gDevice, _iter);
                if (_hr == S_OK)
                {
                    _area.outer_models.push_back(_model);
                }
                else
                {
                    SAFE_DELETE(_model);
                    logger.error("Error on loading model " + _iter->get_name());
                }
            }
            _scene->release();
        }
        else
        {
            logger.write(L"Scene on following path not exists " + _scene_path);
        }
#pragma endregion

#pragma region Load Middle

        _scene_path = _full_path + +(pLoadCollada ? L"middle.dae" : L"middle.wscene");
        _scene = w_content_manager::load<w_cpipeline_scene>(_scene_path);
        if (_scene)
        {
            if (pLoadCollada)
            {
                //convert to wscene
                std::vector<w_cpipeline_scene> _scenes = { *_scene };
                w_content_manager::save_wolf_scenes_to_file(_scenes,
                    wolf::system::io::get_parent_directoryW(_scene_path) + L"/" +
                    wolf::system::io::get_base_file_nameW(_scene_path) + L".wscene");
                _scenes.clear();
            }

            //get all models
            std::vector<w_cpipeline_model*> _cmodels;
            _scene->get_all_models(_cmodels);

            for (auto& _iter : _cmodels)
            {
                if (!_iter) continue;

                auto _model = new model();
                auto _hr = _model->pre_load(_gDevice, _iter);
                if (_hr == S_OK)
                {
                    _area.middle_models.push_back(_model);
                }
                else
                {
                    SAFE_DELETE(_model);
                    logger.error("Error on loading model " + _iter->get_name());
                }
            }
            _scene->release();
        }
        else
        {
            logger.write(L"Scene on following path not exists " + _scene_path);
        }
#pragma endregion

#pragma region Load Inner
        _scene_path = _full_path + +(pLoadCollada ? L"inner.dae" : L"inner.wscene");
        _scene = w_content_manager::load<w_cpipeline_scene>(_scene_path);
        if (_scene)
        {
            if (pLoadCollada)
            {
                //convert to wscene
                std::vector<w_cpipeline_scene> _scenes = { *_scene };
                w_content_manager::save_wolf_scenes_to_file(_scenes,
                    wolf::system::io::get_parent_directoryW(_scene_path) + L"/" +
                    wolf::system::io::get_base_file_nameW(_scene_path) + L".wscene");
                _scenes.clear();
            }

            //get all models
            std::vector<w_cpipeline_model*> _cmodels;
            _scene->get_all_models(_cmodels);

            for (auto& _iter : _cmodels)
            {
                if (!_iter) continue;

                auto _model = new model();
                auto _hr = _model->pre_load(_gDevice, _iter);
                if (_hr == S_OK)
                {
                    _area.inner_models.push_back(_model);
                }
                else
                {
                    SAFE_DELETE(_model);
                    logger.error("Error on loading model " + _iter->get_name());
                }
            }
            _scene->release();
        }
        else
        {
            logger.write(L"Scene on following path not exists " + _scene_path);
        }
#pragma endregion
    }
    else
    {
        //load in thread
        tbb::parallel_invoke([&]()
        {
#pragma region Load Outer

            auto _scene_path = _full_path + (pLoadCollada ? L"outer.dae" : L"outer.wscene");
            auto _scene = w_content_manager::load<w_cpipeline_scene>(_scene_path);
            if (_scene)
            {
                if (pLoadCollada)
                {
                    //convert to wscene
                    std::vector<w_cpipeline_scene> _scenes = { *_scene };
                    w_content_manager::save_wolf_scenes_to_file(_scenes,
                        wolf::system::io::get_parent_directoryW(_scene_path) + L"/" +
                        wolf::system::io::get_base_file_nameW(_scene_path) + L".wscene");
                    _scenes.clear();
                }

                //get all models
                std::vector<w_cpipeline_model*> _cmodels;
                _scene->get_all_models(_cmodels);

                for (auto& _iter : _cmodels)
                {
                    if (!_iter) continue;

                    auto _model = new model();
                    auto _hr = _model->pre_load(_gDevice, _iter);
                    if (_hr == S_OK)
                    {
                        _area.outer_models.push_back(_model);
                    }
                    else
                    {
                        SAFE_DELETE(_model);
                        logger.error("Error on loading model " + _iter->get_name());
                    }
                }
                _scene->release();
            }
            else
            {
                logger.write(L"Scene on following path not exists " + _scene_path);
            }
#pragma endregion
        },
            [&]()
        {
#pragma region Load Middle

            auto _scene_path = _full_path + +(pLoadCollada ? L"middle.dae" : L"middle.wscene");
            auto _scene = w_content_manager::load<w_cpipeline_scene>(_scene_path);
            if (_scene)
            {
                if (pLoadCollada)
                {
                    //convert to wscene
                    std::vector<w_cpipeline_scene> _scenes = { *_scene };
                    w_content_manager::save_wolf_scenes_to_file(_scenes,
                        wolf::system::io::get_parent_directoryW(_scene_path) + L"/" +
                        wolf::system::io::get_base_file_nameW(_scene_path) + L".wscene");
                    _scenes.clear();
                }

                //get all models
                std::vector<w_cpipeline_model*> _cmodels;
                _scene->get_all_models(_cmodels);

                for (auto& _iter : _cmodels)
                {
                    if (!_iter) continue;

                    auto _model = new model();
                    auto _hr = _model->pre_load(_gDevice, _iter);
                    if (_hr == S_OK)
                    {
                        _area.middle_models.push_back(_model);
                    }
                    else
                    {
                        SAFE_DELETE(_model);
                        logger.error("Error on loading model " + _iter->get_name());
                    }
                }
                _scene->release();
            }
            else
            {
                logger.write(L"Scene on following path not exists " + _scene_path);
            }
#pragma endregion
        },
            [&]()
        {
#pragma region Load Inner
            auto _scene_path = _full_path + +(pLoadCollada ? L"inner.dae" : L"inner.wscene");
            auto _scene = w_content_manager::load<w_cpipeline_scene>(_scene_path);
            if (_scene)
            {
                if (pLoadCollada)
                {
                    //convert to wscene
                    std::vector<w_cpipeline_scene> _scenes = { *_scene };
                    w_content_manager::save_wolf_scenes_to_file(_scenes,
                        wolf::system::io::get_parent_directoryW(_scene_path) + L"/" +
                        wolf::system::io::get_base_file_nameW(_scene_path) + L".wscene");
                    _scenes.clear();
                }

                //get all models
                std::vector<w_cpipeline_model*> _cmodels;
                _scene->get_all_models(_cmodels);

                for (auto& _iter : _cmodels)
                {
                    if (!_iter) continue;

                    auto _model = new model();
                    auto _hr = _model->pre_load(_gDevice, _iter);
                    if (_hr == S_OK)
                    {
                        _area.inner_models.push_back(_model);
                    }
                    else
                    {
                        SAFE_DELETE(_model);
                        logger.error("Error on loading model " + _iter->get_name());
                    }
                }
                _scene->release();
            }
            else
            {
                logger.write(L"Scene on following path not exists " + _scene_path);
            }
#pragma endregion
        });
    }

    if (_area.outer_models.size() || _area.middle_models.size() || _area.inner_models.size())
    {
#pragma region Load Boundaries
        auto _scene_path = _full_path + +(pLoadCollada ? L"boundaries.dae" : L"boundaries.wscene");
        auto _scene = w_content_manager::load<w_cpipeline_scene>(_scene_path);
        if (_scene)
        {
            if (pLoadCollada)
            {
                //convert to wscene
                std::vector<w_cpipeline_scene> _scenes = { *_scene };
                w_content_manager::save_wolf_scenes_to_file(_scenes,
                    wolf::system::io::get_parent_directoryW(_scene_path) + L"/" +
                    wolf::system::io::get_base_file_nameW(_scene_path) + L".wscene");
                _scenes.clear();
            }

            _scene->get_boundaries(_area.boundaries);
            _scene->release();
        }
        else
        {
            logger.write(L"Scene on following path not exists " + _scene_path);
        }
#pragma endregion

        this->_areas.push_back(_area);
        sLoading = false;
    }
}

HRESULT scene::_load_areas()
{
    concurrency::create_task([this]()->void
    {
        const std::vector<std::wstring> _areas =
        {
            L"120 - water treatment",
            L"151 - substation",
            L"161 - air compressor",
            L"171-173 - office buildings",
            L"230 - proportioning",
            L"310 - pelletizing workshop",
            L"320 - tranfer station",
            L"410 - hot air return system",
            L"420 - rotary annular kiln",
            L"430 - rotary annular cooler",
            L"440 - chain grate workshop",
            L"450 - main ESP"
        };
        this->_total_areas = _areas.size();

        bool _load_collada = true;
        if (_load_collada)
        {
            for (const auto& pArea : _areas)
            {
                _load_area(pArea, _load_collada);
            }
        }
        else
        {
            tbb::parallel_for_each(_areas.begin(), _areas.end(), [&](_In_ const std::wstring& pArea)
            {
                _load_area(pArea, _load_collada);
            });
        }
    });

    //load camera
    auto _scene = w_content_manager::load<w_cpipeline_scene>(content_path + L"models/camera.dae");
    if (_scene)
    {
        _scene->get_first_camera(this->_camera);

        float _near_plan = 0.1f, far_plan = 5000;

        this->_camera.set_near_plan(_near_plan);
        sMOCThreadPool->SetNearClipPlane(_near_plan);
        
        this->_camera.set_far_plan(far_plan);

        auto _screen_width = (float)_screen_size.x;
        auto _screen_height = (float)_screen_size.y;

        this->_camera.set_aspect_ratio(_screen_width / _screen_height);
        sMOCThreadPool->SetResolution(_screen_width, _screen_height);

        this->_camera.update_view();
        this->_camera.update_projection();
        this->_camera.update_frustum();
    }

    return S_OK;
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

    //check for loading areas
    if (this->_total_loaded_areas != this->_total_areas)
    {
        for (auto& _area : this->_areas)
        {
            if (!_area.is_loaded)
            {
                //create mesh in each update frame  

                //create outer meshes
                if (_area.outer_loaded_index < _area.outer_models.size())
                {
                    auto _model = _area.outer_models[_area.outer_loaded_index];
                    if (_model->post_load(this->_draw_render_pass) == S_FALSE)
                    {
                        logger.error("error on post loading " + std::string(_area.outer_models[_area.outer_loaded_index]->get_full_name()));
                    }
                    _area.outer_loaded_index++;
                }
                //create middle meshes
                if (_area.middle_loaded_index < _area.middle_models.size())
                {
                    auto _model = _area.middle_models[_area.middle_loaded_index];
                    if (_model->post_load(this->_draw_render_pass) == S_FALSE)
                    {
                        logger.error("error on post loading " + std::string(_model->get_full_name()));
                    }
                    _area.middle_loaded_index++;
                }
                //create inner meshes
                if (_area.inner_loaded_index < _area.inner_models.size())
                {
                    auto _model = _area.inner_models[_area.inner_loaded_index];
                    if (_model->post_load(this->_draw_render_pass) == S_FALSE)
                    {
                        logger.error("error on post loading " + std::string(_model->get_full_name()));
                    }
                    _area.inner_loaded_index++;
                }

                if (_area.outer_loaded_index == _area.outer_models.size() &&
                    _area.middle_loaded_index == _area.middle_models.size() &&
                    _area.inner_loaded_index == _area.inner_models.size())
                {
                    //all meshes have been loaded for this area
                    _area.is_loaded = true;
                    sForceUpdateCamera = true;
                    _total_loaded_areas++;
                }
            }
        }
    }

    bool _gui_proceeded = false;
    if (this->_show_gui)
    {
        w_imgui::new_frame(windows_frame_time_in_sec.at(0), [&_gui_proceeded, this]()
        {
            _gui_proceeded = _update_gui();
        });
    }

    if (sLoading.load())
    {
        _update_media_player();
        return;
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

        auto _cam_pos = this->_camera.get_translate();

        sTotalModels = 0;
        sVisibleSubModels = 0;
        this->_models_to_be_render.clear();

        //clear buffers and wake up masked occlusion threads
        sMOCThreadPool->ClearBuffer();
        sMOCThreadPool->WakeThreads();

        bool _need_flush_moc = false;

        std::for_each(_areas.begin(), _areas.end(), [&](_In_ area& pArea)
        {
#pragma region pre update stage

            auto _outer_size = pArea.outer_models.size();
            auto _middle_size = pArea.middle_models.size();
            auto _inner_size = pArea.inner_models.size();

            sTotalModels += _outer_size + _middle_size + _inner_size;

            pArea.allow_update_inner_models = false;
            pArea.allow_update_middle_models = false;

            //check showing models
            if (_inner_size)
            {
                glm::vec3 _boundary_center(
                    pArea.boundaries[0]->center[0],
                    pArea.boundaries[0]->center[1],
                    pArea.boundaries[0]->center[2]);
                if (glm::distance(_boundary_center, _cam_pos) < pArea.boundaries[0]->radius)
                {
                    pArea.allow_update_inner_models = true;
                }
            }

            //check showing middle models
            if (_middle_size)
            {
                glm::vec3 _boundary_center(
                    pArea.boundaries[1]->center[0],
                    pArea.boundaries[1]->center[1],
                    pArea.boundaries[1]->center[2]);
                if (glm::distance(_boundary_center, _cam_pos) < pArea.boundaries[1]->radius)
                {
                    pArea.allow_update_middle_models = true;
                }
            }

            if (_outer_size)
            {
                std::for_each(pArea.outer_models.begin(), pArea.outer_models.end(), [&](_In_ model* pModel)
                {
                    if (pModel->pre_update(this->_camera, sMOCThreadPool)) _need_flush_moc = true;
                });
            }
            if (pArea.allow_update_middle_models)
            {
                std::for_each(pArea.middle_models.begin(), pArea.middle_models.end(), [&](_In_ model* pModel)
                {
                    if (pModel->pre_update(this->_camera, sMOCThreadPool)) _need_flush_moc = true;
                });
            }
            if (pArea.allow_update_inner_models)
            {
                std::for_each(pArea.inner_models.begin(), pArea.inner_models.end(), [&](_In_ model* pModel)
                {
                    if (pModel->pre_update(this->_camera, sMOCThreadPool)) _need_flush_moc = true;
                });
            }

#pragma endregion
        });

        if (!_need_flush_moc)
        {
            sMOCThreadPool->SuspendThreads();
            return;
        }

        //apply flush for all masked occlusion threads and force them to finish the jobs
        sMOCThreadPool->Flush();

        std::for_each(this->_areas.begin(), this->_areas.end(), [&](_In_ area& pArea)
        {
#pragma region post update stage
            //tbb::parallel_invoke(
              //  [&]()
            //{
                if (pArea.outer_models.size())
                {
                    std::for_each(pArea.outer_models.begin(), pArea.outer_models.end(), [&](_In_ model* pModel)
                    {
                        if (pModel->post_update(sMOCThreadPool, sVisibleSubModels))
                        {
                            this->_models_to_be_render.push_back(pModel);
                        }
                    });
                }
            //},
             //   [&]()
            //{
                if (pArea.allow_update_middle_models)
                {
                    std::for_each(pArea.middle_models.begin(), pArea.middle_models.end(), [&](_In_ model* pModel)
                    {
                        if (pModel->post_update(sMOCThreadPool, sVisibleSubModels))
                        {
                            this->_models_to_be_render.push_back(pModel);
                        }
                    });
                }
            //},
            //        [&]()
            //{
                if (pArea.allow_update_inner_models)
                {
                    std::for_each(pArea.inner_models.begin(), pArea.inner_models.end(), [&](_In_ model* pModel)
                    {
                        if (pModel->post_update(sMOCThreadPool, sVisibleSubModels))
                        {
                            this->_models_to_be_render.push_back(pModel);
                        }
                    });
                }
            //});

#pragma endregion
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

    std::vector<VkSemaphore> _wait_semaphors = { _output_window->vk_swap_chain_image_is_available_semaphore };

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
    _submit_info.pSignalSemaphores = this->_show_gui ? &gui_semaphore : &_output_window->vk_rendering_done_semaphore; //signal to gui if gui is avaiable, otherwise end the render 

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
    
    //release staging texture
    SAFE_RELEASE(sStagingMediaTexture);

    //release all rendering threads
    for (auto _thread_context : this->_render_thread_pool)
    {
        SAFE_RELEASE(_thread_context);
    }
    this->_render_thread_pool.clear();

    this->_models_to_be_render.clear();
    //release all models
    for (auto _area : this->_areas)
    {
        _area.release();
    }
    this->_areas.clear();

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

    w_ffmpeg::release_MF();

    //wait one sec before closing to ensure all threads destroyed successfully
    w_thread::sleep_current_thread(1000);

    return w_game::release();
}

void scene::_update_media_player()
{
#ifdef DEBUG
    return;
#endif // DEBUG

    auto _total_frames = this->_video_streamer.get_total_video_frames();
    if (_total_frames == 0) return;
    const float _one_sec = 1;
    this->_video_time.set_target_elapsed_seconds(static_cast<double>(_one_sec / this->_media_fps));

    if (this->_current_frame >= _total_frames - 1)
    {
        //loop again until loading finished
        this->_current_video_buffer = 0;
        this->_video_frame_address = 0;
        _async_seek(138);
        return;
    }

    this->_video_time.tick([&]()
    {
        w_memory* _memory = nullptr;
        auto _videoFrameAddress = this->_video_frame_address++;
        _memory = &_video_buffers[this->_current_video_buffer];

        auto _writeAddress = _memory->get_address();
        auto _mem = _memory->read(_videoFrameAddress);
        if (_mem)
        {
            auto _frame = static_cast<int*>(_mem);
            if (!_frame)
            {
                //Critical error
                logger.error("Error on casting memory to W_VideoFrameData");
            }
            else
            {
                const size_t _length = this->_media_width * this->_media_height;
                //copy frame to the staging texture
                if (sStagingMediaData)
                {
                    tbb::parallel_for(tbb::blocked_range<size_t>(0, _length), [&](const tbb::blocked_range<size_t>& pRange)
                    {
                        for (size_t i = pRange.begin(); i < pRange.end(); ++i)
                        {
                            auto _c = _frame[i];
                            auto _j = i * 4;
                            sStagingMediaData[_j + 0] = _c & 0xFF;//R
                            sStagingMediaData[_j + 1] = (_c >> 8) & 0xFF;//G
                            sStagingMediaData[_j + 2] = (_c >> 16) & 0xFF;//B
                            sStagingMediaData[_j + 3] = (_c >> 24) & 0xFF;//A
                        }
                    });
                    sStagingMediaTexture->copy_data_to_texture_2D(sStagingMediaData);
                }
                this->_current_frame++;
            }
            if (_writeAddress != 0 && this->_current_frame != _total_video_frames && this->_video_frame_address % _writeAddress == 0)
            {
                //We need to switch to another buffer
                this->_current_video_buffer = (this->_current_video_buffer + 1) % BUFFER_COUNTS;
                this->_video_frame_address = 0;
                //Signal to the waiting thread
                this->_signal_video_buffer_cv.notify_one();
            }

            _frame = nullptr;
            _mem = nullptr;
            _memory = nullptr;
        }
    });
}

void scene::_async_seek(_In_ int64_t pSeekFrame)
{
    this->_halt_buffering.store(true);

    auto _task = concurrency::create_task([this, pSeekFrame]()->void
    {
        //Seek video
        this->_video_streamer_cs.lock();
        this->_video_streamer.seek_frame(pSeekFrame);
        this->_video_streamer_cs.unlock();
        this->_current_frame = pSeekFrame;
        
        _fill_buffers();

        this->_halt_buffering.store(false);
    });
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

    if (sLoading.load())
    {
#pragma region Show Intro

        window_flags |= ImGuiWindowFlags_NoResize;
        window_flags |= ImGuiWindowFlags_NoScrollbar;
        window_flags |= ImGuiWindowFlags_NoCollapse;
        window_flags |= ImGuiWindowFlags_NoTitleBar;
        window_flags |= ImGuiWindowFlags_NoMove;

        _style.Colors[ImGuiCol_Text].x = 1.0f;
        _style.Colors[ImGuiCol_Text].y = 1.0f;
        _style.Colors[ImGuiCol_Text].z = 1.0f;
        _style.Colors[ImGuiCol_Text].w = 1.0f;

        _style.Colors[ImGuiCol_WindowBg].x = 0.9098039215686275f;
        _style.Colors[ImGuiCol_WindowBg].y = 0.4431372549019608f;
        _style.Colors[ImGuiCol_WindowBg].z = 0.3176470588235294f;
        _style.Colors[ImGuiCol_WindowBg].w = 1.0f;

        ImVec2 _image_size;
        ImVec2 _window_size;

        _image_size.x = 1280;
        _image_size.y = 720;
        ImGui::SetNextWindowSizeConstraints(_image_size, _image_size);
        ImGui::SetNextWindowContentWidth(_image_size.x);
        ImGui::SetWindowPos(ImVec2(0, 0));
        if (!ImGui::Begin("", 0, window_flags))
        {
            // Early out if the window is collapsed, as an optimization.
            ImGui::End();
            return _proceeded;
        }

        ImTextureID tex_id = (void*)("#s");//this texture used as staging texture
        ImGui::Image(tex_id, _image_size);
        ImGui::End();

#pragma endregion
    }
    else
    {
        window_flags = 0;
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
            ImTextureID tex_id = (void*)("#i");//this texture used for icons
            ImGui::PushID(i);
            ImGui::PushStyleColor(ImGuiCol_ImageActive, ImColor(0.0f, 0.0f, 255.0f, 255.0f));
            ImGui::PushStyleColor(ImGuiCol_ImageHovered, ImColor(0.0f, 0.0f, 255.0f, 155.0f));
            if (ImGui::ImageButton(tex_id, ImVec2(32, 32), ImVec2(i * 0.1, 0.0), ImVec2(i * 0.1 + 0.1f, 0.1), 0, ImColor(232, 113, 83, 255)))
            {
                //show debugger
                if (i == 4)
                {
#ifdef DEBUG_MASKED_OCCLUSION_CULLING
                    sShowDebugMaskedOcclusionCulling = !sShowDebugMaskedOcclusionCulling;
#endif
                }
                //show live camera
                else if (i == 5)
                {
                    sShowingLiveCamera = !sShowingLiveCamera;
                }
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
                        //start seraching areas
                        for (auto& _area : this->_areas)
                        {
                            std::for_each(_area.outer_models.begin(), _area.outer_models.end(), [_lower_str](_In_ model* pModel)
                            {
                                pModel->search_for_name(_lower_str, sSearchedItems);
                            });
                        }
                    }, [this]()
                    {
                        //on callback
                        sSearching = false;
                        if (sSearchedItems.size() == 0)
                        {
                            //switch all model's transparency to 1
                            for (auto& _area : this->_areas)
                            {
                                std::for_each(_area.outer_models.begin(), _area.outer_models.end(), [](_In_ model* pModel)
                                {
                                    pModel->set_color(glm::vec4(1.0f));
                                });
                            }
                        }
                    });
                }
                else
                {
                    //switch all model's transparency to 1
                    for (auto& _area : this->_areas)
                    {
                        std::for_each(_area.outer_models.begin(), _area.outer_models.end(), [](_In_ model* pModel)
                        {
                            pModel->set_color(glm::vec4(1.0f));
                        });
                    }
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

        if (sShowingLiveCamera)
        {
#pragma region Show Live Camera

            window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |= ImGuiWindowFlags_NoScrollbar;
            window_flags |= ImGuiWindowFlags_NoCollapse;

            _style.Colors[ImGuiCol_Text].x = 1.0f;
            _style.Colors[ImGuiCol_Text].y = 1.0f;
            _style.Colors[ImGuiCol_Text].z = 1.0f;
            _style.Colors[ImGuiCol_Text].w = 1.0f;

            _style.Colors[ImGuiCol_WindowBg].x = 0.9098039215686275f;
            _style.Colors[ImGuiCol_WindowBg].y = 0.4431372549019608f;
            _style.Colors[ImGuiCol_WindowBg].z = 0.3176470588235294f;
            _style.Colors[ImGuiCol_WindowBg].w = 1.0f;

            ImVec2 _image_size(480, 360);
            ImVec2 _window_size = ImVec2(_image_size.x + 5.0f, _image_size.y + 33.0f);

            ImGui::SetNextWindowSizeConstraints(_window_size, _window_size);
            ImGui::SetNextWindowContentWidth(_window_size.x);
            if (!ImGui::Begin("Live Camera", 0, window_flags))
            {
                // Early out if the window is collapsed, as an optimization.
                ImGui::End();
                return _proceeded;
            }

            ImTextureID tex_id = (void*)("#s");//this texture used as staging texture
            ImGui::Image(tex_id, _image_size);
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
        ImGui::Text("FPS:%d\r\nFrameTime:%f\r\nThread pool rendering size: %d\r\nTotal Ref Models: %d\r\nTotal Visible Models: %d\r\nTotal Visible Ref Models: %d",
            sFPS, windows_frame_time_in_sec.at(0), sRenderingThreads, sTotalModels, sVisibleSubModels, sVisibleModels);

        ImGui::End();

#pragma endregion

    }

    return _proceeded;
}

HRESULT scene::_open_media(_In_z_ const std::wstring& pPath, int64_t pSeekToFrame)
{
    auto _hr = S_FALSE;

    if (pPath.empty()) return _hr;

    //Try open and buffer the video of media
    this->_video_streamer_cs.lock();
    {
        _hr = this->_video_streamer.open_media(pPath, pSeekToFrame);
        V(_hr, L"Could not open media for following path: " + pPath, this->name, 3);
    }
    this->_video_streamer_cs.unlock();

    if (_hr == S_FALSE) return _hr;

    //initialize buffers
    this->_media_width = static_cast<uint32_t>(this->_video_streamer.get_video_width());
    this->_media_height = static_cast<uint32_t>(this->_video_streamer.get_video_height());

    for (size_t i = 0; i < BUFFER_COUNTS; ++i)
    {
        if (!this->_video_buffers[i].malloc(MAXIMUM_BUFFER_SIZE * this->_media_width * this->_media_height * sizeof(int)))
        {
            V(S_FALSE, L"Could not allocate memory for preview buffer", this->name, 3, true, false);
            break;
        }
    }

    if (_hr == S_OK)
    {
        this->_media_fps = this->_video_streamer.get_video_frame_rate();
        _fill_buffers();
    }

    return _hr;
}

void scene::_video_buffering_thread()
{
    while (true)
    {
        auto _hr = S_FALSE;
        this->_video_is_buffering = false;

        //Lock this section and wait for signal to start buffering
        tbb::interface5::unique_lock<tbb::mutex> _lk(this->_video_buffer_mutex);
        {
            //Continue to the first state of while
            if (this->_halt_buffering.load() || w_game::exiting)
            {
                continue;
            }
            //Wait for signal from video
            this->_signal_video_buffer_cv.wait(_lk);

            this->_video_is_buffering = true;

            auto _index = this->_current_video_buffer - 1;
            if (_index < 0)
            {
                _index = BUFFER_COUNTS - 1;
            }

            _video_buffers[_index].clear();
            if (this->_total_video_frames_buffered == this->_total_video_frames) continue;

            //Read frames			
            for (size_t i = 0; i < MAXIMUM_BUFFER_SIZE; ++i)
            {
                if (this->_halt_buffering.load())
                {
                    break;
                }
                this->_video_streamer_cs.lock();
                _hr = this->_video_streamer.buffer_video_to_memory(_video_buffers[_index]);
                this->_video_streamer_cs.unlock();

                if (_hr == S_OK)
                {
                    this->_total_video_frames_buffered++;
                    if (this->_total_video_frames_buffered == this->_total_video_frames)
                    {
                        break;
                    }
                }
                else
                {
                    logger.error(L"Video thread could not buffer frame to memory");
                }
            }
        }
        _lk.unlock();
    }
}

void scene::_fill_buffers()
{
    this->_total_video_frames_buffered = 0;
    auto _time = this->_video_streamer.get_duration_time();
    auto _tms = _time.get_total_milliseconds();
    this->_total_video_frames = this->_video_streamer.get_total_video_frames();

    _clear_buffers();

    bool _break_parent = false;
    auto _hr = S_OK;
    for (size_t i = 0; i < BUFFER_COUNTS; ++i)
    {
        for (size_t j = 0; j < MAXIMUM_BUFFER_SIZE; ++j)
        {
            //Check for cancel
            if (w_game::exiting)
            {
                _break_parent = true;
                break;
            }

            this->_video_streamer_cs.lock();
            _hr = _video_streamer.buffer_video_to_memory(_video_buffers[i]);
            this->_video_streamer_cs.unlock();

            if (_hr == S_OK)
            {
                this->_total_video_frames_buffered++;
                if (this->_total_video_frames_buffered >= this->_total_video_frames)
                {
                    _break_parent = true;
                    break;
                }
            }
            else
            {
                logger.warning(L"Could not fill video buffer number : " + std::to_wstring(i));
            }

        }
        if (_break_parent) break;
    }
}

void scene::_clear_buffers()
{
    for (size_t i = 0; i < BUFFER_COUNTS; ++i)
    {
        this->_video_buffers[i].clear();
    }
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

