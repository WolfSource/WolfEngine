#include "pch.h"
#include "model.h"

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

model::model() : 
    _mesh(nullptr),
    _shader(nullptr),
    _loaded(false)
{
    //define vertex binding attributes
    this->_vertex_binding_attributes.declaration = w_vertex_declaration::USER_DEFINED;
    this->_vertex_binding_attributes.binding_attributes[0] = { Vec3, Vec3, Vec2 }; //position, normal, uv per vertex
    this->_vertex_binding_attributes.binding_attributes[1] = { Vec3, Vec3 }; // position, rotation per instance
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
    
    this->_gDevice = pGDevice;

    const std::string _trace = this->name + "model::load";

    //get trasform of base object and instances objects
    this->_transform = pCPModel->get_transform();
    pCPModel->get_instances(this->_instances_transforms);
    
    //get full name 
    this->_full_name = pCPModel->get_name();
    get_searchable_name(this->_full_name);
    //get instances names
    for (auto& _iter : this->_instances_transforms)
    {
        get_searchable_name(_iter.name);
    }
    
    uint32_t _lod_distance_index = 1;
    const uint32_t _lod_distance_offset = 700;

    //create one big vertex buffer and index buffer from root model and LODs
    std::vector<float> _batch_vertices;
    std::vector<uint32_t> _batch_indices;

    //first store vertice and indices of root model
    std::vector<w_cpipeline_model::w_mesh*> _model_meshes;
    pCPModel->get_meshes(_model_meshes);

    size_t _sub_meshes_count = _model_meshes.size();
    uint32_t _base_vertex = 0;
    
    //we need original bounding sphere for camera focus
    if (_sub_meshes_count)
    {
        this->_root_bounding_sphere.create_from_bounding_box(_model_meshes[0]->bounding_box);
        this->_root_bounding_sphere.center[0] = this->_transform.position[0];
        this->_root_bounding_sphere.center[1] = this->_transform.position[1];
        this->_root_bounding_sphere.center[2] = this->_transform.position[2];
    }

    //generate masked occlusion culling data
    auto _size = pCPModel->get_convex_hulls_count();
    if (_size)
    {
        std::vector<w_cpipeline_model*> _chs;
        pCPModel->get_convex_hulls(_chs);

        for (auto& _iter : _chs)
        {
            //generate vertices and indices of bounding box
            _add_data_for_masked_occlusion_culling(_iter);
        }
    }
    else if (_sub_meshes_count)
    {
        //generate vertices and indices of bounding box
        _model_meshes[0]->bounding_box.generate_vertices_indices();
        _add_data_for_masked_occlusion_culling(_model_meshes[0]->bounding_box);
    }
    
    if (_sub_meshes_count)
    {
        //create first lod information
        lod _lod;
        _lod.first_index = _batch_indices.size();// First index for this LOD
        _lod.index_count = _model_meshes[0]->indices.size();// Index count for this LOD
        _lod.distance = _lod_distance_index * _lod_distance_offset;
        _lod_distance_index++;

        this->_lod_levels.push_back(_lod);

        _store_to_batch(_model_meshes, _batch_vertices, _batch_indices, _base_vertex);

        //load texture
        w_texture::load_to_shared_textures(this->_gDevice,
            content_path + L"textures/areas/" +
            wolf::system::convert::string_to_wstring(_model_meshes[0]->textures_path), &fs.texture);
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
        V(S_FALSE, "allocating memory of mesh for " + this->_full_name, _trace);
        return S_FALSE;
    }

    auto _v_size = static_cast<uint32_t>(_batch_vertices.size());
    auto _hr = _mesh->load(this->_gDevice,
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

    if (_load_buffers() == S_FALSE) return S_FALSE;
    if (_load_shader() == S_FALSE) return S_FALSE;
    if (_load_pipelines(pRenderPass) == S_FALSE) return S_FALSE;   
    if (_build_compute_command_buffers() == S_FALSE) return S_FALSE;

    VkSemaphoreCreateInfo _semaphore_create_info = {};
    _semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(_gDevice->vk_device,
        &_semaphore_create_info,
        nullptr,
        &this->cs.semaphore))
    {
        logger.error("Error on creating semaphore for compute command buffer");
        release();
        exit(1);
    }

    this->_loaded.store(true);

    return S_OK;
}

