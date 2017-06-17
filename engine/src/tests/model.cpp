#include "pch.h"
#include "model.h"

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

model::model() : 
    _mesh(nullptr),
    _shader(nullptr)
{
    //define vertex binding attributes
    this->_vertex_binding_attributes.declaration = w_vertex_declaration::USER_DEFINED;
    this->_vertex_binding_attributes.binding_attributes[0] = { Vec3, Vec2 };
    this->_vertex_binding_attributes.binding_attributes[1] = { Vec3, Vec3, Float };
}

model::~model()
{
    release();
}

HRESULT model::load(
    _In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
    _In_ w_cpipeline_model* pCPModel,
    _In_ w_render_pass& pRenderPass)
{
    if (!pGDevice || !pCPModel) return S_FALSE;

    const std::string _trace = this->name + "::load";

    //get trasform of base object and instances objects
    this->_transform = pCPModel->get_transform();
    pCPModel->get_instances(this->_instances_transforms);
    
    //get full name 
    this->_full_name = pCPModel->get_name();

    //get serachable name
    std::vector<std::string> _splits;
    wolf::system::convert::split_string(this->_full_name, "_", _splits);
    for (size_t i = 1; i < _splits.size(); i++)
    {
        this->_search_name += _splits[i];
    }
    _splits.clear();

    uint32_t _lod_distance_index = 1;
    const uint32_t _lod_distance_offset = 50;

    //create one big vertex buffer and index buffer from root model and LODs
    std::vector<float> _batch_vertices;
    std::vector<uint32_t> _batch_indices;

    //first store vertice and indices of root model
    std::vector<w_cpipeline_model::w_mesh*> _model_meshes;
    pCPModel->get_meshes(_model_meshes);

    uint32_t _base_vertex = 0;

    if (_model_meshes.size())
    {
        this->_bounding_boxes = _model_meshes[0]->bounding_box;

        //create first lod information
        lod _lod;
        _lod.first_index = _batch_indices.size();// First index for this LOD
        _lod.index_count = _model_meshes[0]->indices.size();// Index count for this LOD
        _lod.distance = _lod_distance_index * _lod_distance_offset;
        _lod_distance_index++;

        this->_lod_levels.push_back(_lod);

        _store_to_batch(_model_meshes, _batch_vertices, _batch_indices, _base_vertex);
    }

    //append load mesh data to big vertices and indices
    std::vector<w_cpipeline_model*> _lods;
    pCPModel->get_lods(_lods);

    for (auto& _lod_mesh_data : _lods)
    {
        _model_meshes.clear();
        _lod_mesh_data->get_meshes(_model_meshes);

        if (_model_meshes.size())
        {
            //create first lod information
            lod _lod;
            _lod.first_index = _batch_indices.size();// First index for this LOD
            _lod.index_count = _model_meshes[0]->indices.size();// Index count for this LOD
            _lod.distance = _lod_distance_index * _lod_distance_offset;
            _lod_distance_index++;

            this->_lod_levels.push_back(_lod);

            _store_to_batch(_model_meshes, _batch_vertices, _batch_indices, _base_vertex);
        }
    }

    if (!_batch_vertices.size())
    {
        V(S_FALSE, "Model " + this->_full_name + " does not have vertices data", _trace);
        return S_FALSE;
    }

    //create mesh
    this->_mesh = new (std::nothrow) wolf::graphics::w_mesh();
    if (!_mesh)
    {
        V(S_FALSE, "Error on allocating memory for mesh for " + this->_full_name, _trace);
        return S_FALSE;
    }

    auto _v_size = static_cast<uint32_t>(_batch_vertices.size());
    auto _hr = _mesh->load(pGDevice,
        _batch_vertices.data(),
        _v_size * sizeof(float),
        _v_size,
        _batch_indices.data(),
        _batch_indices.size());
    _mesh->set_vertex_binding_attributes(this->_vertex_binding_attributes);

    _batch_vertices.clear();
    _batch_indices.clear();

    if (_hr == S_FALSE)
    {
        V(S_FALSE, "Error on loading mesh for " + this->_full_name, _trace);
        return S_FALSE;
    }


    if (_load_buffers(pGDevice) == S_FALSE) return S_FALSE;
    if (_load_shader(pGDevice) == S_FALSE) return S_FALSE;
    if (_load_pipelines(pGDevice, pRenderPass) == S_FALSE) return S_FALSE;

    return S_OK;
}

