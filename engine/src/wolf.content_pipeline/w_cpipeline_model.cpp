#include "w_cpipeline_pch.h"
#include "w_cpipeline_model.h"

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

w_cpipeline_model* w_cpipeline_model::create_model(_In_ c_geometry& pGeometry, 
    _In_ c_skin* pSkin,
	_In_ std::vector<c_bone*>& pBones, 
    _In_ string pBoneNames [], 
    _In_ std::map<std::string, std::string>& sLibraryMaterials,
    _In_ std::map<std::string, std::string>& sLibraryEffects,
    _In_ std::map<std::string, std::string>& sLibraryImages,
    _In_ bool pOptimizing,
    _In_ bool pZUp)
{
	auto _model = new w_cpipeline_model();

	std::string _messages;

	for (auto _triangle : pGeometry.triangles)
	{
		std::vector<std::vector<int>> _normal_list;

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

#pragma region read vertices & indices data

		//auto _ind2 = 0;
		//auto _ind3 = 0;
		auto _pos_source = pGeometry.sources[_pos_index];
		auto _tex_source = _tex_index != -1 ? pGeometry.sources[_tex_index] : nullptr;
		auto _nor_source = _nor_index != -1 ? pGeometry.sources[_nor_index] : nullptr;

		std::vector<float> _just_vertices_pos;
		std::vector<w_vertex_data> _vertices_data;
		std::vector<UINT> _indices_data;
		std::map<int, w_vertex_index> _vertices;

		std::map<int, int> _dic1;
		std::vector<std::vector<int>> _l1;

		uint32_t _faces = 0;
		if (_ind)
		{
			_faces = (static_cast<uint32_t>(_pos_source->float_array.size()) / _ind) - 2;
		}
		std::vector<UINT> _indices;
		_indices.reserve(_faces * 3);

		//read indices
		for (int i = 0; i < _triangle->indices.size(); i += _ind)
		{
			auto _vertex_index = 0;
			auto _texcoord_index = 0;
			std::vector<float> _pos;
			std::vector<float> _tex;
			std::vector<float> _nor;
			UINT _index = 0;

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
				auto _start_index = _triangle->indices[i + _nor_index];
				//resize destination vector
				_nor.resize(_nor_source->stride);

				auto _from = _nor_source->float_array.begin() + _start_index;
				auto _to = _nor_source->float_array.begin() + _start_index + _nor_source->stride;
				std::copy(_from, _to, _nor.begin());
			}

			auto _is_duplicate = false;

#pragma region optimizing

			const float _epsilon = 0.0001f;

			int _vertices_size = static_cast<int>(_vertices.size());
			if (pOptimizing)
			{
				for (int j = _vertices_size - 1; j >= 0; j--)
				{
					if (_vertices[j].texture.size() > 0)
					{
						if (std::abs(_vertices[j].vertex[0] - _pos[0]) < _epsilon &&
							std::abs(_vertices[j].vertex[1] - _pos[1]) < _epsilon &&
							std::abs(_vertices[j].vertex[2] - _pos[2]) < _epsilon)
						{
							if (std::abs(_vertices[j].texture[0] - _tex[0]) < _epsilon &&
								std::abs(_vertices[j].texture[1] - _tex[1]) < _epsilon)
							{
								_is_duplicate = true;
								_index = static_cast<UINT>(j);
							}
							else
							{
								_messages += "Duplicated vertex with different uv for model: " + pGeometry.name + "\r\n";
							
								_dic1[_vertices_size] = j;
								bool _done = false;
								for (auto int1 : _normal_list)
								{
									if (std::find_if(int1.begin(), int1.end(), [&_vertices_size](auto _iter) -> bool { return _iter == _vertices_size; }) != int1.end())
									{
										int1.push_back(j);
										_done = true;
									}
									if (std::find_if(int1.begin(), int1.end(), [&j](auto _iter) -> bool { return _iter == j; }) != int1.end())
									{
										int1.push_back(_vertices_size);
										_done = true;
									}
								}

								if (!_done)
								{
									std::vector<int> _iner_list = { _vertices_size, j };
									_normal_list.push_back(_iner_list);
								}
							}
							break;
						}
					}
					else if (std::abs(_vertices[j].vertex[0] - _pos[0]) < _epsilon &&
						std::abs(_vertices[j].vertex[1] - _pos[1]) < _epsilon &&
						std::abs(_vertices[j].vertex[2] - _pos[2]) < _epsilon)
					{
						_is_duplicate = true;
						_index = static_cast<UINT>(j);
						break;
					}

				}
			}

#pragma endregion

			if (!_is_duplicate)
			{
				_indices_data.push_back(static_cast<UINT>(_vertices_size));
				_vertices[_vertices_size].vertex.swap(_pos);
				_vertices[_vertices_size].normal.swap(_nor);
				_vertices[_vertices_size].texture.swap(_tex);
				_vertices[_vertices_size].vertex_index = _vertex_index + 1;

				_pos.clear();
				_nor.clear();
				_tex.clear();
			}
			else
			{
				_indices_data.push_back(_index);
			}
		}

#pragma endregion

#pragma region create vertex data format

		glm::vec3 _min_vertex;
		glm::vec3 _max_vertex;

		for (int i = 0; i < _vertices.size(); ++i)
		{
			auto _v = _vertices[i];
			glm::vec4 _bw(-1);
			glm::int4 _bi(-1);
			std::vector<c_weight_bone> _wbs;
			if (pSkin)
			{
				//Not implemented yet
			}

            glm::vec3 _pos;
            
            _pos.x = _v.vertex[0];
            if (pZUp)
            {
                _pos.y = -_v.vertex[2];
            }
            else
            {
                _pos.y = _v.vertex[1];
            }
            _pos.z = _v.vertex[1];

			_min_vertex.x = min(_pos.x, _min_vertex.x);
			_min_vertex.y = min(_pos.y, _min_vertex.y);
			_min_vertex.z = min(_pos.z, _min_vertex.z);

			_max_vertex.x = max(_pos.x, _max_vertex.x);
			_max_vertex.y = max(_pos.y, _max_vertex.y);
			_max_vertex.z = max(_pos.z, _max_vertex.z);

			w_vertex_data _vertex_data;
			_vertex_data.position[0] = _pos[0];
            _vertex_data.position[1] = _pos[1];
            _vertex_data.position[2] = _pos[2];


			auto _normal = _v.normal.size() > 0 ? glm::vec3(_v.normal[0], _v.normal[1], _v.normal[2]) : glm::vec3(0);
            _vertex_data.normal[0] = _normal[0];
            _vertex_data.normal[1] = _normal[1];
            _vertex_data.normal[2] = _normal[2];

			_vertex_data.blend_weight[0] = _bw[0];
            _vertex_data.blend_weight[1] = _bw[1];
            _vertex_data.blend_weight[2] = _bw[2];
            _vertex_data.blend_weight[3] = _bw[3];

			_vertex_data.blend_indices[0] = _bi[0];
            _vertex_data.blend_indices[1] = _bi[1];
            _vertex_data.blend_indices[2] = _bi[2];
            _vertex_data.blend_indices[3] = _bi[3];

            auto _uv = _v.texture.size() > 0 ?  glm::vec2(_v.texture[0], _v.texture[1]) : glm::vec2(0);
            _vertex_data.uv[0] = _uv[0];
            _vertex_data.uv[1] = _uv[1];

			_vertex_data.vertex_index = _v.vertex_index;

			_just_vertices_pos.push_back(_pos[0]);
			_just_vertices_pos.push_back(_pos[1]);
            _just_vertices_pos.push_back(_pos[2]);

			_vertices_data.push_back(_vertex_data);
		}

        auto _round_to_four = _vertices_data.size() % 4;
        if (_round_to_four)
        {
            for(size_t i = 0; i < _round_to_four; ++i)
            {
                w_vertex_data _vertex_data;
                std::memset(&_vertex_data, 0, sizeof(_vertex_data));
                _vertex_data.vertex_index = _vertices_data.size() + i + 1;
                _vertices_data.push_back(_vertex_data);
            }
        }
        
#pragma endregion

		if (_nor_index == -1)
		{
#pragma region compute tangent



#pragma endregion
		}

		for (auto _normal : _normal_list)
		{
			glm::vec3 _nor(0);
			glm::vec3 _tan(0);
			glm::vec3 _bin(0);
			for (auto i : _normal)
			{
				auto _v = _vertices_data[i];
				
                _nor[0] += _v.normal[0];
                _nor[1] += _v.normal[1];
                _nor[2] += _v.normal[2];

				_tan[0] += _v.tangent[0];
                _tan[1] += _v.tangent[1];
                _tan[2] += _v.tangent[2];

				_bin[0] += _v.binormal[0];
                _bin[1] += _v.binormal[1];
                _bin[2] += _v.binormal[2];
			}
			_nor = glm::normalize(_nor);
			_tan = glm::normalize(_tan);
			_bin = glm::normalize(_bin);

			for (auto i : _normal)
			{
				auto _v = _vertices_data[i];

                _v.normal[0] = _nor[0];
                _v.normal[1] = _nor[1];
                _v.normal[2] = _nor[2];

				_v.tangent[0] = _tan[0];
                _v.tangent[1] = _tan[1];
                _v.tangent[2] = _tan[2];

				_v.binormal[0] = _bin[0];
                _v.binormal[1] = _bin[1];
                _v.binormal[2] = _bin[2];

				_vertices_data[i] = _v;
			}
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

		_mesh->just_vertices_pos.swap(_just_vertices_pos);
		_mesh->vertices.swap(_vertices_data);
		_mesh->indices.swap(_indices_data);
		
        _mesh->bounding_box.min[0] = _min_vertex[0];
        _mesh->bounding_box.min[1] = _min_vertex[1];
        _mesh->bounding_box.min[2] = _min_vertex[2];

		_mesh->bounding_box.max[0] = _max_vertex[0];
        _mesh->bounding_box.max[1] = _max_vertex[1];
        _mesh->bounding_box.max[2] = _max_vertex[2];

		_model->_meshes.push_back(*_mesh);

#pragma region free resources
		if (_normal_list.size() > 0) _normal_list.clear();
		if (_vertices_data.size() > 0) _vertices_data.clear();
		if (_indices_data.size() > 0) _indices_data.clear();

		if (_vertices.size() > 0)
		{
			std::for_each(_vertices.begin(), _vertices.end(), [](std::pair<int, w_vertex_index> pPair)
			{
				pPair.second.release();
			});
			_vertices.clear();
		}

		if (_dic1.size() > 0)
		{
			_dic1.clear();
		}

		if (_l1.size() > 0)
		{
			std::for_each(_l1.begin(), _l1.end(), [](std::vector<int>& pVector)
			{
				if (pVector.size() > 0)
				{
					pVector.clear();
				}
			});
			_l1.clear();
		}

#pragma endregion
	}

	if (_model)
	{
		_model->_skeleton.swap(pBones);
	}
    
	logger.warning(_messages);
	_messages.clear();

	return _model;
}

void w_cpipeline_model::add_instance(_In_ const w_instance_info pValue)
{
    this->_instances_info.push_back(pValue);
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

w_cpipeline_model::w_instance_info* w_cpipeline_model::get_instance_at(_In_ const size_t pIndex)
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

void w_cpipeline_model::get_meshes(_Inout_ std::vector<w_cpipeline_model::w_mesh*>& pMeshes)
{
    auto _size = this->_meshes.size();
    if (!_size) return;

    for (size_t i = 0; i < _size; ++i)
    {
        pMeshes.push_back(&this->_meshes[i]);
    }
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

void w_cpipeline_model::set_transform(w_transform_info& pValue)
{
	this->_transform = pValue;
}

#pragma endregion
