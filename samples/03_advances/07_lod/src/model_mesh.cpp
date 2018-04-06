#include "pch.h"
#include "model_mesh.h"

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

model_mesh::model_mesh(
	_In_ w_cpipeline_model* pContentPipelineModel,
	_In_ w_vertex_binding_attributes pVertexBindingAttributes) :
	vertex_binding_attributes(pVertexBindingAttributes),
	_name("model"),
	_mesh(nullptr),
	_visible(true),
	c_model(pContentPipelineModel)
{
}

model_mesh::~model_mesh()
{
	release();
}

W_RESULT model_mesh::load(
	_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_z_ const std::string& pPipelineCacheName,
	_In_z_ const std::wstring& pVertexShaderPath,
	_In_z_ const std::wstring& pFragmentShaderPath,
	_In_ const w_render_pass& pRenderPass)
{
	if (!pGDevice || !this->c_model) return W_FAILED;
	this->gDevice = pGDevice;

	const std::string _trace_info = this->_name + "::load";

	//create mesh
	this->_mesh = new (std::nothrow) w_mesh();
	if (!this->_mesh)
	{
		release();
		V(W_FAILED, "allocating memory for w_mesh for model: " + this->model_name, _trace_info, 2);
		return W_FAILED;
	}
	_mesh->set_vertex_binding_attributes(this->vertex_binding_attributes);

	//load all textures
	if (_load_textures())
	{
		//TODO: currently we use one texture per mesh
		_mesh->set_texture(this->_textures[0]);
	}
	else
	{
		//set the default texture
		_mesh->set_texture(w_texture::default_texture);
	}

	//load mesh
	auto _v_size = static_cast<uint32_t>(this->tmp_batch_vertices.size());
	auto _i_size = static_cast<uint32_t>(this->tmp_batch_indices.size());

	auto _hr = _mesh->load(
		this->gDevice,
		this->tmp_batch_vertices.data(),
		static_cast<uint32_t>(this->tmp_batch_vertices.size() * sizeof(float)),
		_v_size,
		this->tmp_batch_indices.data(),
		_i_size);

	//clear vectors
	this->tmp_batch_vertices.clear();
	this->tmp_batch_indices.clear();

	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "loading mesh for model: " + this->model_name, _trace_info, 2);
		return W_FAILED;
	}

	_hr = _create_buffers();
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "creating buffers for model: " + this->model_name, _trace_info, 2);
		return W_FAILED;
	}

#pragma region create shader modules
	if (_create_shader_modules(pVertexShaderPath, pFragmentShaderPath) == W_FAILED)
	{
		V(W_FAILED, "creating shader for model: " + this->model_name, _trace_info, 2);
		return W_FAILED;
	}
#pragma endregion

#pragma region create pipeline
	if (_create_pipeline(pPipelineCacheName, pRenderPass) == W_FAILED)
	{
		V(W_FAILED, "creating pipeline for model: " + this->model_name, _trace_info, 2);
		return W_FAILED;
	}
#pragma endregion

	//release content pipeline model
	this->c_model->release();
	this->c_model = nullptr;

	return W_PASSED;
}

W_RESULT model_mesh::draw(_In_ const w_command_buffer& pCommandBuffer, _In_ const bool& pInDirectMode)
	{
		if (!this->_visible) return W_PASSED;

		const std::string _trace_info = this->_name + "::draw";

		if (!this->_mesh) return W_FAILED;

		//bind pipeline
		this->_pipeline.bind(pCommandBuffer);
		auto _buffer_handle = this->_instances_buffer.get_buffer_handle();
		return this->_mesh->draw(pCommandBuffer, _buffer_handle.handle ? &_buffer_handle : nullptr, this->instnaces_transforms.size(), pInDirectMode);
	}

