#include "pch.h"
#include "model.h"

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

class model_pimp
{
public:
	model_pimp(
		_In_ w_cpipeline_model* pContentPipelineModel,
		_In_ w_vertex_binding_attributes pVertexBindingAttributes) :
		_vertex_binding_attributes(pVertexBindingAttributes),
		_name("model"),
		_mesh(nullptr)
	{
		this->_model_name = pContentPipelineModel->get_name();
		this->_transform = pContentPipelineModel->get_transform();
		pContentPipelineModel->get_instances(this->_instnaces_transforms);
		pContentPipelineModel->get_meshes(this->_model_meshes);
	}

	W_RESULT load(
		_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
		_In_z_ const std::wstring& pVertexShaderPath,
		_In_z_ const std::wstring& pFragmentShaderPath)
	{
		const std::string _trace_info = this->_name + "::pre_load";
		
		if (!pGDevice) return W_FAILED;
		
		this->_gDevice = pGDevice;

		size_t _meshes_count = this->_model_meshes.size();
		uint32_t _base_vertex_index = 0;

		//std::vector<w_cpipeline_model*> _lods;
		//pCPModel->get_lods(_lods);

		//generate masked occlusion culling data from -ch files
		//auto _size = pCPModel->get_convex_hulls_count();
		//if (_size)
		//{
		//	std::vector<w_cpipeline_model*> _chs;
		//	pCPModel->get_convex_hulls(_chs);

		//	for (auto& _iter : _chs)
		//	{
		//		//generate vertices and indices of bounding box
		//		_add_data_for_masked_occlusion_culling(_iter);
		//	}
		//}
		//else if (_lods.size() > 0)
		//{
		//	//if there are no -ch for this model, then use first lod for masked occlusion culling
		//	_add_data_for_masked_occlusion_culling(_lods[0]);
		//}
		//else if (_sub_meshes_count)
		//{
		//	//no -ch and no -lod, so generate vertices and indices of model's bounding box
		//	_model_meshes[0]->bounding_box.generate_vertices_indices();
		//	_add_data_for_masked_occlusion_culling(_model_meshes[0]->bounding_box);
		//}

		//if (_meshes_count)
		//{
		//	_store_to_batch(
		//		_model_meshes,
		//		_vertex_binding_attributes,
		//		_base_vertex_index,
		//		this->_batch_vertices,
		//		this->_batch_indices,
		//		this->_merged_bounding_box,
		//		this->_sub_meshes_bounding_box,
		//		this->_textures_paths);
		//}

		//create first lod information
		//lod _lod;
		//_lod.first_index = _batch_indices.size();// First index for this LOD
		//_lod.index_count = _model_meshes[0]->indices.size();// Index count for this LOD
		//_lod.distance = _lod_distance_index * _lod_distance_offset;
		//_lod_distance_index++;

		//this->_lod_levels.push_back(_lod);

		//load texture
		//w_texture::load_to_shared_textures(this->_gDevice,
		//	content_path + L"textures/areas/" +
		//	wolf::system::convert::string_to_wstring(_model_meshes[0]->textures_path), &fs.texture);


	//append load mesh data to big vertices and indices
	//for (auto& _lod_mesh_data : _lods)
	//{
	//	_model_meshes.clear();
	//	_lod_mesh_data->get_meshes(_model_meshes);

	//	if (_model_meshes.size())
	//	{
	//		//create first lod information
	//		lod _lod;
	//		_lod.first_index = _batch_indices.size();// First index for this LOD
	//		_lod.index_count = _model_meshes[0]->indices.size();// Index count for this LOD
	//		_lod.distance = _lod_distance_index * _lod_distance_offset;
	//		_lod_distance_index++;

	//		this->_lod_levels.push_back(_lod);

	//		_store_to_batch(_model_meshes, _base_vertex);
	//	}
	//}

		this->_model_meshes.clear();

		return W_PASSED;
	}
		
	void release()
	{
		//release textures
		this->_textures_paths.clear();
	}

#pragma region Setters

#pragma endregion

#pragma region Getters

	glm::vec3 get_position() const
	{
		return glm::vec3(this->_transform.position[0], this->_transform.position[1], this->_transform.position[2]);
	}

	glm::vec3 get_rotation() const
	{
		return glm::vec3(this->_transform.rotation[0], this->_transform.rotation[1], this->_transform.rotation[2]);
	}

	glm::vec3 get_scale() const
	{
		return glm::vec3(this->_transform.scale[0], this->_transform.scale[1], this->_transform.scale[2]);
	}

