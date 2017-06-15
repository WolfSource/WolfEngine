#include "pch.h"
#include "model.h"

using namespace wolf::system;
using namespace wolf::content_pipeline;

#define COMPUE_SHADER_LOCAL_SIZE_X_ 1

model::model()
{
}

model::~model()
{
    release();
}

HRESULT model::load(
    _In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice, 
    _In_ w_cpipeline_model* pCPModel,
    _In_ std::vector<w_cpipeline_model*> pCPModelLODs)
{
    if (!pGDevice || !pCPModel) return S_FALSE;

    uint32_t _base_index = 0;
    uint32_t _base_vertex = 0;

    //get trasform of base object and instances objects
    this->_transform = pCPModel->get_transform();
    pCPModel->get_instances(_instances_transforms);

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

    uint32_t n = 1;

    //create one big vertex buffer and index buffer from root model and LODs
    std::vector<float> _big_one_vertices;
    std::vector<uint32_t> _big_one_indices;

    //std::vector<size_t> _order_of_lods = { 0, 0, 0 };
    //std::vector<size_t> _insts = { 1, 2, 3, 4, 5 };
    //for (auto& _iter : _order_of_lods)
    //{
    //    auto _model = _cmodels[_iter];
    //    //load meshes
    //    std::vector<w_cpipeline_model::w_mesh*> _model_meshes;
    //    _model->get_meshes(_model_meshes);

    //    for (auto& _mesh_data : _model_meshes)
    //    {
    //        _min_bb = glm::vec4(
    //            _mesh_data->bounding_box.min[0],
    //            _mesh_data->bounding_box.min[1],
    //            _mesh_data->bounding_box.min[2],
    //            0);

    //        _max_bb = glm::vec4(
    //            _mesh_data->bounding_box.max[0],
    //            _mesh_data->bounding_box.max[1],
    //            _mesh_data->bounding_box.max[2],
    //            0);

    //        _base_index = _indices.size();

    //        std::string _inds;
    //        for (size_t i = 0; i < _mesh_data->indices.size(); ++i)
    //        {
    //            _indices.push_back(_base_vertex + _mesh_data->indices[i]);
    //            _inds += std::to_string(_indices[i]) + ", ";
    //        }

    //        LOD lod;
    //        lod.firstIndex = _base_index;			        // First index for this LOD
    //        lod.indexCount = _mesh_data->indices.size();	// Index count for this LOD
    //        lod.distance = n * 50.0f;				        // Starting distance (to viewer) for this LOD
    //        n++;
    //        LODLevels.push_back(lod);

    //        std::string _tris;
    //        _tris = "{";
    //        for (auto& _data : _mesh_data->vertices)
    //        {
    //            auto _pos = _data.position;
    //            auto _uv = _data.uv;

    //            //position
    //            _vertices.push_back(_pos[0]);
    //            _vertices.push_back(_pos[1]);
    //            _vertices.push_back(_pos[2]);

    //            //uv
    //            _vertices.push_back(_uv[0]);
    //            _vertices.push_back(1 - _uv[1]);

    //            _base_vertex++;

    //            _tris += "{ " + std::to_string(_pos[0]) + ", " + std::to_string(_pos[1]) + ",0.0f, " + std::to_string(_pos[2]) + "},";
    //        }

    //        _tris += "}";

    //        logger.write(_tris);
    //        logger.write("NULL");
    //        logger.write(_inds);
    //    }
    //}

    //_mesh = new (std::nothrow) wolf::graphics::w_mesh();
    //if (!_mesh)
    //{
    //    logger.error("Error on allocating memory for mesh");
    //    w_game::exit();
    //}
    //auto _v_size = static_cast<uint32_t>(_vertices.size());
    //_hr = _mesh->load(_gDevice,
    //    _vertices.data(),
    //    _v_size * sizeof(float),
    //    _v_size,
    //    _indices.data(),
    //    _indices.size());
    //_mesh->set_vertex_binding_attributes(_vertex_binding_attrs);

    //_vertices.clear();
    //_indices.clear();

    //if (_hr == S_FALSE)
    //{
    //    logger.error("Error on loading vertices for mesh");
    //    w_game::exit();
    //}

    return S_OK;
}

HRESULT model::_prepare_buffers(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice)
{
    return S_OK;
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