#include "w_cpipeline_pch.h"
#include "w_cpipeline_model.h"

//#ifdef __WIN32
#include "amd/amd_tootle.h"
//#endif

using namespace std;
using namespace wolf::system;
using namespace wolf::content_pipeline;
using namespace wolf::content_pipeline::collada;

w_cpipeline_model::w_cpipeline_model() :
	_name("unknown"),
	_animation_time(0),
	_last_animation_time(0),
	_frame_overlap(0),
	_overlapping(false),
	_overlapping_start_time(false)
{
}

w_cpipeline_model::~w_cpipeline_model()
{
}

w_cpipeline_model* w_cpipeline_model::create_model(
    _In_ c_geometry& pGeometry, 
    _In_ c_skin* pSkin,
	_In_ std::vector<c_bone*>& pBones, 
    _In_ string pBoneNames [], 
    _In_ std::map<std::string, std::string>& sLibraryMaterials,
    _In_ std::map<std::string, std::string>& sLibraryEffects,
    _In_ std::map<std::string, std::string>& sLibraryImages,
    _In_ const w_transform_info& pTransform,
    _In_ const bool& pAMDTootleOptimizing,
    _In_ const bool& pZUp,
    _In_ const bool& pInvertNormal)
{
	auto _model = new w_cpipeline_model();
    _model->set_transform(pTransform);
    _model->update_world();

	//std::string _messages;

    for (auto _triangle : pGeometry.triangles)
    {
        //find indices
        auto _pos_index = -1;
        auto _nor_index = -1;
        auto _tex_index = -1;
        auto _map_index = -1;
        auto _ind = 0;

        //find semantic resource index
        for (auto _sem : _triangle->semantics)
        {
            for (int i = 0; i < pGeometry.sources.size(); ++i)
            {
                if (pGeometry.sources[i] && pGeometry.sources[i]->c_id != _sem->source) continue;

                if (_pos_index == -1 && _sem->semantic == "POSITION")
                {
                    _pos_index = i;
                }
                else if (_nor_index == -1 && _sem->semantic == "NORMAL")
                {
                    _nor_index = i;
                }
                else if (_tex_index == -1 && _sem->semantic == "TEXCOORD")
                {
                    _tex_index = i;
                }
                else if (_map_index == -1 && _sem->semantic == "map")
                {
                    _map_index = i;
                }
                _ind++;
            }
        }


        //auto _joint_index = 0;
        //auto _weight_index = 0;
        //auto _weight_src = -1;
        if (pSkin != nullptr)
        {
            ////find transform and joint source index
            //for (size_t i = 0; i < pSkin->source.size(); ++i)
            //{
            //	auto _common = pSkin->sources[i]->technique_common;
            //	if (_common.param_name == "TRANSFORM")
            //	{
            //	}
            //	else if(_common.param_name == "JOINT")
            //	{
            //		_model->_bone_names.swap(pSkin->sources[i]->name_array);
            //	}
            //	if (_common.param_name == "NULL")
            //	{
            //		//if (_common.accessor_source.Contains("Joints"))
            //		//{
            //		//	m.BoneNames = s.Sources[i].NameArray.Array;
            //		//}
            //	}
            //}
            //if (s.VertexWeight != null)
            //{
            //	for (var i = 0; i < s.VertexWeight.Inputs.Count; i++)
            //	{
            //		switch (s.VertexWeight.Inputs[i].Semantic)
            //		{
            //		case "JOINT":
            //			jointIndex = i;
            //			break;
            //		case "WEIGHT":
            //			weightIndex = i;
            //			break;
            //		}
            //	}
            //}
            ////find vertex weight values index in skin sources
            //for (var i = 0; i < s.Sources.Count; i++)
            //{
            //	if (s.Sources[i].TechniqueCommon.ParamName == "WEIGHT" ||
            //		s.Sources[i].TechniqueCommon.AccessorSource.Contains("Weights"))
            //		weightSrc = i;
            //}
        }

        //sort bone names
        for (auto _bone : pBones)
        {
            for (auto i = 0; i < _bone->flat_bones.size(); ++i)
            {
                if (_bone->flat_bones[i] && _bone->flat_bones[i]->c_name != "null")
                {
                    _model->_bone_names.push_back(_bone->flat_bones[i]->c_name);
                }
            }
        }

#pragma region read vertices & indices

        //auto _ind2 = 0;
        //auto _ind3 = 0;
        auto _pos_source = pGeometry.sources[_pos_index];
        auto _tex_source = _tex_index != -1 ? pGeometry.sources[_tex_index] : nullptr;
        auto _nor_source = _nor_index != -1 ? pGeometry.sources[_nor_index] : nullptr;

        // uint32_t _faces = 0;
        //if (_ind)
        //{
        //    _faces = (static_cast<uint32_t>(_pos_source->float_array.size()) / _ind) - 2;
        //}

        std::vector<w_vertex_data> _vertices_data;
        std::vector<float> _vertices_positions;
        std::vector<UINT> _indices_data;// (_faces * 3 * 2, -1);
        
        //read vertices and indices
        w_vertex_data _vertex;
        glm::vec3 _min_vertex;
        glm::vec3 _max_vertex;

        uint32_t _vertex_index = 0, _texcoord_index = 0, _normal_index = 0;
        std::vector<float> _pos;
        std::vector<float> _tex;
        std::vector<float> _nor;

        for (int i = 0; i < _triangle->indices.size(); i += _ind)
        {
            if (_pos_index != -1)
            {
                _vertex_index = _triangle->indices[i];
                auto _start_index = _vertex_index * _pos_source->stride;
                //resize destination vector
                _pos.resize(_pos_source->stride);

                auto _from = _pos_source->float_array.begin() + _start_index;
                auto _to = _pos_source->float_array.begin() + _start_index + _pos_source->stride;
                std::copy(_from, _to, _pos.begin());
            }
            if (_tex_index != -1)
            {
                _texcoord_index = _triangle->indices[i + _tex_index];
                auto _start_index = _texcoord_index * _tex_source->stride;
                //resize destination vector
                _tex.resize(_tex_source->stride);

                auto _from = _tex_source->float_array.begin() + _start_index;
                auto _to = _tex_source->float_array.begin() + _start_index + _tex_source->stride;
                std::copy(_from, _to, _tex.begin());
            }
            if (_nor_index != -1)
            {
                _normal_index = _triangle->indices[i + _nor_index];
                auto _start_index = _normal_index * _nor_source->stride;
                //resize destination vector
                _nor.resize(_nor_source->stride);

                auto _from = _nor_source->float_array.begin() + _start_index;
                auto _to = _nor_source->float_array.begin() + _start_index + _nor_source->stride;
                std::copy(_from, _to, _nor.begin());
            }

            //check if vertex already exists, make sure add vertex that has different uv or normal
            auto _iter = std::find(_indices_data.begin(), _indices_data.end(), _vertex_index);
            if (_iter == _indices_data.end())
            {
                if (pZUp)
                {
                    //std::swap(_pos[1], _pos[2]);
                    //_pos[2] *= -1;

                    //std::swap(_nor[1], _nor[2]);
                    //_nor[2] = 1 - _nor[2];
                }

                if (pInvertNormal)
                {
                    _nor[0] = 1 - _nor[0];
                    _nor[1] = 1 - _nor[1];
                    _nor[2] = 1 - _nor[2];
                }

                //check for minimum and maximum vertices for bounding boxes
                _min_vertex.x = std::min(_pos[0], _min_vertex.x);
                _min_vertex.y = std::min(_pos[1], _min_vertex.y);
                _min_vertex.z = std::min(_pos[2], _min_vertex.z);

                _max_vertex.x = std::max(_pos[0], _max_vertex.x);
                _max_vertex.y = std::max(_pos[1], _max_vertex.y);
                _max_vertex.z = std::max(_pos[2], _max_vertex.z);

                if (_pos.size()) std::memcpy(&_vertex.position[0], _pos.data(), 3 * sizeof(float));
                if (_nor.size()) std::memcpy(&_vertex.normal[0], _nor.data(), 3 * sizeof(float));
                if (_tex.size()) std::memcpy(&_vertex.uv[0], _tex.data(), 2 * sizeof(float));
                _vertex.vertex_index = _vertex_index + 1;

                if (pAMDTootleOptimizing)
                {
                    _vertices_positions.insert(_vertices_positions.end(), _pos.begin(), _pos.end());
                }
                //TODO:
                std::memset(&_vertex.blend_weight[0], -1, 4 * sizeof(float));
                std::memset(&_vertex.blend_indices[0], -1, 4 * sizeof(int));

                _vertices_data.push_back(_vertex);
            }
            else
            {
                if (_vertex_index < _vertices_data.size())
                {
                    bool _do_it = false;

                    if (_tex.size() && ((_vertices_data[_vertex_index].uv[0] != _tex[0] || _vertices_data[_vertex_index].uv[1] != _tex[1])))
                    {
                        _do_it = true;
                    }
                    else if (_nor.size() && (_vertices_data[_vertex_index].normal[0] != _nor[0] || _vertices_data[_vertex_index].normal[1] != _nor[1] || _vertices_data[_vertex_index].normal[2] != _nor[2]))
                    {
                        _do_it = true;
                    }

                    if (_do_it)
                    {
                        if (pZUp)
                        {
                            //std::swap(_pos[1], _pos[2]);
                            //_pos[2] *= -1;

                            //std::swap(_nor[1], _nor[2]);
                            //_nor[2] =  1 - _nor[2];
                        }

                        if (pInvertNormal)
                        {
                            _nor[0] = 1 - _nor[0];
                            _nor[1] = 1 - _nor[1];
                            _nor[2] = 1 - _nor[2];
                        }

                        //check for minimum and maximum vertices for bounding boxes
                        _min_vertex.x = std::min(_pos[0], _min_vertex.x);
                        _min_vertex.y = std::min(_pos[1], _min_vertex.y);
                        _min_vertex.z = std::min(_pos[2], _min_vertex.z);

                        _max_vertex.x = std::max(_pos[0], _max_vertex.x);
                        _max_vertex.y = std::max(_pos[1], _max_vertex.y);
                        _max_vertex.z = std::max(_pos[2], _max_vertex.z);

                        if (_pos.size()) std::memcpy(&_vertex.position[0], _pos.data(), 3 * sizeof(float));
                        if (_nor.size()) std::memcpy(&_vertex.normal[0], _nor.data(), 3 * sizeof(float));
                        if (_tex.size()) std::memcpy(&_vertex.uv[0], _tex.data(), 2 * sizeof(float));

                        _vertex_index = _vertices_data.size();
                        _vertex.vertex_index = _vertex_index + 1;

                        if (pAMDTootleOptimizing)
                        {
                            _vertices_positions.insert(_vertices_positions.end(), _pos.begin(), _pos.end());
                        }
                        //TODO:
                        std::memset(&_vertex.blend_weight[0], -1, 4 * sizeof(float));
                        std::memset(&_vertex.blend_indices[0], -1, 4 * sizeof(int));

                        _vertices_data.push_back(_vertex);
                    }
                }
            }

            //store vertices and indices
            _indices_data.push_back(_vertex_index);

            _pos.clear();
            _nor.clear();
            _tex.clear();
        }

#pragma endregion

        //sort vertices
        std::sort(_vertices_data.begin(), _vertices_data.end(), [](_In_ const w_vertex_data pA, _In_ const w_vertex_data pB)
        {
            return pA.vertex_index < pB.vertex_index;
        });
       
        //check for round vertices to four
        auto _round_to_four = _vertices_data.size() % 4;
        if (_round_to_four)
        {
            for (size_t i = 0; i < _round_to_four; ++i)
            {
                w_vertex_data _vertex_data;
                std::memset(&_vertex_data, 0, sizeof(_vertex_data));
                _vertex_data.vertex_index = _vertices_data.size() + i + 1;
                _vertices_data.push_back(_vertex_data);
            }
        }

#pragma region AMD TOOTLE
        if (pAMDTootleOptimizing)
        {
            amd::tootle::apply(_vertices_data, _vertices_positions, _indices_data);
        }
#pragma endregion

        if (_nor_index == -1)
        {
            //compute tangent
        }

        auto _mesh = new w_mesh();

        auto _mat_iter = sLibraryMaterials.find(_triangle->material_name);
        if (_mat_iter != sLibraryMaterials.end())
        {
            auto _effect_iter = sLibraryEffects.find(_mat_iter->second);
            if (_effect_iter != sLibraryEffects.end())
            {
                auto _image_iter = sLibraryImages.find(_effect_iter->second);
                if (_image_iter != sLibraryImages.end())
                {
                    _mesh->textures_path = _image_iter->second;
                }
            }
        }

        _mesh->vertices.swap(_vertices_data);
        _mesh->indices.swap(_indices_data);

        _mesh->bounding_box.min[0] = _min_vertex[0];
        _mesh->bounding_box.min[1] = _min_vertex[1];
        _mesh->bounding_box.min[2] = _min_vertex[2];

        _mesh->bounding_box.max[0] = _max_vertex[0];
        _mesh->bounding_box.max[1] = _max_vertex[1];
        _mesh->bounding_box.max[2] = _max_vertex[2];

        
        //The transform of bounding box is same as model
        std::memcpy(&_mesh->bounding_box.position[0], &pTransform.position[0], 3 * sizeof(float));
        std::memcpy(&_mesh->bounding_box.rotation[0], &pTransform.rotation[0], 3 * sizeof(float));
        
        _model->_meshes.push_back(*_mesh);

        //free resources
        if (_vertices_data.size() > 0) _vertices_data.clear();
        if (_vertices_positions.size() > 0) _vertices_positions.clear();
        if (_indices_data.size() > 0) _indices_data.clear();
    }

	if (_model)
	{
		_model->_skeleton.swap(pBones);
	}
    
	//logger.warning(_messages);
	//_messages.clear();

	return _model;
}