void model_mesh::_store_to_batch(
	_In_ const std::vector<w_cpipeline_mesh*>& pModelMeshes,
	_In_ const w_vertex_binding_attributes& pVertexBindingAttributes,
	_Inout_ uint32_t& pBaseVertex,
	_Inout_ std::vector<float>& pBatchVertices,
	_Inout_ std::vector<uint32_t>& pBatchIndices,
	_Inout_ w_bounding_box* pMergedBoundingBox,
	_Inout_ std::vector<w_bounding_box>* pSubMeshBoundingBoxes,
	_Inout_ std::vector<std::string>* pTexturePathsToBeLoad)
{
	int _texture_index = 0;
	std::map<std::string, int> _textures_index;
	for (auto& _mesh_data : pModelMeshes)
	{
		//check for finding uv index
		int _texture_uv_index = 0;
		if (pTexturePathsToBeLoad)
		{
			auto _find = _textures_index.find(_mesh_data->textures_path);
			if (_textures_index.empty() && _find == _textures_index.end())
			{
				_texture_uv_index = _texture_index;
				_textures_index.insert({ _mesh_data->textures_path, _texture_index++ });
				pTexturePathsToBeLoad->push_back(_mesh_data->textures_path);
			}
			else
			{
				_texture_uv_index = _find->second;
			}
		}

		//set bounding boxes
		if (pMergedBoundingBox)
		{
			pMergedBoundingBox->merge(_mesh_data->bounding_box);
		}
		if (pSubMeshBoundingBoxes)
		{
			pSubMeshBoundingBoxes->push_back(_mesh_data->bounding_box);
		}

		uint32_t _vertex_offset = 0, _instance_vertex_offset = 0, i = 0;

#pragma region store index buffer
		for (i = 0; i < _mesh_data->indices.size(); ++i)
		{
			pBatchIndices.push_back(pBaseVertex + _mesh_data->indices[i]);
		}
#pragma endregion

#pragma region store vertex buffer
		i = 0;
		auto _vertex_dec = pVertexBindingAttributes.binding_attributes.find(0);
		switch (pVertexBindingAttributes.declaration)
		{
		default:
			//user defined, we need to find vertex declaration
			if (_vertex_dec != pVertexBindingAttributes.binding_attributes.end())
			{
				for (auto& _data : _mesh_data->vertices)
				{
					for (auto _dec : _vertex_dec->second)
					{
						if (_dec == w_vertex_attribute::W_TEXTURE_INDEX)
						{
							pBatchVertices.push_back(_texture_uv_index);
						}
						if (_dec == w_vertex_attribute::W_UV)
						{
							pBatchVertices.push_back(_data.uv[0]);
							pBatchVertices.push_back(1 - _data.uv[1]);
						}
						else if (_dec == w_vertex_attribute::W_POS)
						{
							pBatchVertices.push_back(_data.position[0]);
							pBatchVertices.push_back(_data.position[1]);
							pBatchVertices.push_back(_data.position[2]);
						}
						else if (_dec == w_vertex_attribute::W_NORM)
						{
							pBatchVertices.push_back(_data.normal[0]);
							pBatchVertices.push_back(_data.normal[1]);
							pBatchVertices.push_back(_data.normal[2]);
						}
						else if (_dec == w_vertex_attribute::W_TANGENT)
						{
							pBatchVertices.push_back(_data.tangent[0]);
							pBatchVertices.push_back(_data.tangent[1]);
							pBatchVertices.push_back(_data.tangent[2]);
						}
						else if (_dec == w_vertex_attribute::W_BINORMAL)
						{
							pBatchVertices.push_back(_data.binormal[0]);
							pBatchVertices.push_back(_data.binormal[1]);
							pBatchVertices.push_back(_data.binormal[2]);
						}
						else if (_dec == w_vertex_attribute::W_COLOR)
						{
							pBatchVertices.push_back(_data.color[0]);
							pBatchVertices.push_back(_data.color[1]);
							pBatchVertices.push_back(_data.color[2]);
							pBatchVertices.push_back(_data.color[3]);
						}
						else if (_dec == w_vertex_attribute::W_BLEND_WEIGHT)
						{
							pBatchVertices.push_back(_data.blend_weight[0]);
							pBatchVertices.push_back(_data.blend_weight[1]);
							pBatchVertices.push_back(_data.blend_weight[2]);
							pBatchVertices.push_back(_data.blend_weight[3]);
						}
						else if (_dec == w_vertex_attribute::W_BLEND_INDICES)
						{
							pBatchVertices.push_back(_data.blend_indices[0]);
							pBatchVertices.push_back(_data.blend_indices[1]);
							pBatchVertices.push_back(_data.blend_indices[2]);
							pBatchVertices.push_back(_data.blend_indices[3]);
						}
					}
					pBaseVertex++;
				}
			}
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
				pBatchVertices.push_back(_texture_uv_index);

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
				pBatchVertices.push_back(_texture_uv_index);


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
				pBatchVertices.push_back(_texture_uv_index);

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
				pBatchVertices.push_back(_texture_uv_index);

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
				pBatchVertices.push_back(_texture_uv_index);

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
#pragma endregion

	}
	_textures_index.clear();
}

W_RESULT model_mesh::_load_textures()
{
	const std::string _trace_info = this->_name + "_load_textures";

	bool _problem = false;
	for (auto& _path : this->textures_paths)
	{
		auto _texture = new (std::nothrow) w_texture();
		if (_texture)
		{
			if (w_texture::load_to_shared_textures(
				this->gDevice,
				wolf::content_path + L"models/sponza/sponza/" +
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

W_RESULT model_mesh::_create_buffers()
{
	const std::string _trace_info = this->_name + "::_create_buffers";

	//TODO: later work on one huge buffer instead of creating 4 seperated staging buffers
	w_buffer _staging_buffers[4];
	defer _(nullptr, [&](...)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			_staging_buffers[i].release();
		}
	});

	//set compute shader batch size
	uint32_t _draw_counts = 1 + static_cast<uint32_t>(this->instnaces_transforms.size());
	//find nearest pow of 2 for compute shader local batch size
	this->_cs.batch_local_size = static_cast<uint32_t>(pow(2, ceil(log(_draw_counts) / log(2))));
	this->indirect_draws.drawing_commands.resize(this->_cs.batch_local_size);

	this->_cs_out.draw_count = _draw_counts;

	for (uint32_t i = 0; i < _draw_counts; ++i)
	{
		this->indirect_draws.drawing_commands[i].instanceCount = 1;
		this->indirect_draws.drawing_commands[i].firstInstance = i;
		//firstIndex and indexCount are written by the compute shader
	}

	//uint32_t _size = (uint32_t)(_draw_counts * sizeof(w_draw_indexed_indirect_command));


	return W_PASSED;
}

W_RESULT model_mesh::_create_instance_buffer(_In_ const std::vector<float>& pData, _In_ const uint32_t& pSizeOfBuffer)
{
	const std::string _trace_info = this->_name + "::_create_instance_buffer";

	w_buffer _staging_buffers;

	_staging_buffers.load_as_staging(this->gDevice, pSizeOfBuffer);
	if (_staging_buffers.bind() == W_FAILED)
	{
		V(W_FAILED, "binding to staging buffer of vertex_instance_buffer", _trace_info, 2);
		return W_FAILED;
	}

	if (_staging_buffers.set_data(pData.data()) == W_FAILED)
	{
		V(W_FAILED, "setting data to staging buffer of vertex_instance_buffer", _trace_info, 2);
		return W_FAILED;
	}

	if (this->_instances_buffer.load(
		this->gDevice,
		pSizeOfBuffer,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == W_FAILED)
	{
		V(W_FAILED, "loading device buffer of vertex_instance_buffer", _trace_info, 2);
		return W_FAILED;
	}

	if (this->_instances_buffer.bind() == W_FAILED)
	{
		V(W_FAILED, "binding to device buffer of vertex_instance_buffer", _trace_info, 2);
		return W_FAILED;
	}
	if (_staging_buffers.copy_to(this->_instances_buffer) == W_FAILED)
	{
		V(W_FAILED, "copying to device buffer of vertex_instance_buffer", _trace_info, 2);
		return W_FAILED;
	}
	_staging_buffers.release();

	return W_PASSED;
}

W_RESULT model_mesh::_create_shader_modules(
	_In_z_ const std::wstring& pVertexShaderPath,
	_In_z_ const std::wstring& pFragmentShaderPath)
{
	const std::string _trace_info = this->_name + "_create_shader_module";

	W_RESULT _hr = W_FAILED;

	std::vector<w_shader_binding_param> _shader_params;

	w_shader_binding_param _shader_param;
	_shader_param.index = 0;
	_shader_param.type = w_shader_binding_type::UNIFORM;
	_shader_param.stage = w_shader_stage_flag_bits::VERTEX_SHADER;

	//set U0 uniform of instance vertex shader, which has seperate structure for basic model or instanced models
	if (this->instnaces_transforms.size())
	{
		_hr = this->_instance_u0.load(this->gDevice);
		if (_hr == W_FAILED)
		{
			this->_shader.release();
			V(W_FAILED, "loading vertex shader instance uniform for model: " + this->model_name, _trace_info, 2);
			return W_FAILED;
		}
		_shader_param.buffer_info = this->_instance_u0.get_descriptor_info();
	}
	else
	{
		_hr = this->_basic_u0.load(this->gDevice);
		if (_hr == W_FAILED)
		{
			this->_shader.release();
			V(W_FAILED, "loading vertex shader basic uniform for model: " + this->model_name, _trace_info, 2);
			return W_FAILED;
		}
		_shader_param.buffer_info = this->_basic_u0.get_descriptor_info();
	}
	_shader_params.push_back(_shader_param);

	//The sampler2D of fragment shader
	_shader_param.index = 1;
	_shader_param.type = w_shader_binding_type::SAMPLER2D;
	_shader_param.stage = w_shader_stage_flag_bits::FRAGMENT_SHADER;
	_shader_param.image_info = this->_textures[0]->get_descriptor_info();

	_shader_params.push_back(_shader_param);

	//the U1 uniform of fragment shader
	_hr = this->_u1.load(this->gDevice);
	if (_hr == W_FAILED)
	{
		this->_shader.release();
		V(W_FAILED, "loading fragment shader uniform for model: " + this->model_name, _trace_info, 2);
		return W_FAILED;
	}
	_shader_param.index = 2;
	_shader_param.type = w_shader_binding_type::UNIFORM;
	_shader_param.stage = w_shader_stage_flag_bits::FRAGMENT_SHADER;
	_shader_param.buffer_info = this->_u1.get_descriptor_info();

	_shader_params.push_back(_shader_param);

	_hr = this->_shader.set_shader_binding_params(_shader_params);
	if (_hr == W_FAILED)
	{
		this->_shader.release();
		V(W_FAILED, "setting shader binding param for model: " + this->model_name, _trace_info, 2);
	}
	_shader_params.clear();

	return W_PASSED;
}

W_RESULT model_mesh::_create_pipeline(
	_In_z_ const std::string& pPipelineCacheName,
	_In_ const w_render_pass& pRenderPass)
{
	return this->_pipeline.load(
		this->gDevice,
		this->vertex_binding_attributes,
		w_primitive_topology::TRIANGLE_LIST,
		&pRenderPass,
		&this->_shader,
		{ pRenderPass.get_viewport() },
		{ pRenderPass.get_viewport_scissor() });
}

ULONG model_mesh::release()
{
	if (_super::get_is_released()) return 1;

	this->_name.clear();
	this->model_name.clear();

	this->instnaces_transforms.clear();

	this->_pipeline.release();
	this->_shader.release();

	this->_basic_u0.release();
	this->_instance_u0.release();
	this->_u1.release();

	//release mesh resources
	SAFE_RELEASE(this->_mesh);
	this->_instances_buffer.release();
	this->sub_meshes_bounding_box.clear();

	//release textures
	this->textures_paths.clear();
	for (auto _t : this->_textures)
	{
		if (_t) _t->release();
	}
	this->_textures.clear();

	this->gDevice = nullptr;

	return 0;
}

#pragma region Getters

std::string model_mesh::get_model_name() const
{
	return this->model_name;
}

glm::vec3 model_mesh::get_position() const
{
	return glm::vec3(this->transform.position[0], this->transform.position[1], this->transform.position[2]);
}

glm::vec3 model_mesh::get_rotation() const
{
	return glm::vec3(this->transform.rotation[0], this->transform.rotation[1], this->transform.rotation[2]);
}

glm::vec3 model_mesh::get_scale() const
{
	return glm::vec3(this->transform.scale[0], this->transform.scale[1], this->transform.scale[2]);
}

std::vector<w_instance_info> model_mesh::get_instances() const
{
	return this->instnaces_transforms;
}

w_bounding_box model_mesh::get_global_bounding_box() const
{
	return this->merged_bounding_box;
}

bool model_mesh::get_enable_instances_colors() const
{
	return this->_u1.data.cmds == 1;
}

bool model_mesh::get_visible() const
{
	return this->_visible;
}

#pragma endregion

#pragma region Setters

void model_mesh::set_view_projection(_In_ const glm::mat4& pView, _In_ const glm::mat4& pProjection)
{
	const std::string _trace_info = this->_name + "::set_view_projection";

	if (this->instnaces_transforms.size())
	{
		this->_instance_u0.data.view = pView;
		this->_instance_u0.data.projection = pProjection;
		auto _hr = this->_instance_u0.update();
		if (_hr == W_FAILED)
		{
			V(W_FAILED, "updating instance uniform ViewProjection for model: " + this->model_name, _trace_info, 3);
		}
	}
	else
	{
		auto _position = get_position();
		auto _rotation = get_rotation();
		auto _scale = get_scale();

		this->_basic_u0.data.model = glm::translate(_position) * glm::rotate(_rotation) * glm::scale(_scale);
		this->_basic_u0.data.view = pView;
		this->_basic_u0.data.projection = pProjection;
		auto _hr = this->_basic_u0.update();
		if (_hr == W_FAILED)
		{
			V(W_FAILED, "updating basic uniform ViewProjection for model: " + this->model_name, _trace_info, 3);
		}
	}
}

void model_mesh::set_enable_instances_colors(_In_ const bool& pEnable)
{
	const std::string _trace_info = this->_name + "::set_enable_instances_colors";

	this->_u1.data.cmds = pEnable ? 1 : 0;
	auto _hr = this->_u1.update();
	if (_hr == W_FAILED)
	{
		V(W_FAILED, "updating u1 uniform for model: " + this->model_name, _trace_info, 3);
	}
}

void model_mesh::set_visible(_In_ const bool& pValue)
{
	this->_visible = pValue;
}

#pragma endregion