void model::get_searchable_name(_In_z_ const std::string& pName)
{
    std::string _search_name;
    std::vector<std::string> _splits;
    wolf::system::convert::split_string(pName, "_", _splits);
    for (size_t i = 1; i < _splits.size(); i++)
    {
        _search_name += _splits[i];
    }
    _splits.clear();

    std::transform(_search_name.begin(), _search_name.end(), _search_name.begin(), ::tolower);
    this->_search_names.push_back(_search_name);
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
            auto _nor = _data.normal;
            auto _uv = _data.uv;
            
            //position
            pBatchVertices.push_back(_pos[0]);
            pBatchVertices.push_back(_pos[1]);
            pBatchVertices.push_back(_pos[2]);

            //normal
            pBatchVertices.push_back(_nor[0]);
            pBatchVertices.push_back(_nor[1]);
            pBatchVertices.push_back(_nor[2]);

            //uv
            pBatchVertices.push_back(_uv[0]);
            pBatchVertices.push_back(1 - _uv[1]);

            pBaseVertex++;
        }
    }
}

void model::_add_data_for_masked_occlusion_culling(_In_ const w_bounding_box& pBoundingBox)
{
    moc_data _moc_data;

    auto _bb_vertices_size = pBoundingBox.vertices.size();
    uint32_t _index = 0;
    clipspace_vertex _cv;
    for (size_t i = 0; i < _bb_vertices_size; i += 3)
    {
        _cv.x = pBoundingBox.vertices[i + 0];
        _cv.y = pBoundingBox.vertices[i + 1];
        _cv.z = 0;
        _cv.w = pBoundingBox.vertices[i + 2];

        _moc_data.vertices.push_back(_cv);
        _moc_data.indices.push_back(_index++);
    }
    _moc_data.position.x = pBoundingBox.position[0];
    _moc_data.position.y = pBoundingBox.position[1];
    _moc_data.position.z = pBoundingBox.position[2];

    _moc_data.rotation.x = pBoundingBox.rotation[0];
    _moc_data.rotation.y = pBoundingBox.rotation[1];
    _moc_data.rotation.z = pBoundingBox.rotation[2];

    _moc_data.num_of_tris_for_moc = static_cast<int>(_bb_vertices_size / 9);
    this->_mocs.push_back(_moc_data);
}

void model::_add_data_for_masked_occlusion_culling(_In_ w_cpipeline_model* pConvexHull)
{
    
    auto _size = pConvexHull->get_meshes_count();
    if (!_size) return;
    
    std::vector<w_cpipeline_model::w_mesh*> _meshes;
    pConvexHull->get_meshes(_meshes);

    auto _transform = pConvexHull->get_transform();
    auto _pos = _transform.position;
    auto _rot = _transform.rotation;

    for (auto& _iter : _meshes)
    {
        moc_data _moc_data;

        clipspace_vertex _cv;
        auto _vert_size = _iter->vertices.size();
        for (uint32_t i = 0; i < _vert_size; i++)
        {
            auto _vertex_pos = _iter->vertices[i].position;
            _cv.x = _vertex_pos[0];
            _cv.y = _vertex_pos[1];
            _cv.z = 0;
            _cv.w = _vertex_pos[2];

            _moc_data.vertices.push_back(_cv);
            //_moc_data.indices.push_back(i);
        }

        _size = _iter->indices.size();
        for (uint32_t i = 0; i < _size; i++)
        {
            _moc_data.indices.push_back(_iter->indices[i]);
        }

        auto _pos = _iter->bounding_box.position;
        auto _rot = _iter->bounding_box.rotation;

        _moc_data.position.x = _pos[0];
        _moc_data.position.y = _pos[1];
        _moc_data.position.z = _pos[2];

        _moc_data.rotation.x = _rot[0];
        _moc_data.rotation.y = _rot[1];
        _moc_data.rotation.z = _rot[2];

        _moc_data.num_of_tris_for_moc = static_cast<int>(_vert_size / 3);
        this->_mocs.push_back(_moc_data);
    }
}