void w_cpipeline_model::add_instance(_In_ const w_instance_info pValue)
{
    this->_instances_info.push_back(pValue);
}

void w_cpipeline_model::add_lods(_Inout_ std::vector<w_cpipeline_model*>& pLODs)
{
    for (size_t i = 0; i < pLODs.size(); ++i)
    {
        this->_lods.push_back(*pLODs[i]);
    }
}

void w_cpipeline_model::add_convex_hulls(_Inout_ std::vector<w_cpipeline_model*>& pCHs)
{
    for (size_t i = 0; i < pCHs.size(); ++i)
    {
        this->_convex_hulls.push_back(*pCHs[i]);
    }
}

void w_cpipeline_model::update_world()
{
	this->_world = glm::make_wpv_mat(
        glm::vec3(this->_transform.scale[0], this->_transform.scale[1], this->_transform.scale[2]),
        glm::vec3(this->_transform.rotation[0], this->_transform.rotation[1], this->_transform.rotation[2]),
        glm::vec3(this->_transform.position[0], this->_transform.position[1], this->_transform.position[2]));
}

void w_cpipeline_model::release()
{
    this->_bone_names.clear();
    this->_instanced_geo_name.empty();
    this->_instances_info.clear();

    this->_m_bind_pos.clear();
    this->_v_bind_pos.clear();
    
    //release animation containers
    for (auto& iter : this->_animation_containers)
    {
        iter.second.release();
    }
    this->_animation_containers.clear();

    //release meshes
    for (auto& iter : this->_meshes)
    {
        iter.release();
    }
    this->_meshes.clear();

    //release skeleton
    for (auto& iter : this->_skeleton)
    {
        iter->release();
    }
    this->_skeleton.clear();

    //release temp skeleton
    for (auto& iter : this->_temp_skeleton)
    {
        iter->release();
    }
    this->_temp_skeleton.clear();
}