	glm::vec3 get_instance_position(_In_ const size_t& pIndex) const
	{
		if (pIndex >= this->_instnaces_transforms.size()) return glm::vec3(0);
		return glm::vec3(
			this->_instnaces_transforms[pIndex].position[0],
			this->_instnaces_transforms[pIndex].position[1],
			this->_instnaces_transforms[pIndex].position[2]);
	}

	glm::vec3 get_instance_rotation(_In_ const size_t& pIndex) const
	{
		if (pIndex >= this->_instnaces_transforms.size()) return glm::vec3(0);
		return glm::vec3(
			this->_instnaces_transforms[pIndex].rotation[0],
			this->_instnaces_transforms[pIndex].rotation[1],
			this->_instnaces_transforms[pIndex].rotation[2]);
	}

	float get_instance_scale(_In_ const size_t& pIndex) const
	{
		if (pIndex >= this->_instnaces_transforms.size()) return 0.0f;
		return this->_instnaces_transforms[pIndex].scale;
	}

#pragma endregion

private:

	static void _store_to_batch(
		_In_ const std::vector<wolf::content_pipeline::w_cpipeline_mesh*>& pModelMeshes,
		_In_ const w_vertex_binding_attributes& pVertexBindingAttributes,
		_Inout_ uint32_t& pBaseVertex,
		_Inout_ std::vector<float>& pBatchVertices,
		_Inout_ std::vector<uint32_t>& pBatchIndices,
		_Inout_ w_bounding_box& pMergedBoundingBox,
		_Inout_ std::vector<w_bounding_box>& pSubMeshBoundingBoxes,
		_Inout_ std::vector<std::string>& pTexturePathsToBeLoad)
	{
		int _texture_index = 0;
		std::map<std::string, int> _textures_index;
		for (auto& _mesh_data : pModelMeshes)
		{
			int _uv_index = 0;
			if (pVertexBindingAttributes.declaration == w_vertex_declaration::VERTEX_POSITION_UV_INDEX ||
				pVertexBindingAttributes.declaration == w_vertex_declaration::VERTEX_POSITION_UV_INDEX_COLOR ||
				pVertexBindingAttributes.declaration == w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX ||
				pVertexBindingAttributes.declaration == w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL ||
				pVertexBindingAttributes.declaration == w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL_BLEND_WEIGHT_BLEND_INDICES)
			{
				auto _find = _textures_index.find(_mesh_data->textures_path);
				if (_textures_index.empty() && _find == _textures_index.end())
				{
					_uv_index = _texture_index;
					_textures_index.insert({ _mesh_data->textures_path, _texture_index++ });
					pTexturePathsToBeLoad.push_back(_mesh_data->textures_path);
				}
				else
				{
					_uv_index = _find->second;
				}
			}

			pMergedBoundingBox.merge(_mesh_data->bounding_box);
			pSubMeshBoundingBoxes.push_back(_mesh_data->bounding_box);

			for (size_t i = 0; i < _mesh_data->indices.size(); ++i)
			{
				pBatchIndices.push_back(pBaseVertex + _mesh_data->indices[i]);
			}

			switch (pVertexBindingAttributes.declaration)
			{
			default:
				break;
			case w_vertex_declaration::VERTEX_POSITION:
				for (auto& _data : _mesh_data->vertices)
				{
					auto _pos = _data.position;

					//position
					pBatchVertices.push_back(_pos[0]);
					pBatchVertices.push_back(_pos[1]);
					pBatchVertices.push_back(_pos[2]);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_COLOR:
				for (auto& _data : _mesh_data->vertices)
				{
					auto _pos = _data.position;
					auto _color = _data.color;

					//position
					pBatchVertices.push_back(_pos[0]);
					pBatchVertices.push_back(_pos[1]);
					pBatchVertices.push_back(_pos[2]);

					//color
					pBatchVertices.push_back(_color[0]);
					pBatchVertices.push_back(_color[1]);
					pBatchVertices.push_back(_color[2]);
					pBatchVertices.push_back(_color[3]);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_UV:
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
					pBatchVertices.push_back(_uv[1]);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_UV_INDEX:
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
					pBatchVertices.push_back(_uv[1]);
					pBatchVertices.push_back(_uv_index);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_UV_COLOR:
				for (auto& _data : _mesh_data->vertices)
				{
					auto _pos = _data.position;
					auto _uv = _data.uv;
					auto _color = _data.color;

					//position
					pBatchVertices.push_back(_pos[0]);
					pBatchVertices.push_back(_pos[1]);
					pBatchVertices.push_back(_pos[2]);

					//uv
					pBatchVertices.push_back(_uv[0]);
					pBatchVertices.push_back(_uv[1]);

					//color
					pBatchVertices.push_back(_color[0]);
					pBatchVertices.push_back(_color[1]);
					pBatchVertices.push_back(_color[2]);
					pBatchVertices.push_back(_color[3]);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_UV_INDEX_COLOR:
				for (auto& _data : _mesh_data->vertices)
				{
					auto _pos = _data.position;
					auto _uv = _data.uv;
					auto _color = _data.color;

					//position
					pBatchVertices.push_back(_pos[0]);
					pBatchVertices.push_back(_pos[1]);
					pBatchVertices.push_back(_pos[2]);

					//uv
					pBatchVertices.push_back(_uv[0]);
					pBatchVertices.push_back(_uv[1]);
					pBatchVertices.push_back(_uv_index);


					//color
					pBatchVertices.push_back(_color[0]);
					pBatchVertices.push_back(_color[1]);
					pBatchVertices.push_back(_color[2]);
					pBatchVertices.push_back(_color[3]);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_NORMAL_COLOR:
				for (auto& _data : _mesh_data->vertices)
				{
					auto _pos = _data.position;
					auto _nor = _data.normal;
					auto _color = _data.color;

					//position
					pBatchVertices.push_back(_pos[0]);
					pBatchVertices.push_back(_pos[1]);
					pBatchVertices.push_back(_pos[2]);

					//normal
					pBatchVertices.push_back(_nor[0]);
					pBatchVertices.push_back(_nor[1]);
					pBatchVertices.push_back(_nor[2]);

					//color
					pBatchVertices.push_back(_color[0]);
					pBatchVertices.push_back(_color[1]);
					pBatchVertices.push_back(_color[2]);
					pBatchVertices.push_back(_color[3]);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV:
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
					pBatchVertices.push_back(_uv[1]);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX:
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
					pBatchVertices.push_back(_uv[1]);
					pBatchVertices.push_back(_uv_index);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL:
				for (auto& _data : _mesh_data->vertices)
				{
					auto _pos = _data.position;
					auto _nor = _data.normal;
					auto _uv = _data.uv;
					auto _tangent = _data.tangent;
					auto _binormal = _data.binormal;

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
					pBatchVertices.push_back(_uv[1]);

					//tangent
					pBatchVertices.push_back(_tangent[0]);
					pBatchVertices.push_back(_tangent[1]);
					pBatchVertices.push_back(_tangent[2]);

					//binormal
					pBatchVertices.push_back(_binormal[0]);
					pBatchVertices.push_back(_binormal[1]);
					pBatchVertices.push_back(_binormal[2]);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL:
				for (auto& _data : _mesh_data->vertices)
				{
					auto _pos = _data.position;
					auto _nor = _data.normal;
					auto _uv = _data.uv;
					auto _tangent = _data.tangent;
					auto _binormal = _data.binormal;

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
					pBatchVertices.push_back(_uv[1]);
					pBatchVertices.push_back(_uv_index);

					//tangent
					pBatchVertices.push_back(_tangent[0]);
					pBatchVertices.push_back(_tangent[1]);
					pBatchVertices.push_back(_tangent[2]);

					//binormal
					pBatchVertices.push_back(_binormal[0]);
					pBatchVertices.push_back(_binormal[1]);
					pBatchVertices.push_back(_binormal[2]);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL_BLEND_WEIGHT_BLEND_INDICES:
				for (auto& _data : _mesh_data->vertices)
				{
					auto _pos = _data.position;
					auto _nor = _data.normal;
					auto _uv = _data.uv;
					auto _tangent = _data.tangent;
					auto _binormal = _data.binormal;
					auto _blend_weight = _data.blend_weight;
					auto _blend_indices = _data.blend_indices;

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
					pBatchVertices.push_back(_uv[1]);

					//tangent
					pBatchVertices.push_back(_tangent[0]);
					pBatchVertices.push_back(_tangent[1]);
					pBatchVertices.push_back(_tangent[2]);

					//binormal
					pBatchVertices.push_back(_binormal[0]);
					pBatchVertices.push_back(_binormal[1]);
					pBatchVertices.push_back(_binormal[2]);

					//blend_weight
					pBatchVertices.push_back(_blend_weight[0]);
					pBatchVertices.push_back(_blend_weight[1]);
					pBatchVertices.push_back(_blend_weight[2]);

					//blend_indices
					pBatchVertices.push_back(_blend_indices[0]);
					pBatchVertices.push_back(_blend_indices[1]);
					pBatchVertices.push_back(_blend_indices[2]);

					pBaseVertex++;
				}
				break;
			case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_INDEX_TANGENT_BINORMAL_BLEND_WEIGHT_BLEND_INDICES:
				for (auto& _data : _mesh_data->vertices)
				{
					auto _pos = _data.position;
					auto _nor = _data.normal;
					auto _uv = _data.uv;
					auto _tangent = _data.tangent;
					auto _binormal = _data.binormal;
					auto _blend_weight = _data.blend_weight;
					auto _blend_indices = _data.blend_indices;

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
					pBatchVertices.push_back(_uv[1]);
					pBatchVertices.push_back(_uv_index);

					//tangent
					pBatchVertices.push_back(_tangent[0]);
					pBatchVertices.push_back(_tangent[1]);
					pBatchVertices.push_back(_tangent[2]);

					//binormal
					pBatchVertices.push_back(_binormal[0]);
					pBatchVertices.push_back(_binormal[1]);
					pBatchVertices.push_back(_binormal[2]);

					//blend_weight
					pBatchVertices.push_back(_blend_weight[0]);
					pBatchVertices.push_back(_blend_weight[1]);
					pBatchVertices.push_back(_blend_weight[2]);

					//blend_indices
					pBatchVertices.push_back(_blend_indices[0]);
					pBatchVertices.push_back(_blend_indices[1]);
					pBatchVertices.push_back(_blend_indices[2]);

					pBaseVertex++;
				}
				break;
			};
		}
		_textures_index.clear();
	}

	HRESULT _load_textures()
	{
		const std::string _trace_info = this->_name + "_load_textures";

		bool _problem = false;
		for (auto& _path : this->_textures_paths)
		{
			auto _texture = new (std::nothrow) w_texture();
			if (_texture)
			{
				if (w_texture::load_to_shared_textures(
					this->_gDevice,
					wolf::system::convert::string_to_wstring(_path),
					&_texture) == W_PASSED)
				{
					this->_textures.push_back(_texture);
				}
				else
				{
					V(W_FAILED, "loading texture\'" + _path + "\'", _trace_info);
					_problem = true;
					//release texture
					SAFE_DELETE(_texture);
				}
			}
			else
			{
				V(W_FAILED, "allocating memory for w_texture\'" + _path + "\'", _trace_info);
				_problem = true;
			}
		}
		return _problem ? W_FAILED : W_PASSED;
	}

	HRESULT _load_buffers()
	{
		const std::string _trace_info = this->_name + "_load_buffers";

		bool _problem = false;


		return _problem ? S_FALSE : S_OK;
	}


	std::shared_ptr<w_graphics_device> 		_gDevice;
	std::string                             _name;
	w_vertex_binding_attributes             _vertex_binding_attributes;

	std::string								_model_name;
	w_transform_info						_transform;
	std::vector<w_instance_info>			_instnaces_transforms;

	//model meshes
	std::vector<w_cpipeline_mesh*>			_model_meshes;

	//global bounding box of all meshes
	w_bounding_box							_merged_bounding_box;
	//sub bounding boxes for all meshes
	std::vector<w_bounding_box>				_sub_meshes_bounding_box;

	w_mesh*									_mesh;

	std::vector<std::string>                _textures_paths;
	std::vector<w_texture*>					_textures;
};

model::model(
	_In_ w_cpipeline_model* pContentPipelineModel,
	_In_ w_vertex_binding_attributes pVertexBindingAttributes) :
	_pimp(new model_pimp(pContentPipelineModel, pVertexBindingAttributes))
{
	_super::set_class_name("w_model");
}

model::~model()
{
	release();
}

W_RESULT model::load(
	_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
	_In_z_ const std::wstring& pVertexShaderPath,
	_In_z_ const std::wstring& pFragmentShaderPath)
{
	return (!this->_pimp) ? W_FAILED : this->_pimp->load(pGDevice, pVertexShaderPath, pFragmentShaderPath);
}

void model::draw(_In_ const VkCommandBuffer& pCommandBuffer)
{

}

void model::indirect_draw(_In_ const VkCommandBuffer& pCommandBuffer)
{

}

ULONG model::release()
{
	if (_super::get_is_released()) return 1;

	//release the private implementation
	SAFE_RELEASE(this->_pimp);

	return _super::release();
}

#pragma region Getters

glm::vec3 model::get_position() const
{
	return (!this->_pimp) ? glm::vec3(0) : this->_pimp->get_position();
}

glm::vec3 model::get_rotation() const
{
	return (!this->_pimp) ? glm::vec3(0) : this->_pimp->get_rotation();
}

glm::vec3 model::get_scale() const
{
	return (!this->_pimp) ? glm::vec3(0) : this->_pimp->get_scale();
}

#pragma endregion

#pragma region Setters



#pragma endregion