void model::_store_to_batch(
    _In_ const std::vector<wolf::content_pipeline::w_cpipeline_model::w_mesh*>& pModelMeshes,
    _Inout_ std::vector<float>& pBatchVertices,
    _Inout_ std::vector<uint32_t>& pBatchIndices,
    _Inout_ uint32_t& pBaseVertex)
{
    for (auto& _mesh_data : pModelMeshes)
    {
        for (size_t i = 0; i < _mesh_data->indices.size(); ++i)
        {
            pBatchIndices.push_back(pBaseVertex + _mesh_data->indices[i]);
        }

        for (auto& _data : _mesh_data->vertices)
        {
            auto _pos = _data.position;
            auto _uv = _data.uv;

            //position
            pBatchVertices.push_back(_pos[0]);
            pBatchVertices.push_back(_pos[1]);
            pBatchVertices.push_back(_pos[2]);

            //uv
            pBatchVertices.push_back(_uv[0]);
            pBatchVertices.push_back(1 - _uv[1]);

            pBaseVertex++;
        }
    }
}

HRESULT model::_load_shader(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
    const std::string _trace = this->name + "::_load_shader";

    if (this->vs.unifrom.load(pGDevice) == S_FALSE)
    {
        V(S_FALSE, "Error on loading vertex uniform for " + this->_full_name, _trace);
        return S_FALSE;
    }
    if (this->cs.unifrom.load(pGDevice) == S_FALSE)
    {
        V(S_FALSE, "Error on loading compute uniform for " + this->_full_name, _trace);
        return S_FALSE;
    }

    std::vector<w_shader_binding_param> _shader_params;

    w_shader_binding_param _param;
    _param.index = 0;
    _param.type = w_shader_binding_type::UNIFORM;
    _param.stage = w_shader_stage::VERTEX_SHADER;
    _param.buffer_info = this->vs.unifrom.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 1;
    _param.type = w_shader_binding_type::SAMPLER;
    _param.stage = w_shader_stage::FRAGMENT_SHADER;
    _param.image_info = w_texture::default_texture->get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 0;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = cs.instance_buffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 1;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = cs.indirect_draw_commands_buffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 2;
    _param.type = w_shader_binding_type::UNIFORM;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = cs.unifrom.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 3;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = cs.indirect_draw_count_buffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 4;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = cs.lod_levels_buffers.get_descriptor_info();
    _shader_params.push_back(_param);

    //check path of shader
    auto _compute_shader_path = content_path + L"shaders/compute/cull_lod_local_size_x" + std::to_wstring(cs.batch_local_size) + L".comp.spv";
    if (wolf::system::io::get_is_file(_compute_shader_path.c_str()) == S_FALSE)
    {
        V(S_FALSE, L"compute shader not exists on path " + _compute_shader_path, _trace);
        return S_FALSE;
    }

    //load shaders
    if (w_shader::load_to_shared_shaders(
        pGDevice,
        "shader",
        content_path + L"shaders/compute/indirect_draw.vert.spv",
        L"",
        L"",
        L"",
        content_path + L"shaders/compute/indirect_draw.frag.spv",
        _compute_shader_path,
        _shader_params,
        &this->_shader) == S_FALSE)
    {
        V(S_FALSE, "Error on loading shader for " + this->_full_name, _trace);
        return S_FALSE;
    }

    return S_OK;
}