#pragma region Getters

w_instance_info* w_cpipeline_model::get_instance_at(_In_ const size_t pIndex)
{
    return pIndex < this->_instances_info.size() ? &this->_instances_info[pIndex] : nullptr;
}

void w_cpipeline_model::get_instances(_Inout_ std::vector<w_instance_info>& pInstances)
{
    auto _size = this->_instances_info.size();
    if (!_size) return;
    
    pInstances = this->_instances_info;
}

std::string w_cpipeline_model::get_instance_geometry_name() const
{
    return this->_instanced_geo_name;
}

size_t w_cpipeline_model::get_meshes_count()
{
    return  this->_meshes.size();
}

void w_cpipeline_model::get_meshes(_Inout_ std::vector<w_cpipeline_model::w_mesh*>& pMeshes)
{
    auto _size = this->_meshes.size();
    if (!_size) return;

    for (size_t i = 0; i < _size; ++i)
    {
        pMeshes.push_back(&this->_meshes[i]);
    }
}

void w_cpipeline_model::get_lods(_Inout_ std::vector<w_cpipeline_model*>& pLODs)
{
    auto _size = this->_lods.size();
    if (!_size) return;

    for (size_t i = 0; i < _size; ++i)
    {
        pLODs.push_back(&this->_lods[i]);
    }
}

