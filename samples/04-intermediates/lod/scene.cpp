#include "pch.h"
#include "scene.h"
#include <w_graphics/w_pipeline.h>
#include <w_content_manager.h>
#include <w_framework/w_quad.h>
#include <imgui/imgui.h>
#include <w_graphics/w_imgui.h>

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::framework;
using namespace wolf::content_pipeline;

//forward declaration
static void make_gui();

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

    ,_shader(nullptr)

{
    w_game::set_fixed_time_step(false);

#if defined(__WIN32) || defined(__UWP)
    auto _running_dir = pRunningDirectory;
    content_path = _running_dir + L"../../../../content/";
#elif defined(__APPLE__)
    auto _running_dir = wolf::system::convert::string_to_wstring(pRunningDirectory);
    content_path = _running_dir + L"/../../../../../content/";
#endif

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
}

struct LOD
{
    uint32_t    firstIndex;
    uint32_t    indexCount;
    float       distance;
    float       _padding;
};
std::vector<LOD> LODLevels;

wolf::graphics::w_mesh* _mesh = nullptr;

void scene::load()
{
    auto _gDevice = this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    _screen_size.x = static_cast<uint32_t>(_output_window->width);
    _screen_size.y = static_cast<uint32_t>(_output_window->height);

    w_game::load();
    
    HRESULT _hr;

    w_vertex_binding_attributes _vertex_binding_attrs;
    _vertex_binding_attrs.declaration = w_vertex_declaration::USER_DEFINED;
    _vertex_binding_attrs.binding_attributes[0] = { Vec3, Vec2 };
    _vertex_binding_attrs.binding_attributes[1] = { Vec3, Vec3, Float };

    //load scene
    auto _scene = w_content_manager::load<w_cpipeline_scene>(content_path + L"models/lod_pri.dae");
    if (_scene)
    {
        //get all models
        std::vector<w_cpipeline_model*> _cmodels;
        _scene->get_all_models(_cmodels);

        auto _z_up = _scene->get_z_up();

        uint32_t n = 0;

        //create one big vertex buffer and index buffer from all LODs
        std::vector<float> _vertices;
        std::vector<uint32_t> _indices;

        uint32_t _base_index = 0;
        uint32_t _base_vertex = 0;

        for (auto& _model : _cmodels)
        {
            //load meshes
            std::vector<w_cpipeline_model::w_mesh*> _model_meshes;
            _model->get_meshes(_model_meshes);

            for (auto& _mesh_data : _model_meshes)
            {
                _base_index = _indices.size();

                for (size_t i = 0; i < _mesh_data->indices.size(); ++i)
                {
                    _indices.push_back(_base_vertex + _mesh_data->indices[i]);
                }

                LOD lod;
                lod.firstIndex = _base_index;			        // First index for this LOD
                lod.indexCount = _mesh_data->indices.size();	// Index count for this LOD
                lod.distance = 200.0f + n * 50.0f;				// Starting distance (to viewer) for this LOD
                n++;
                LODLevels.push_back(lod);

                for (auto& _data : _mesh_data->vertices)
                {
                    auto _pos = _data.position;
                    auto _uv = _data.uv;

                    //position
                    _vertices.push_back(_pos[0]);
                    _vertices.push_back(_pos[1]);
                    _vertices.push_back(_pos[2]);

                    //uv
                    _vertices.push_back(_uv[0]);
                    _vertices.push_back(1 - _uv[1]);

                    _base_vertex++;
                }
            }

            //std::vector<w_cpipeline_model::w_instance_info> _model_instances;
            //_model->get_instances(_model_instances);
            //if (_model_instances.size())
            //{
            //    //update_instance_buffer(_model_instances.data(),
            //    //    static_cast<UINT>(_model_instances.size() * sizeof(content_pipeline::w_cpipeline_model::w_instance_info)),
            //    //    w_mesh::w_vertex_declaration::VERTEX_POSITION_UV_INSTANCE_VEC7_INT);
            //}

        }

        _mesh = new (std::nothrow) wolf::graphics::w_mesh();
        if (!_mesh)
        {
            logger.error("Error on allocating memory for mesh");
            w_game::exit();
        }
        auto _v_size = static_cast<uint32_t>(_vertices.size());
        _hr = _mesh->load(_gDevice,
            _vertices.data(),
            _v_size * sizeof(float),
            _v_size,
            _indices.data(),
            _indices.size());
        _mesh->set_vertex_binding_attributes(_vertex_binding_attrs);

        _vertices.clear();
        _indices.clear();

        if (_hr == S_FALSE)
        {
            logger.error("Error on loading vertices for mesh");
            w_game::exit();
        }

        _scene->get_first_camera(this->_camera);
        this->_camera.set_far_plan(10000);
        this->_camera.set_aspect_ratio((float)_screen_size.x / (float)_screen_size.y);

        this->_camera.set_translate(0, 0, 0);
        this->_camera.update_view();
        this->_camera.update_projection();
        this->_camera.update_frustum();

        _scene->release();
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

    //create render pass
    _hr = this->_render_pass.load(
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


    _prepare_buffers(_gDevice);

    auto _render_pass_handle = this->_render_pass.get_handle();
    
    std::string _pipeline_cache_name = "pipeline_cache";
    if (w_pipeline::create_pipeline_cache(_gDevice, _pipeline_cache_name) == S_FALSE)
    {
        logger.error("Could not create pipeline cache");
        _pipeline_cache_name.clear();
    }

    this->_vertex_unifrom.load(_gDevice);
    this->_compute_unifrom.load(_gDevice);

    //load shader
    std::vector<w_shader_binding_param> _shader_params;

    w_shader_binding_param _param;
    _param.index = 0;
    _param.type = w_shader_binding_type::UNIFORM;
    _param.stage = w_shader_stage::VERTEX_SHADER;
    _param.buffer_info = this->_vertex_unifrom.get_descriptor_info();
    _shader_params.push_back(_param);
    
    _param.index = 1;
    _param.type = w_shader_binding_type::SAMPLER;
    _param.stage = w_shader_stage::FRAGMENT_SHADER;
    _param.image_info = w_texture::default_texture->get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 0;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = instanceBuffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 1;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = indirectCommandsBuffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 2;
    _param.type = w_shader_binding_type::UNIFORM;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = this->_compute_unifrom.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 3;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = indirectDrawCountBuffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 4;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = compute.lodLevelsBuffers.get_descriptor_info();
    _shader_params.push_back(_param);

    //load shaders
    _hr = w_shader::load_to_shared_shaders(
        _gDevice,
        "shader",
        content_path + L"shaders/compute/indirect_draw.vert.spv",
        L"",
        L"",
        L"",
        content_path + L"shaders/compute/indirect_draw.frag.spv",
        content_path + L"shaders/compute/cull_lod_local_size_x1.comp.spv",
        _shader_params,
        &this->_shader);
    if (_hr == S_FALSE)
    {
        logger.error("Error on loading indirect shader");
        w_game::exiting = true;
        return;
    }


    std::vector<VkDynamicState> _dynamic_states =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    auto _descriptor_set_layout_binding = this->_shader->get_descriptor_set_layout();
    this->_pipeline = new w_pipeline();
    _hr = this->_pipeline->load(
        _gDevice,
        _vertex_binding_attrs,
        VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        _render_pass_handle,
        _shader->get_shader_stages(),
        &_descriptor_set_layout_binding,
        { this->_render_pass.get_viewport() }, //viewports
        { this->_render_pass.get_viewport_scissor() }, //viewports scissor
        _dynamic_states,
        "pipeline_cache",
        0,
        nullptr,
        nullptr,
        _output_window->vk_depth_buffer_format != VkFormat::VK_FORMAT_UNDEFINED);
    if (_hr)
    {
        logger.error("Error creating pipeline for mesh");
    }

        
    auto _compute_descriptor_set_layout_binding = this->_shader->get_compute_descriptor_set_layout();
    auto _compute_shader_stage = this->_shader->get_compute_shader_stage();

    this->_compute_pipeline = new w_pipeline();
    _hr = this->_compute_pipeline->load_compute(
        _gDevice,
        _compute_shader_stage,
        _compute_descriptor_set_layout_binding,
        6,
        "pipeline_cache");
    if (_hr)
    {
        logger.error("Error creating pipeline for mesh");
    }

    //create frame buffers
    _hr = this->_frame_buffers.load(_gDevice,
        _render_pass_handle,
        _output_window->vk_swap_chain_image_views,
        &_output_window->vk_depth_buffer_image_view,
        _screen_size,
        1);
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating frame buffers");
        release();
        exit(1);
    }


    //create command buffer for compute shader
    compute.command_buffer.load(_gDevice, 1, true, &_gDevice->vk_compute_queue);
    // Fence for compute CB sync
    VkFenceCreateInfo _fence_create_info = {};
    _fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    _fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    _hr = vkCreateFence(
        _gDevice->vk_device, 
        &_fence_create_info,
        nullptr, 
        &compute.fence);

    VkSemaphoreCreateInfo _semaphore_create_info = {};
    _semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    _hr = vkCreateSemaphore(_gDevice->vk_device, 
        &_semaphore_create_info,
        nullptr, 
        &compute.semaphore);
    
    //now assign new command buffers
    this->_draw_command_buffers.set_enable(true);
    this->_draw_command_buffers.set_order(1);
    _hr = this->_draw_command_buffers.load(_gDevice, _output_window->vk_swap_chain_image_views.size());
    if (_hr == S_FALSE)
    {
        logger.error("Error on creating command buffers");
        release();
        exit(1);
    }

    _hr = _gDevice->store_to_global_command_buffers("draw",
        &this->_draw_command_buffers,
        _output_window->index);
    if (_hr == S_FALSE)
    {
        logger.error("Error creating command buffer");
        return;
    }

    _output_window->command_buffers.at("clear_color_screen")->set_enable(false);

    //load image texture
    w_texture* _gui_images = new w_texture();
    _gui_images->load(_gDevice);
    _gui_images->initialize_texture_2D_from_file(content_path + L"textures/gui/icons.png", &_gui_images);
    w_imgui::load(_gDevice, _output_window->hwnd, _screen_size, _render_pass_handle, _gui_images);



    _record_compute_command_buffer(_gDevice);
    _record_draw_command_buffer(_gDevice);
}

void scene::_prepare_buffers(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    w_buffer stagingBuffer;

    const int __count = 1;

    // Indirect draw commands
    indirectCommands.resize(__count);
    for (uint32_t z = 0; z < __count; z++)
    {
        uint32_t index = 0;
        indirectCommands[index].instanceCount = 1;
        indirectCommands[index].firstInstance = index++;
        // firstIndex and indexCount are written by the compute shader
    }

    indirectStats.drawCount = static_cast<uint32_t>(indirectCommands.size());

    uint32_t _size = (uint32_t)(indirectCommands.size() * sizeof(VkDrawIndexedIndirectCommand));
    auto _hr = stagingBuffer.load_as_staging(pGDevice, _size);
    _hr = stagingBuffer.bind();
    _hr = stagingBuffer.set_data(indirectCommands.data());

    _hr = indirectCommandsBuffer.load(
        pGDevice,
        _size,
        VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    _hr = indirectCommandsBuffer.bind();

    _hr = stagingBuffer.copy_to(indirectCommandsBuffer);
    stagingBuffer.release();

    _size = sizeof(indirectStats);
    _hr = this->indirectDrawCountBuffer.load(
        pGDevice,
        _size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    _hr = this->indirectDrawCountBuffer.bind();
    this->indirectDrawCountBuffer.map();


    std::vector<InstanceData> instanceData(__count);
    // Map for host access
    for (uint32_t z = 0; z < __count; z++)
    {
        uint32_t index = 0;
        instanceData[index].pos = glm::vec3(0, 0, 0);
        instanceData[index].rot = glm::vec3(0, 0, 0);
        instanceData[index].scale = 1.0f;
    }

    _size = instanceData.size() * sizeof(InstanceData);
    w_buffer stagingBuffer_1;
    _hr = stagingBuffer_1.load_as_staging(pGDevice, _size);
    _hr = stagingBuffer_1.bind();
    _hr = stagingBuffer_1.set_data(instanceData.data());

    _hr = instanceBuffer.load(pGDevice,
        _size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    _hr = instanceBuffer.bind();
    _hr = stagingBuffer_1.copy_to(instanceBuffer);
    stagingBuffer_1.release();

    _size = LODLevels.size() * sizeof(LOD);
    w_buffer stagingBuffer_2;
    _hr = stagingBuffer_2.load_as_staging(pGDevice, _size);
    _hr = stagingBuffer_2.bind();
    _hr = stagingBuffer_2.set_data(LODLevels.data());

    compute.lodLevelsBuffers.load(
        pGDevice,
        _size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    compute.lodLevelsBuffers.bind();

    _hr = stagingBuffer_2.copy_to(compute.lodLevelsBuffers);
    _hr = stagingBuffer_2.release();

    _hr = this->_compute_unifrom.load(pGDevice, true);
}

HRESULT scene::_record_compute_command_buffer(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    VkCommandBufferBeginInfo _cmd_buffer_info = {};
    _cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    
    auto _cmd = compute.command_buffer.get_command_at(0);
    vkBeginCommandBuffer(_cmd, &_cmd_buffer_info);

    // Add memory barrier to ensure that the indirect commands have been consumed before the compute shader updates them
    VkBufferMemoryBarrier _buffer_barrier = {};
    _buffer_barrier .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    _buffer_barrier.buffer = indirectCommandsBuffer.get_handle();
    _buffer_barrier.size = indirectCommandsBuffer.get_descriptor_info().range;
    _buffer_barrier.srcAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    _buffer_barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    _buffer_barrier.srcQueueFamilyIndex = pGDevice->vk_graphics_queue.index;
    _buffer_barrier.dstQueueFamilyIndex = pGDevice->vk_compute_queue.index;

    vkCmdPipelineBarrier(
        _cmd,
        VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0,
        0, nullptr,
        1, &_buffer_barrier,
        0, nullptr);

    auto _desciptor_set = this->_shader->get_compute_descriptor_set();
    vkCmdBindPipeline(_cmd, VK_PIPELINE_BIND_POINT_COMPUTE, this->_compute_pipeline->get_handle());
    vkCmdBindDescriptorSets(
        _cmd,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        this->_compute_pipeline->get_layout_handle(),
        0,
        1, &_desciptor_set,
        0,
        0);

    // Dispatch the compute job
    // The compute shader will do the frustum culling and adjust the indirect draw calls depending on object visibility. 
    // It also determines the lod to use depending on distance to the viewer.
    vkCmdDispatch(_cmd, 1 /*objectCount / 16*/, 1, 1);

    // Add memory barrier to ensure that the compute shader has finished writing the indirect command buffer before it's consumed
    _buffer_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    _buffer_barrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    _buffer_barrier.buffer = indirectCommandsBuffer.get_handle();
    _buffer_barrier.size = indirectCommandsBuffer.get_descriptor_info().range;
    _buffer_barrier.srcQueueFamilyIndex = pGDevice->vk_compute_queue.index;
    _buffer_barrier.dstQueueFamilyIndex = pGDevice->vk_graphics_queue.index;

    vkCmdPipelineBarrier(
        _cmd,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
        0,
        0, nullptr,
        1, &_buffer_barrier,
        0, nullptr);

    // todo: barrier for indirect stats buffer?

    vkEndCommandBuffer(_cmd);

    return S_OK;
}

HRESULT scene::_record_draw_command_buffer(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    //record clear screen command buffer for every swap chain image
    for (uint32_t i = 0; i < this->_draw_command_buffers.get_commands_size(); ++i)
    {
        this->_draw_command_buffers.begin(i);
        {
            auto _cmd = this->_draw_command_buffers.get_command_at(i);

            this->_render_pass.begin(_cmd,
                this->_frame_buffers.get_frame_buffer_at(i),
                w_color(43));
            {

                // Mesh containing the LODs


                auto _descriptor_set = this->_shader->get_descriptor_set();
                this->_pipeline->bind(_cmd, &_descriptor_set);

                uint32_t _instances_count = 1;
                _mesh->draw(_cmd, instanceBuffer.get_handle(), _instances_count, true);

                if (pGDevice->vk_physical_device_features.multiDrawIndirect)
                {
                    vkCmdDrawIndexedIndirect(
                        _cmd,
                        indirectCommandsBuffer.get_handle(),
                        0,
                        indirectStats.drawCount,
                        sizeof(VkDrawIndexedIndirectCommand));
                }
                else
                {
                    // If multi draw is not available, we must issue separate draw commands
                    for (auto j = 0; j < indirectCommands.size(); j++)
                    {
                        vkCmdDrawIndexedIndirect(
                            _cmd,
                            indirectCommandsBuffer.get_handle(),
                            j * sizeof(VkDrawIndexedIndirectCommand),
                            1, sizeof(VkDrawIndexedIndirectCommand));
                    }
                }


                //make sure render all gui before loading gui_render
                //w_imgui::new_frame((float)pGameTime.get_elapsed_seconds(), []()
                //{
                //    make_gui();
                //});
                //w_imgui::update_buffers(this->_render_pass);
                //w_imgui::render(_cmd);
            }
            this->_render_pass.end(_cmd);
        }
        this->_draw_command_buffers.end(i);
    }

    return S_OK;
}

void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
    if (w_game::exiting) return;
    this->_camera.update(pGameTime, this->_screen_size);
    this->_camera.update_frustum();
    
    auto _pos = this->_camera.get_translate();
    auto _view_projection = this->_camera.get_projection() * this->_camera.get_view();

    this->_vertex_unifrom.data.projection_view = _view_projection;

    this->_compute_unifrom.data.cameraPos = glm::vec4(_pos.x, _pos.y, _pos.z, 1.0) * -1.0f;
    std::memcpy(this->_compute_unifrom.data.frustumPlanes, this->_camera.get_frustum_plans().data(), sizeof(glm::vec4) * 6);
    
    auto _hr = this->_compute_unifrom.update();
    _hr = this->_vertex_unifrom.update();
    
    w_game::update(pGameTime);
}

static float __t = 1.0f;
HRESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
    auto _gDevice = this->graphics_devices[0];
    auto _output_window = &(_gDevice->output_presentation_windows[0]);
    
    // Wait for fence to ensure that compute buffer writes have finished
    vkWaitForFences(_gDevice->vk_device, 1, &compute.fence, VK_TRUE, UINT64_MAX);
    vkResetFences(_gDevice->vk_device, 1, &compute.fence);

    auto _c_cmd = this->compute.command_buffer.get_command_at(0);

    VkSubmitInfo computeSubmitInfo = {};
    computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &_c_cmd;
    computeSubmitInfo.signalSemaphoreCount = 1;
    computeSubmitInfo.pSignalSemaphores = &compute.semaphore;

    auto _hr = vkQueueSubmit(_gDevice->vk_compute_queue.queue, 1, &computeSubmitInfo, 0);

    // Submit graphics command buffer
    auto _cmd = this->_draw_command_buffers.get_command_at(_output_window->vk_swap_chain_image_index);

    VkSubmitInfo _submit_info = {};
    _submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    _submit_info.commandBufferCount = 1;
    _submit_info.pCommandBuffers = &_cmd;

    // Wait on present and compute semaphores
    std::array<VkPipelineStageFlags, 2> stageFlags = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
    };
    std::array<VkSemaphore, 2> waitSemaphores = 
    {
        _output_window->vk_swap_chain_image_is_available_semaphore,						// Wait for presentation to finished
        compute.semaphore								// Wait for compute to finish
    };

    _submit_info.pWaitSemaphores = waitSemaphores.data();
    _submit_info.waitSemaphoreCount = static_cast<uint32_t>(waitSemaphores.size());
    _submit_info.pWaitDstStageMask = stageFlags.data();
    _submit_info.signalSemaphoreCount = 1;
    _submit_info.pSignalSemaphores = &_output_window->vk_rendering_done_semaphore;

    // Submit to queue
    _hr = vkQueueSubmit(_gDevice->vk_graphics_queue.queue, 1, &_submit_info, compute.fence);



    logger.write(std::to_string(pGameTime.get_frames_per_second()));
    auto __hr =  w_game::render(pGameTime);


    // Get draw count from compute
    auto _mapped = indirectDrawCountBuffer.map();
    memcpy(&indirectStats, _mapped, sizeof(indirectStats));
    indirectDrawCountBuffer.flush();
    indirectDrawCountBuffer.unmap();

    for (size_t i = 0; i < 6; i++)
    {
        if (indirectStats.lodCount[i])
        {
            logger.write("lod " + std::to_string(i));
        }
    }
    return __hr;
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

    this->_vertex_unifrom.release();
    this->_compute_unifrom.release();

    this->_render_pass.release();
    this->_frame_buffers.release();

    w_imgui::release();
    
    for (auto _model : this->_models)
    {
        SAFE_RELEASE(_model);
    }
    this->_models.clear();

    SAFE_RELEASE(this->_shader);
    SAFE_RELEASE(this->_pipeline);
   
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
    ImGui::SetNextWindowSize(ImVec2(400, 400), ImGuiSetCond_FirstUseEver);
    for (int i = 0; i < 6; i++)
    {
        ImTextureID tex_id = (void*)("#image");
        ImGui::PushID(i);
        ImGui::PushStyleColor(ImGuiCol_ImageActive, ImColor(0.0f, 0.0f, 255.0f, 255.0f));
        ImGui::PushStyleColor(ImGuiCol_ImageHovered, ImColor(0.0f, 0.0f, 255.0f, 155.0f));
        if (ImGui::ImageButton(tex_id, ImVec2(40, 40), ImVec2(i * 0.1, 0.0), ImVec2(i * 0.1 + 0.1f, 0.1), 0, ImColor(232, 113, 83, 255)))
        {
            __t -= 0.1f;
        }
        ImGui::PopStyleColor(2);
        ImGui::PopID();
        ImGui::Spacing();
        ImGui::Spacing();
    }

    //ImGui::ShowTestWindow();

    ImGui::End();
}