HRESULT model::_load_shader()
{
    const std::string _trace = this->name + "::_load_shader";

    //load vertex shader uniform
    if (this->vs.unifrom.load(this->_gDevice) == S_FALSE)
    {
        V(S_FALSE, "loading vertex shader uniform for " + this->_full_name, _trace);
        return S_FALSE;
    }

    //load fragment shader uniform
    if (this->fs.unifrom.load(this->_gDevice) == S_FALSE)
    {
        V(S_FALSE, "loading fragment shader uniform for " + this->_full_name, _trace);
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
    _param.image_info = this->fs.texture == nullptr ? w_texture::default_texture->get_descriptor_info() :
        this->fs.texture->get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 2;
    _param.type = w_shader_binding_type::UNIFORM;
    _param.stage = w_shader_stage::FRAGMENT_SHADER;
    _param.buffer_info = this->fs.unifrom.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 0;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = this->cs.instance_buffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 1;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = this->indirect.indirect_draw_commands_buffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 2;
    _param.type = w_shader_binding_type::UNIFORM;
    _param.stage = w_shader_stage::COMPUTE_SHADER;

    //load compute uniform then assign it to shader
    switch (this->cs.batch_local_size)
    {
    default:
        V(S_FALSE, "batch_local_size " + std::to_string(this->cs.batch_local_size) + " not supported " + this->_full_name, _trace);
        return S_FALSE;
    case 1:
        this->_visibilities.resize(1);
        this->cs.unifrom_x1 = new w_uniform<compute_unifrom_x1>();
        if (this->cs.unifrom_x1->load(this->_gDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader uniform_x1 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x1->get_descriptor_info();
        break;
    case 2:
        this->_visibilities.resize(2);
        this->cs.unifrom_x2 = new w_uniform<compute_unifrom_x2>();
        if (this->cs.unifrom_x2->load(this->_gDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x2 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x2->get_descriptor_info();
        break;
    case 4:
        this->_visibilities.resize(4);
        this->cs.unifrom_x4 = new w_uniform<compute_unifrom_x4>();
        if (this->cs.unifrom_x4->load(this->_gDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x4 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x4->get_descriptor_info();
        break;
    case 8:
        this->_visibilities.resize(8);
        this->cs.unifrom_x8 = new w_uniform<compute_unifrom_x8>();
        if (this->cs.unifrom_x8->load(this->_gDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x8 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x8->get_descriptor_info();
        break;
    case 16:
        this->_visibilities.resize(16);
        this->cs.unifrom_x16 = new w_uniform<compute_unifrom_x16>();
        if (this->cs.unifrom_x16->load(this->_gDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x16 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x16->get_descriptor_info();
        break;
    case 32:
        this->_visibilities.resize(32);
        this->cs.unifrom_x32 = new w_uniform<compute_unifrom_x32>();
        if (this->cs.unifrom_x32->load(this->_gDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x32 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x32->get_descriptor_info();
        break;
    case 64:
        this->_visibilities.resize(64);
        this->cs.unifrom_x64 = new w_uniform<compute_unifrom_x64>();
        if (this->cs.unifrom_x64->load(this->_gDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader unifrom_x64 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x64->get_descriptor_info();
        break;
    case 128:
        this->_visibilities.resize(128);
        this->cs.unifrom_x128 = new w_uniform<compute_unifrom_x128>();
        if (this->cs.unifrom_x128->load(this->_gDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader uniform_x128 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x128->get_descriptor_info();
        break;
    case 256:
        this->_visibilities.resize(256);
        this->cs.unifrom_x256 = new w_uniform<compute_unifrom_x256>();
        if (this->cs.unifrom_x256->load(this->_gDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader uniform_x256 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x256->get_descriptor_info();
        break;
    case 512:
        this->_visibilities.resize(512);
        this->cs.unifrom_x512 = new w_uniform<compute_unifrom_x512>();
        if (this->cs.unifrom_x512->load(this->_gDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader uniform_x512 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x512->get_descriptor_info();
        break;
    case 1024:
        this->_visibilities.resize(1024);
        this->cs.unifrom_x1024 = new w_uniform<compute_unifrom_x1024>();
        if (this->cs.unifrom_x1024->load(this->_gDevice) == S_FALSE)
        {
            V(S_FALSE, "loading compute shader uniform_x512 for " + this->_full_name, _trace);
            return S_FALSE;
        }
        _param.buffer_info = this->cs.unifrom_x1024->get_descriptor_info();
        break;
    }
    _shader_params.push_back(_param);

    _param.index = 3;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = this->indirect.indirect_draw_count_buffer.get_descriptor_info();
    _shader_params.push_back(_param);

    _param.index = 4;
    _param.type = w_shader_binding_type::STORAGE;
    _param.stage = w_shader_stage::COMPUTE_SHADER;
    _param.buffer_info = this->cs.lod_levels_buffers.get_descriptor_info();
    _shader_params.push_back(_param);

    //check path of shader
    auto _lod_level = this->_lod_levels.size() ? this->_lod_levels.size() - 1 : 0;
    auto _shader_name = "cull_lod_" + std::to_string(_lod_level) + "_local_size_x" + std::to_string(cs.batch_local_size) + ".comp.spv";
    auto _compute_shader_path = content_path + L"shaders/compute/" + wolf::system::convert::string_to_wstring(_shader_name);
    if (wolf::system::io::get_is_file(_compute_shader_path.c_str()) == S_FALSE)
    {
        V(S_FALSE, L"compute shader not exists on path " + _compute_shader_path, _trace);
        return S_FALSE;
    }

    //load shaders
    if (w_shader::load_shader(
        this->_gDevice,
        _shader_name,
        content_path + L"shaders/compute/indirect_draw.vert.spv",
        L"",
        L"",
        L"",
        content_path + L"shaders/compute/indirect_draw.frag.spv",
        _compute_shader_path,
        _shader_params,
        false,
        &this->_shader) == S_FALSE)
    {
        V(S_FALSE, "Loading shader for " + this->_full_name, _trace);
        return S_FALSE;
    }

    return S_OK;
}

HRESULT model::_load_buffers()
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
    this->cs.batch_local_size = static_cast<uint32_t>(pow(2, ceil(log(_draw_counts) / log(2))));
    this->indirect.indirect_draw_commands.resize(cs.batch_local_size);
    
    this->indirect_status.draw_count = _draw_counts;

    for (uint32_t i = 0; i < _draw_counts; ++i)
    {
        this->indirect.indirect_draw_commands[i].instanceCount = 1;
        this->indirect.indirect_draw_commands[i].firstInstance = i;
        // firstIndex and indexCount are written by the compute shader
    }

    uint32_t _size = (uint32_t)(_draw_counts * sizeof(VkDrawIndexedIndirectCommand));
    if (_staging_buffers[0].load_as_staging(this->_gDevice, _size) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of indirect_draw_commands", _trace, 3);
        return S_FALSE;
    }
    
    if (_staging_buffers[0].bind() == S_FALSE)
    {
        V(S_FALSE, "binding to staging buffer of indirect_draw_commands", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[0].set_data(this->indirect.indirect_draw_commands.data()) == S_FALSE)
    {
        V(S_FALSE, "setting data for staging buffer of indirect_draw_commands", _trace, 3);
        return S_FALSE;
    }

    if (this->indirect.indirect_draw_commands_buffer.load(
        this->_gDevice,
        _size,
        VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading staging buffer of indirect_commands_buffer", _trace, 3);
        return S_FALSE;
    }

    if (this->indirect.indirect_draw_commands_buffer.bind() == S_FALSE)
    {
        V(S_FALSE, "binding to staging buffer of indirect_commands_buffer", _trace, 3);
        return S_FALSE;
    }

    if (_staging_buffers[0].copy_to(this->indirect.indirect_draw_commands_buffer) == S_FALSE)
    {
        V(S_FALSE, "copy staging buffer to device buffer of indirect_commands_buffer", _trace, 3);
        return S_FALSE;
    }

    _size = (uint32_t)sizeof(this->indirect_status);
    if (this->indirect.indirect_draw_count_buffer.load(
        this->_gDevice,
        _size,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == S_FALSE)
    {
        V(S_FALSE, "loading device buffer of indirect_draw_count", _trace, 3);
        return S_FALSE;
    }
    
    if (this->indirect.indirect_draw_count_buffer.bind() == S_FALSE)
    {
        V(S_FALSE, "binding to device buffer of indirect_draw_count", _trace, 3);
        return S_FALSE;
    }

    std::vector<vertex_instance_data> _vertex_instances_data(_draw_counts);
    //first one is root model
    _vertex_instances_data[0].pos[0] = this->_transform.position[0];
    _vertex_instances_data[0].pos[1] = this->_transform.position[1];
    _vertex_instances_data[0].pos[2] = this->_transform.position[2];
    
    _vertex_instances_data[0].rot[0] = this->_transform.rotation[0];
    _vertex_instances_data[0].rot[1] = this->_transform.rotation[1];
    _vertex_instances_data[0].rot[2] = this->_transform.rotation[2];
        
    //others are instances
    _size = static_cast<uint32_t>(this->_instances_transforms.size());

    //resize world view projections of root and other instances
    for (size_t i = 0; i <  _size; ++i)
    {
        auto _index = i + 1;
        _vertex_instances_data[_index].pos[0] = this->_instances_transforms[i].position[0];
        _vertex_instances_data[_index].pos[1] = this->_instances_transforms[i].position[1];
        _vertex_instances_data[_index].pos[2] = this->_instances_transforms[i].position[2];

        _vertex_instances_data[_index].rot[0] = this->_instances_transforms[i].rotation[0];
        _vertex_instances_data[_index].rot[1] = this->_instances_transforms[i].rotation[1];
        _vertex_instances_data[_index].rot[2] = this->_instances_transforms[i].rotation[2];
    }

    _size = (uint32_t)(_vertex_instances_data.size() * sizeof(vertex_instance_data));
    if (_staging_buffers[1].load_as_staging(this->_gDevice, _size) == S_FALSE)
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

    if (vs.instance_buffer.load(this->_gDevice,
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

    _size = static_cast<uint32_t>(this->_lod_levels.size() * sizeof(lod));
    if (_staging_buffers[2].load_as_staging(this->_gDevice, _size) == S_FALSE)
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
        this->_gDevice,
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

    std::vector<compute_instance_data> _compute_instance_data(_draw_counts);
    for (size_t i = 0; i < _draw_counts; i++)
    {
        _compute_instance_data[i].pos = glm::vec4(_vertex_instances_data[i].pos, 0.0f);
    }

    _size = static_cast<uint32_t>(_compute_instance_data.size() * sizeof(compute_instance_data));
    if (_staging_buffers[3].load_as_staging(this->_gDevice, _size) == S_FALSE)
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

    if (cs.instance_buffer.load(this->_gDevice,
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

HRESULT model::_load_pipelines(_In_ w_render_pass& pRenderPass)
{
    const std::string _trace = this->name + "::_load_pipelines";

    std::vector<VkDynamicState> _dynamic_states =
    {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    auto _descriptor_set_layout_binding = this->_shader->get_descriptor_set_layout();

    auto _hr = vs.pipeline.load(
        this->_gDevice,
        this->_vertex_binding_attributes,
        VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        pRenderPass.get_handle(),
        _shader->get_shader_stages(),
        &_descriptor_set_layout_binding,
        { pRenderPass.get_viewport() }, //viewports
        { pRenderPass.get_viewport_scissor() }, //viewports scissor
        _dynamic_states,
        "model_pipeline_cache",
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

    if (this->cs.pipeline.load_compute(
        this->_gDevice,
        _compute_shader_stage,
        _compute_descriptor_set_layout_binding,
        6,
        "model_pipeline_cache") == S_FALSE)
    {
        V(S_FALSE, "loading compute pipeline for " + this->_full_name, _trace);
        return S_FALSE;
    }
    
    return S_OK;
}

HRESULT model::_build_compute_command_buffers()
{
    const std::string _trace = this->name + "::_build_compute_command_buffers";

    auto _hr = this->cs.command_buffers.load(
        this->_gDevice,
        1,
        VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        true,
        &_gDevice->vk_compute_queue);
    if (_hr == S_FALSE)
    {
        V(S_FALSE, "creating compute command buffer for " + this->_full_name, _trace);
        return S_FALSE;
    }

    auto _cmd = this->cs.command_buffers.get_command_at(0);

    VkCommandBufferBeginInfo _cmd_buffer_info = {};
    _cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(_cmd, &_cmd_buffer_info);

    auto _cmd_handle = this->indirect.indirect_draw_commands_buffer.get_handle();
    auto _size = this->indirect.indirect_draw_commands_buffer.get_descriptor_info().range;

    // Add memory barrier to ensure that the indirect commands have been consumed before the compute shader updates them
    VkBufferMemoryBarrier _buffer_barrier = {};
    _buffer_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
    _buffer_barrier.buffer = _cmd_handle;
    _buffer_barrier.size = _size;
    _buffer_barrier.srcAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    _buffer_barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    _buffer_barrier.srcQueueFamilyIndex = this->_gDevice->vk_graphics_queue.index;
    _buffer_barrier.dstQueueFamilyIndex = this->_gDevice->vk_compute_queue.index;

    vkCmdPipelineBarrier(
        _cmd,
        VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        0,
        0, nullptr,
        1, &_buffer_barrier,
        0, nullptr);

    auto _pipline_layout_handle = this->cs.pipeline.get_layout_handle();
    auto _desciptor_set = this->_shader->get_compute_descriptor_set();
    vkCmdBindPipeline(_cmd, VK_PIPELINE_BIND_POINT_COMPUTE, this->cs.pipeline.get_handle());
    vkCmdBindDescriptorSets(
        _cmd,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        _pipline_layout_handle,
        0,
        1, &_desciptor_set,
        0,
        0);

    vkCmdDispatch(_cmd, (uint32_t)(this->indirect.indirect_draw_commands.size() / cs.batch_local_size), 1, 1);

    // Add memory barrier to ensure that the compute shader has finished writing the indirect command buffer before it's consumed
    _buffer_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    _buffer_barrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
    _buffer_barrier.buffer = _cmd_handle;
    _buffer_barrier.size = _size;
    _buffer_barrier.srcQueueFamilyIndex = this->_gDevice->vk_compute_queue.index;
    _buffer_barrier.dstQueueFamilyIndex = this->_gDevice->vk_graphics_queue.index;

    vkCmdPipelineBarrier(
        _cmd,
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
        0,
        0, nullptr,
        1, &_buffer_barrier,
        0, nullptr);

    vkEndCommandBuffer(_cmd);

    return S_OK;
}

void model::pre_update(
    _In_    w_first_person_camera pCamera,
    _Inout_ MaskedOcclusionCulling** sMOC)
{
    if (!this->_loaded.load()) return;

    using namespace glm;
    this->_view_projection = pCamera.get_projection_view();

    glm::mat4 _model_to_clip_matrix;
    //render bounding boxes of root model to Masked Occlusion culling
    for (auto& _iter : this->_mocs)
    {
        //world view projection for bounding box of masked occlusion culling
        _model_to_clip_matrix = this->_view_projection * 
            glm::translate(_iter.position) * 
            glm::rotate(_iter.rotation);

        (*sMOC)->RenderTriangles(
            (float*)&_iter.vertices[0],
            _iter.indices.data(),
            _iter.num_of_tris_for_moc,
            (float*)(&_model_to_clip_matrix[0]));
    }

    size_t _index = 1;
    glm::vec3 _pos, _rot, _dif_pos, _dif_rot;
    for (auto& _ins : this->_instances_transforms)
    {
        _pos = glm::vec3(
            _ins.position[0],
            _ins.position[1],
            _ins.position[2]);

        _rot = glm::vec3(
            _ins.rotation[0],
            _ins.rotation[1],
            _ins.rotation[2]);

        //render all bounding boxes of instances to masked occlusion culling
        for (auto& _iter : this->_mocs)
        {
            if (this->_mocs.size() == 1)
            {
                //use transform of instance model
                _model_to_clip_matrix = _view_projection * glm::translate(_pos) * glm::rotate(_rot);
            }
            else
            {
                //find the difference from transform of instance and root
                _dif_pos = _pos - glm::vec3(this->_transform.position[0], this->_transform.position[1], this->_transform.position[2]);
                _dif_rot = _rot - glm::vec3(this->_transform.rotation[0], this->_transform.rotation[1], this->_transform.rotation[2]);
                _model_to_clip_matrix = _view_projection * glm::translate(_iter.position + _dif_pos) * glm::rotate(_iter.rotation + _dif_rot);
            }

            (*sMOC)->RenderTriangles(
                (float*)&_iter.vertices[0],
                _iter.indices.data(),
                _iter.num_of_tris_for_moc,
                (float*)(&_model_to_clip_matrix[0]));
        }
    }
}

bool model::post_update(
    _Inout_ MaskedOcclusionCulling* sMOC)
{
    bool _add_to_render_queue = false;
    if (!this->_loaded.load()) return _add_to_render_queue;

    std::fill(this->_visibilities.begin(), this->_visibilities.end(), 0.0f);

    glm::mat4 _model_to_clip_matrix;
    //check bounding boxes of root model from Masked Occlusion culling
    MaskedOcclusionCulling::CullingResult _culling_result;
    for (auto& _iter : this->_mocs)
    {
        _model_to_clip_matrix = this->_view_projection * glm::translate(_iter.position) * glm::rotate(_iter.rotation);
        _culling_result = sMOC->TestTriangles(
            (float*)&_iter.vertices[0],
            _iter.indices.data(),
            _iter.num_of_tris_for_moc,
            (float*)(&_model_to_clip_matrix[0]));

        //if at least one of the bounding boxes is visible, break this loop
        if (_culling_result == MaskedOcclusionCulling::VISIBLE)
        {
            this->_visibilities[0] = true;
            _add_to_render_queue = true;
            break;
        }
    }

    //check all instnaces
    glm::vec3 _pos, _rot, _dif;
    for (size_t i = 0; i < this->_instances_transforms.size(); ++i)
    {
        _pos = glm::vec3(
            this->_instances_transforms[i].position[0],
            this->_instances_transforms[i].position[1],
            this->_instances_transforms[i].position[2]);

        _rot = glm::vec3(
            this->_instances_transforms[i].rotation[0],
            this->_instances_transforms[i].rotation[1],
            this->_instances_transforms[i].rotation[2]);

        for (auto& _iter : this->_mocs)
        {
            if (this->_mocs.size() == 1)
            {
                //use transform of instance model
                _model_to_clip_matrix = _view_projection * glm::translate(_pos) * glm::rotate(_rot);
            }
            else
            {
                //find the difference from transform of instance and root
                _dif = _pos - glm::vec3(this->_transform.position[0], this->_transform.position[1], this->_transform.position[2]);
                _model_to_clip_matrix = _view_projection * glm::translate(_iter.position + _dif) * glm::rotate(_rot);
            }

            _culling_result = sMOC->TestTriangles(
                (float*)&_iter.vertices[0],
                _iter.indices.data(),
                _iter.num_of_tris_for_moc,
                (float*)(&_model_to_clip_matrix[0]));

            if (_culling_result == MaskedOcclusionCulling::VISIBLE)
            {
                this->_visibilities[i + 1] = true;
                _add_to_render_queue = true;
                break;
            }
        }
    }

    return _add_to_render_queue;
}

void model::indirect_draw(_In_ const VkCommandBuffer& pCommandBuffer)
{
    auto _descriptor_set = this->_shader->get_descriptor_set();
    this->vs.pipeline.bind(pCommandBuffer, &_descriptor_set);

    auto _instances_size = (uint32_t)this->_instances_transforms.size();
    _mesh->draw(pCommandBuffer, this->vs.instance_buffer.get_handle(), _instances_size, true);

    if (this->_gDevice->vk_physical_device_features.multiDrawIndirect)
    {
        vkCmdDrawIndexedIndirect(
            pCommandBuffer,
            this->indirect.indirect_draw_commands_buffer.get_handle(),
            0,
            this->indirect_status.draw_count,
            sizeof(VkDrawIndexedIndirectCommand));
    }
    else
    {
        // If multi draw is not available, we must issue separate draw commands
        for (auto j = 0; j < this->indirect.indirect_draw_commands.size(); j++)
        {
            vkCmdDrawIndexedIndirect(
                pCommandBuffer,
                this->indirect.indirect_draw_commands_buffer.get_handle(),
                j * sizeof(VkDrawIndexedIndirectCommand),
                1, sizeof(VkDrawIndexedIndirectCommand));
        }
    }

}

HRESULT model::submit_compute_shader(_In_ const wolf::content_pipeline::w_first_person_camera* pCamera)
{
    HRESULT _hr = S_OK;

    if (!this->_loaded.load()) return _hr;

    const std::string _trace = this->name + "::draw";

    //Update uniforms
    auto _camera_pos = pCamera->get_translate();

    this->vs.unifrom.data.view = pCamera->get_view();
    this->vs.unifrom.data.projection = pCamera->get_projection();

    if (this->vs.unifrom.update() == S_FALSE)
    {
        _hr = S_FALSE;
        V(_hr, "updating vertex shader unifrom", _trace, 3);
    }
    if (this->fs.unifrom.update() == S_FALSE)
    {
        _hr = S_FALSE;
        V(_hr, "updating fragment shader unifrom", _trace, 3);
    }

    switch (this->cs.batch_local_size)
    {
    case 1:
        this->cs.unifrom_x1->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x1->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x1->data.is_visible));
        _hr = this->cs.unifrom_x1->update();
        break;
    case 2:
        this->cs.unifrom_x2->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x2->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x2->data.is_visible));
        _hr = this->cs.unifrom_x2->update();
        break;
    case 4:
        this->cs.unifrom_x4->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x4->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x4->data.is_visible));
        _hr = this->cs.unifrom_x4->update();
        break;
    case 8:
        this->cs.unifrom_x8->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x8->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x8->data.is_visible));
        _hr = this->cs.unifrom_x8->update();
        break;
    case 16:
        this->cs.unifrom_x16->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x16->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x16->data.is_visible));
        _hr = this->cs.unifrom_x16->update();
        break;
    case 32:
        this->cs.unifrom_x32->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x32->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x32->data.is_visible));
        _hr = this->cs.unifrom_x32->update();
        break;
    case 64:
        this->cs.unifrom_x64->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x64->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x64->data.is_visible));
        _hr = this->cs.unifrom_x64->update();
        break;
    case 128:
        this->cs.unifrom_x128->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x128->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x128->data.is_visible));
        _hr = this->cs.unifrom_x128->update();
        break;
    case 256:
        this->cs.unifrom_x256->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x256->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x256->data.is_visible));
        _hr = this->cs.unifrom_x256->update();
        break;
    case 512:
        this->cs.unifrom_x512->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x512->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x512->data.is_visible));
        _hr = this->cs.unifrom_x512->update();
        break;
    case 1024:
        this->cs.unifrom_x1024->data.camera_pos = glm::vec4(_camera_pos, 1.0f);
        std::memcpy(&this->cs.unifrom_x1024->data.is_visible[0],
            this->_visibilities.data(), sizeof(this->cs.unifrom_x1024->data.is_visible));
        _hr = this->cs.unifrom_x1024->update();
        break;
    }

    if (_hr == S_FALSE)
    {
        V(_hr, "updating compute shader's unifrom", _trace, 3);
    }

    auto _cmd = this->cs.command_buffers.get_command_at(0);

    VkSubmitInfo _submit_info = {};
    _submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    _submit_info.commandBufferCount = 1;
    _submit_info.pCommandBuffers = &_cmd;
    _submit_info.signalSemaphoreCount = 1;
    _submit_info.pSignalSemaphores = &this->cs.semaphore;

    //execute compute shader on gpu
    if (vkQueueSubmit(this->_gDevice->vk_compute_queue.queue, 1, &_submit_info, 0))
    {
        _hr = S_FALSE;
        V(_hr, "submiting queu for " + this->_full_name, _trace, 3);
    }

    return _hr;
}

ULONG model::release()
{
    if (_super::get_is_released()) return 0;

    auto _device = _gDevice->vk_device;
    if (this->cs.semaphore)
    {
        vkDestroySemaphore(_device, this->cs.semaphore, nullptr);
        this->cs.semaphore = 0;
    }

    this->cs.release();
    this->vs.release();
    this->fs.release();
    this->indirect.release();
    this->_instances_transforms.clear();
    this->_lod_levels.clear();
    this->_search_names.clear();
    this->_visibilities.clear();

    //release all masked occlusion data
    for (auto& _iter : this->_mocs)
    {
        _iter.release();
    }
    this->_mocs.clear();
    
    SAFE_RELEASE(this->_mesh);
    
    this->_gDevice = nullptr;

    return _super::release();
}

void model::search_for_name(
    _In_z_ const std::string& pToBeFind,
    _Inout_ std::vector<search_item_struct>& pResults)
{
    set_color(glm::vec4(0.7f));

    search_item_struct _item;
    for (uint32_t i = 0; i < this->_search_names.size(); ++i)
    {
        if (strstr(this->_search_names[i].c_str(), pToBeFind.c_str()))
        {
            //TODO: we need to set opacity for each instance not all instanced
            set_color(glm::vec4(1.0f));
            
            _item.name = this->_search_names[i];
            _item.bounding_sphere = this->_root_bounding_sphere;
            if (i != 0)
            {
                _item.bounding_sphere.center[0] = this->_instances_transforms[i - 1].position[0];
                _item.bounding_sphere.center[1] = this->_instances_transforms[i - 1].position[1];
                _item.bounding_sphere.center[2] = this->_instances_transforms[i - 1].position[2];
            }

            pResults.push_back(_item);
        }
    }
}

#pragma region Getters

const char*  model::get_full_name() const
{
    return this->_full_name.c_str();
}

glm::vec3 model::get_position() const
{
    return glm::vec3(
        this->_transform.position[0],
        this->_transform.position[1],
        this->_transform.position[2]);
}

VkSemaphore  model::get_compute_semaphore() const
{
    return this->cs.semaphore;
}

#pragma endregion

#pragma region Setters

void model::set_opacity(const float& pValue)
{
    this->fs.unifrom.data.color.a = pValue;
}

void model::set_color(const glm::vec4& pValue)
{
    this->fs.unifrom.data.color.r = pValue.r;
    this->fs.unifrom.data.color.g = pValue.g;
    this->fs.unifrom.data.color.b = pValue.b;
    this->fs.unifrom.data.color.a = pValue.a;
}

#pragma endregion