HRESULT model::_load_buffers(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
    const std::string _trace = this->name + "::_prepare_buffers";

    w_buffer _staging_buffers[4];
    defer _(nullptr, [&](...) 
    { 
        for (size_t i = 0; i < 4; ++i)
        {
            _staging_buffers[i].release();
        }
    });

    //set compute shader batch size
    uint32_t _draw_counts = 1 + static_cast<uint32_t>(this->_instances_transforms.size());

    //find nearest pow of 2 for compute shader local batch size
    cs.batch_local_size = pow(2, ceil(log(_draw_counts) / log(2)));
    cs.indirect_draw_commands.resize(cs.batch_local_size);
    
    cs.indirect_status.draw_count = _draw_counts;
    cs.indirect_status.lod_count.resize(_draw_counts);

    for (size_t i = 0; i < _draw_counts; ++i)
    {
        cs.indirect_draw_commands[i].instanceCount = 1;
        cs.indirect_draw_commands[i].firstInstance = i;
        // firstIndex and indexCount are written by the compute shader
    }

    uint32_t _size = (uint32_t)(_draw_counts * sizeof(VkDrawIndexedIndirectCommand));
    if (_staging_buffers[0].load_as_staging(pGDevice, _size) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of indirect_draw_commands", _trace, 3);
        return S_FALSE;
    }
    
    if (_staging_buffers[0].bind() == S_FALSE)
    {
        V(S_FALSE, "binding to staging buffer of indirect_draw_commands", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[0].set_data(cs.indirect_draw_commands.data()) == S_FALSE)
    {
        V(S_FALSE, "setting data for staging buffer of indirect_draw_commands", _trace, 3);
        return S_FALSE;
    }

    if (cs.indirect_draw_commands_buffer.load(
        pGDevice,
        _size,
        VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of indirect_commands_buffer", _trace, 3);
        return S_FALSE;
    }

    if (cs.indirect_draw_commands_buffer.bind() == S_FALSE)
    {
        V(S_FALSE, "binding to staging buffer of indirect_commands_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[0].copy_to(cs.indirect_draw_commands_buffer) == S_FALSE)
    {
        V(S_FALSE, "copy staging buffer to device buffer of indirect_commands_buffer", _trace, 3);
        return S_FALSE;
    }

    _size = sizeof(cs.indirect_status);
    if (cs.indirect_draw_count_buffer.load(
        pGDevice,
        _size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading device buffer of indirect_draw_count", _trace, 3);
        return S_FALSE;
    }
    
    if (cs.indirect_draw_count_buffer.bind() == S_FALSE)
    {
        V(S_FALSE, "binding to device buffer of indirect_draw_count", _trace, 3);
        return S_FALSE;
    }
    //cs.indirect_draw_count_buffer.map();

    std::vector<vertex_instance_data> _vertex_instances_data(_draw_counts);
    //first one is root model
    _vertex_instances_data[0].pos[0] = this->_transform.position[0];
    _vertex_instances_data[0].pos[1] = this->_transform.position[1];
    _vertex_instances_data[0].pos[2] = this->_transform.position[2];
    
    _vertex_instances_data[0].rot[0] = this->_transform.rotation[0];
    _vertex_instances_data[0].rot[1] = this->_transform.rotation[1];
    _vertex_instances_data[0].rot[2] = this->_transform.rotation[2];

    //TODO: remove it
    _vertex_instances_data[0].scale = 1.0f;
    
    //others are instances
    _size = this->_instances_transforms.size();
    for (size_t i = 1; i < _size; ++i)
    {
        _vertex_instances_data[i].pos[0] = this->_instances_transforms[i].position[0];
        _vertex_instances_data[i].pos[1] = this->_instances_transforms[i].position[1];
        _vertex_instances_data[i].pos[2] = this->_instances_transforms[i].position[2];

        _vertex_instances_data[i].rot[0] = this->_instances_transforms[i].rotation[0];
        _vertex_instances_data[i].rot[1] = this->_instances_transforms[i].rotation[1];
        _vertex_instances_data[i].rot[2] = this->_instances_transforms[i].rotation[2];

        //TODO: remove it
        _vertex_instances_data[i].scale = 1.0f;
    }


    if (_staging_buffers[1].load_as_staging(pGDevice, _size * sizeof(vertex_instance_data)) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[1].bind() == S_FALSE)
    {
        V(S_FALSE, "binding to staging buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[1].set_data(_vertex_instances_data.data()) == S_FALSE)
    {
        V(S_FALSE, "setting data to staging buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (vs.instance_buffer.load(pGDevice,
        _size,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading device buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (vs.instance_buffer.bind() == S_FALSE)
    {
        V(S_FALSE, "binding to device buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }
    if (_staging_buffers[1].copy_to(vs.instance_buffer) == S_FALSE)
    {
        V(S_FALSE, "copying to device buffer of vertex_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    _size = this->_lod_levels.size() * sizeof(lod);
    if (_staging_buffers[2].load_as_staging(pGDevice, _size) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[2].bind() == S_FALSE)
    {
        V(S_FALSE, "binding staging buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[2].set_data(this->_lod_levels.data()))
    {
        V(S_FALSE, "setting data to staging buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    if (cs.lod_levels_buffers.load(
        pGDevice,
        _size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading data to staging buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    if (cs.lod_levels_buffers.bind() == S_FALSE)
    {
        V(S_FALSE, "binding to device buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[2].copy_to(cs.lod_levels_buffers) == S_FALSE)
    {
        V(S_FALSE, "copy staging buffer to device buffer of lod_levels_buffers", _trace, 3);
        return S_FALSE;
    }

    if (this->cs.unifrom.load(pGDevice, true) == S_FALSE)
    {
        V(S_FALSE, "loading compute shader uniform", _trace, 3);
        return S_FALSE;
    }

    std::vector<compute_instance_data> _compute_instance_data(_draw_counts);
    // Map for host access
    auto _bb_min = glm::vec4(this->_bounding_boxes.min[0], this->_bounding_boxes.min[1], this->_bounding_boxes.min[2], 0.0f);
    auto _bb_max = glm::vec4(this->_bounding_boxes.max[0], this->_bounding_boxes.max[1], this->_bounding_boxes.max[2], 0.0f);

    for (size_t i = 0; i < _draw_counts; i++)
    {
        _compute_instance_data[i].min_bounding_box = _bb_min + glm::vec4(_vertex_instances_data[i].pos, 0.0f);
        _compute_instance_data[i].max_bounding_box = _bb_max + glm::vec4(_vertex_instances_data[i].pos, 0.0f);
    }

    //centers[0] = (_compute_instance_data[0].min_bounding_box + _compute_instance_data[0].max_bounding_box) / 2.0f;

    _size = _compute_instance_data.size() * sizeof(compute_instance_data);
    if (_staging_buffers[3].load_as_staging(pGDevice, _size) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[3].bind() == S_FALSE)
    {
        V(S_FALSE, "binding to device buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[3].set_data(_compute_instance_data.data()) == S_FALSE)
    {
        V(S_FALSE, "setting data to staging buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (cs.instance_buffer.load(pGDevice,
        _size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading device buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (cs.instance_buffer.bind() == S_FALSE)
    {
        V(S_FALSE, "binding device buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[3].copy_to(cs.instance_buffer) == S_FALSE)
    {
        V(S_FALSE, "copy staging buffer to device buffer of compute_instance_buffer", _trace, 3);
        return S_FALSE;
    }
    
    return S_OK;
}

HRESULT model::_load_pipelines(
    _In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
    _In_ w_render_pass& pRenderPass)
{
    const std::string _trace = this->name + "::_load_pipelines";

    std::vector<VkDynamicState> _dynamic_states =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    auto _descriptor_set_layout_binding = this->_shader->get_descriptor_set_layout();
    auto _hr = vs.pipeline.load(
        pGDevice,
        this->_vertex_binding_attributes,
        VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        pRenderPass.get_handle(),
        _shader->get_shader_stages(),
        &_descriptor_set_layout_binding,
        { pRenderPass.get_viewport() }, //viewports
        { pRenderPass.get_viewport_scissor() }, //viewports scissor
        _dynamic_states,
        "pipeline_cache",
        0,
        nullptr,
        nullptr,
        true);//depth
    if (_hr)
    {
        V(S_FALSE, "creating vertex pipeline for " + this->_full_name, _trace);
        return S_FALSE;
    }

    auto _compute_descriptor_set_layout_binding = this->_shader->get_compute_descriptor_set_layout();
    auto _compute_shader_stage = this->_shader->get_compute_shader_stage();

    if (cs.pipeline.load_compute(
        pGDevice,
        _compute_shader_stage,
        _compute_descriptor_set_layout_binding,
        6,
        "model_pipeline_cache") == S_FALSE)
    {
        V(S_FALSE, "loading compute pipeline for " + this->_full_name, _trace);
        return S_FALSE;
    }
}

HRESULT model::draw(_In_ const VkCommandBuffer& pCommandBuffer, _In_ VkDescriptorSet* pDescriptorSet)
{
    return S_OK;
}

ULONG model::release()
{
    if (this->get_is_released()) return 0;


    return _super::release();
}