void w_cpipeline_model::get_convex_hulls(_Inout_ std::vector<w_cpipeline_model*>& pCHs)
{
    auto _size = this->_convex_hulls.size();
    if (!_size) return;

    for (size_t i = 0; i < _size; ++i)
    {
        pCHs.push_back(&this->_convex_hulls[i]);
    }
}

w_bounding_box* w_cpipeline_model::get_bounding_box(_In_ const size_t& pIndex)
{
    return pIndex < this->_meshes.size() ? &this->_meshes[pIndex].bounding_box : nullptr;
}

size_t w_cpipeline_model::get_lods_count()
{
    return this->_lods.size();
}

w_cpipeline_model* w_cpipeline_model::get_lod_at(_In_ size_t pIndex)
{
    return pIndex < this->_lods.size() ? &this->_lods[pIndex] : nullptr;
}

size_t w_cpipeline_model::get_convex_hulls_count()
{
    return this->_convex_hulls.size();
}

w_cpipeline_model* w_cpipeline_model::get_convex_hull_at(_In_ size_t pIndex)
{
    return pIndex < this->_convex_hulls.size() ? &this->_convex_hulls[pIndex] : nullptr;
}

#pragma endregion

#pragma region Setters

void w_cpipeline_model::set_name(_In_z_ const std::string& pValue)
{
	this->_name = pValue;
}

void w_cpipeline_model::set_instance_geometry_name(_In_z_ const std::string& pValue)
{
    this->_instanced_geo_name = pValue;
}

void w_cpipeline_model::set_transform(_In_ const w_transform_info& pValue)
{
	this->_transform = pValue;
}

#pragma endregion
