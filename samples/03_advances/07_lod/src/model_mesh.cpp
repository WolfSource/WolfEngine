#include "pch.h"
#include "model_mesh.h"

using namespace wolf;
using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

model_mesh::model_mesh(
	_In_ w_cpipeline_model* pContentPipelineModel,
	_In_ w_vertex_binding_attributes pVertexBindingAttributes) :
	vertex_binding_attributes(pVertexBindingAttributes),
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	_name("model"),
	_mesh(nullptr),
	_show_only_lod(false),
	global_visiblity(true),
	c_model(pContentPipelineModel),
	_selected_lod_index(0),
	_is_sky(false)
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
{
}

model_mesh::~model_mesh()
{
	release();
}

W_RESULT model_mesh::load(
	_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_z_ const std::string& pPipelineCacheName,
	_In_z_ const std::string& pComputePipelineCacheName,
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
	if (_load_textures() == W_PASSED)
	{
		//TODO: currently we use one texture per mesh
		_mesh->set_texture(this->_textures[0]);
	}
	else
	{
		//set the default texture
		this->_textures.push_back(w_texture::default_texture);
		logger.warning("default texture will be used for model: " + this->model_name);
		_mesh->set_texture(this->_textures[0]);
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

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (_create_buffers() == W_FAILED)
	{
		release();
		return W_FAILED;
	}

	//create shader modules
	if (_create_shader_modules(pVertexShaderPath, pFragmentShaderPath) == W_FAILED)
	{
		release();
		return W_FAILED;
	}

	//create pipeline
	if (_create_pipelines(pPipelineCacheName, pRenderPass) == W_FAILED)
	{
		release();
		return W_FAILED;
	}

	//release content pipeline model
	this->c_model->release();
	this->c_model = nullptr;

	auto _number_of_instances = static_cast<uint32_t>(this->instnaces_transforms.size());
	if (_number_of_instances)
	{
		//create compute semaphore
		if (this->_cs.semaphore.initialize(this->gDevice) == W_FAILED)
		{
			release();
			V(W_FAILED, "initializing compute semaphore for model: " + this->model_name, _trace_info, 2);
			return W_FAILED;
		}
		//build compute command buffer
		if (_build_compute_command_buffer() == W_FAILED)
		{
			release();
			V(W_FAILED, "building compute command buffer for model: " + this->model_name, _trace_info, 2);
			return W_FAILED;
		}
	}

	//get bounding sphere
	auto _get_first_model_bsphere = w_bounding_sphere::create_from_bounding_box(this->sub_meshes_bounding_box.at(0));
	this->_u1.data.texture_max_mip_maps_max_level = this->_textures[0]->get_mip_maps_level();
	this->_u1.data.bounding_sphere_radius = _get_first_model_bsphere.radius;
	if (this->_u1.update() == W_FAILED)
	{
		V(W_FAILED, "updating uniform u1(mipmaps) for model: " + this->model_name, _trace_info, 3);
		return W_FAILED;
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	return W_PASSED;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++

W_RESULT model_mesh::_build_compute_command_buffer()
{
	const std::string _trace_info = this->name + "::_build_compute_command_buffer";

	if (this->_cs.command_buffers.load(
		this->gDevice,
		1,
		w_command_buffer_level::PRIMARY,
		true,
		&gDevice->vk_compute_queue) == W_FAILED)
	{
		V(W_FAILED, "creating compute command buffer for " + this->model_name, _trace_info);
		return W_FAILED;
	}

	auto _cmd = this->_cs.command_buffers.get_command_at(0);
	this->_cs.command_buffers.begin(0);
	{
		auto _indirect_draws_buffer = this->indirect_draws.buffer.get_buffer_handle();
		auto _size = this->indirect_draws.buffer.get_descriptor_info().range;

		// Add memory barrier to ensure that the indirect commands have been consumed before the compute shader updates them
		VkBufferMemoryBarrier _barrier = {};
		_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		_barrier.buffer = _indirect_draws_buffer.handle;
		_barrier.size = _size;
		_barrier.srcAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
		_barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		_barrier.srcQueueFamilyIndex = this->gDevice->vk_graphics_queue.index;
		_barrier.dstQueueFamilyIndex = this->gDevice->vk_compute_queue.index;

		vkCmdPipelineBarrier(
			_cmd.handle,
			VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0,
			0, nullptr,
			1, &_barrier,
			0, nullptr);

		if (this->_cs.pipeline.bind(_cmd, w_pipeline_bind_point::COMPUTE) == W_FAILED)
		{
			this->_cs.command_buffers.end(0);
			V(W_FAILED, "binding compute command buffer for " + this->model_name, _trace_info);
			return W_FAILED;
		}

		vkCmdDispatch(
			_cmd.handle,
			(uint32_t)(this->indirect_draws.drawing_commands.size() / this->_cs.batch_local_size),
			1,
			1);

		// Add memory barrier to ensure that the compute shader has finished writing the indirect command buffer before it's consumed
		_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		_barrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
		_barrier.buffer = _indirect_draws_buffer.handle;
		_barrier.size = _size;
		_barrier.srcQueueFamilyIndex = this->gDevice->vk_compute_queue.index;
		_barrier.dstQueueFamilyIndex = this->gDevice->vk_graphics_queue.index;

		vkCmdPipelineBarrier(
			_cmd.handle,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
			0,
			0, nullptr,
			1, &_barrier,
			0, nullptr);
	}
	this->_cs.command_buffers.end(0);

	return W_PASSED;
}

W_RESULT model_mesh::submit_compute_shader()
{
	W_RESULT _hr = W_PASSED;
	const std::string _trace_info = this->_name + "::submit_compute_shader";

	auto _cam_pos = this->_camera_position;

	auto _model_pos = glm::vec4(get_position(), 1.0f);
	auto _distance_to_camera = glm::distance(_model_pos, _cam_pos);
	const uint32_t _base_offset = 2000;
	auto _max_mip_map_level = this->_textures[0]->get_mip_maps_level();
	
	if (this->_show_only_lod)
	{
		_cam_pos.x += 10000;//set camera to far
		_distance_to_camera = glm::distance(_model_pos, _cam_pos);
	}

	auto _number_of_instances = static_cast<uint32_t>(this->instnaces_transforms.size());
	if (!_number_of_instances)
	{
		if (this->lods_info.size())
		{
			//find lod index for model which has not instnaces
			this->_selected_lod_index = this->lods_info.size() - 1;
			for (uint32_t i = 0; i < this->_selected_lod_index; ++i)
			{
				if (_distance_to_camera <= this->lods_info[i].distance)
				{
					_selected_lod_index = i;
					break;
				}
			}
		}
		else
		{
			this->_selected_lod_index = 0;
		}
		//return W_FAILED, because we do not want to store compute shader's semaphore for this model
		return W_FAILED;
	}

	switch (this->_cs.batch_local_size)
	{
	case 1:
		this->_cs.unifrom_x1->data.camera_pos = _cam_pos;
		std::memcpy(
			&this->_cs.unifrom_x1->data.is_visible[0],
			this->visibilities.data(),
			sizeof(this->_cs.unifrom_x1->data.is_visible));
		_hr = this->_cs.unifrom_x1->update();
		break;
	case 2:
		this->_cs.unifrom_x2->data.camera_pos = _cam_pos;
		std::memcpy(
			&this->_cs.unifrom_x2->data.is_visible[0],
			this->visibilities.data(),
			sizeof(this->_cs.unifrom_x2->data.is_visible));
		_hr = this->_cs.unifrom_x2->update();
		break;
	case 4:
		this->_cs.unifrom_x4->data.camera_pos = _cam_pos;
		std::memcpy(
			&this->_cs.unifrom_x4->data.is_visible[0],
			this->visibilities.data(),
			sizeof(this->_cs.unifrom_x4->data.is_visible));
		_hr = this->_cs.unifrom_x4->update();
		break;
	case 8:
		this->_cs.unifrom_x8->data.camera_pos = _cam_pos;
		std::memcpy(
			&this->_cs.unifrom_x8->data.is_visible[0],
			this->visibilities.data(),
			sizeof(this->_cs.unifrom_x8->data.is_visible));
		_hr = this->_cs.unifrom_x8->update();
		break;
	case 16:
		this->_cs.unifrom_x16->data.camera_pos = _cam_pos;
		std::memcpy(
			&this->_cs.unifrom_x16->data.is_visible[0],
			this->visibilities.data(),
			sizeof(this->_cs.unifrom_x16->data.is_visible));
		_hr = this->_cs.unifrom_x16->update();
		break;
	case 32:
		this->_cs.unifrom_x32->data.camera_pos = _cam_pos;
		std::memcpy(
			&this->_cs.unifrom_x32->data.is_visible[0],
			this->visibilities.data(),
			sizeof(this->_cs.unifrom_x32->data.is_visible));
		_hr = this->_cs.unifrom_x32->update();
		break;
	case 64:
		this->_cs.unifrom_x64->data.camera_pos = _cam_pos;
		std::memcpy(
			&this->_cs.unifrom_x64->data.is_visible[0],
			this->visibilities.data(),
			sizeof(this->_cs.unifrom_x64->data.is_visible));
		_hr = this->_cs.unifrom_x64->update();
		break;
	case 128:
		this->_cs.unifrom_x128->data.camera_pos = _cam_pos;
		std::memcpy(
			&this->_cs.unifrom_x128->data.is_visible[0],
			this->visibilities.data(),
			sizeof(this->_cs.unifrom_x128->data.is_visible));
		_hr = this->_cs.unifrom_x128->update();
		break;
	case 256:
		this->_cs.unifrom_x256->data.camera_pos = _cam_pos;
		std::memcpy(
			&this->_cs.unifrom_x256->data.is_visible[0],
			this->visibilities.data(),
			sizeof(this->_cs.unifrom_x256->data.is_visible));
		_hr = this->_cs.unifrom_x256->update();
		break;
	case 512:
		this->_cs.unifrom_x512->data.camera_pos = _cam_pos;
		std::memcpy(
			&this->_cs.unifrom_x512->data.is_visible[0],
			this->visibilities.data(),
			sizeof(this->_cs.unifrom_x512->data.is_visible));
		_hr = this->_cs.unifrom_x512->update();
		break;
	case 1024:
		this->_cs.unifrom_x1024->data.camera_pos = _cam_pos;
		std::memcpy(
			&this->_cs.unifrom_x1024->data.is_visible[0],
			this->visibilities.data(),
			sizeof(this->_cs.unifrom_x1024->data.is_visible));
		_hr = this->_cs.unifrom_x1024->update();
		break;
	}

	if (_hr == W_FAILED)
	{
		V(_hr, "updating compute shader's unifrom for model: " + this->model_name, _trace_info, 3);
	}

	auto _cmd = this->_cs.command_buffers.get_command_at(0);


	if (this->gDevice->submit(
		{ &_cmd },//command buffers
		this->gDevice->vk_compute_queue, //graphics queue
		{}, //destination masks
		{}, //wait semaphores
		{ this->_cs.semaphore }, //signal semaphores
		nullptr,
		false) == W_FAILED)
	{
		_hr = W_FAILED;
		V(_hr, "submiting compute shader queue for model: " + this->model_name, _trace_info, 3);
	}

	return _hr;
}

W_RESULT model_mesh::draw(_In_ const w_command_buffer& pCommandBuffer)
{
	if (!this->global_visiblity) return W_PASSED;

	const std::string _trace_info = this->_name + "::draw";

	if (!this->_mesh) return W_FAILED;

	//bind pipeline
	this->_pipeline.bind(pCommandBuffer, w_pipeline_bind_point::GRAPHICS);
	if (get_instances_count())
	{
		auto _instance_buffer_handle = this->_instances_buffer.get_buffer_handle();
		return this->_mesh->draw(
			pCommandBuffer, 
			&_instance_buffer_handle,
			this->instnaces_transforms.size(), 
			0, 
			&this->indirect_draws);
	}
	else
	{
		if (this->_selected_lod_index < this->lods_info.size())
		{
			auto _lod_info = &this->lods_info[this->_selected_lod_index];
			return this->_mesh->draw(
				pCommandBuffer,
				nullptr,
				0,
				0,
				nullptr,
				0,
				_lod_info->index_count,
				_lod_info->first_index);
		}
		else
		{
			V(W_FAILED, "drawing model, lod info is out of range for model: " + this->model_name, _trace_info, 3);
			return W_FAILED;
		}
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++

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

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma region create methods

W_RESULT model_mesh::_load_textures()
{
	const std::string _trace_info = this->_name + "_load_textures";

	if (!this->textures_paths.size()) return W_FAILED;

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
				true,
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

	auto _number_of_instances = static_cast<uint32_t>(this->instnaces_transforms.size());
	if (!_number_of_instances) return W_PASSED;

	//set compute shader batch size
	uint32_t _draw_counts = 1 + _number_of_instances;
	//find nearest pow of 2 for compute shader local batch size
	this->_cs.batch_local_size = static_cast<uint32_t>(pow(2, ceil(log(_draw_counts) / log(2))));
	this->indirect_draws.drawing_commands.resize(this->_cs.batch_local_size);

	this->_cs_out_struct.draw_count = _draw_counts;

	for (uint32_t i = 0; i < _draw_counts; ++i)
	{
		this->indirect_draws.drawing_commands[i].instanceCount = 1;
		this->indirect_draws.drawing_commands[i].firstInstance = i;
		//firstIndex and indexCount are written by the compute shader for models which has at least one instance
	}

	//load indirect draws
	if (this->indirect_draws.load(this->gDevice, _draw_counts) == W_FAILED)
	{
		V(W_FAILED, "loading indirect draws command buffer for model: " + this->model_name, _trace_info, 3);
		return W_FAILED;
	}

	//create instance buffers
	if (_create_instance_buffers() == W_FAILED)
	{
		return W_FAILED;
	}

	//create compute shader lod buffer
	if (_create_lod_levels_buffer() == W_FAILED)
	{
		return W_FAILED;
	}

	//create compute shader output buffer
	if (_create_cs_out_buffer() == W_FAILED)
	{
		return W_FAILED;
	}

	return W_PASSED;
}

W_RESULT model_mesh::_create_instance_buffers()
{
	const std::string _trace_info = this->_name + "::_create_instance_buffer";

	auto _number_of_instances = static_cast<uint32_t>(this->instnaces_transforms.size());
	if (!_number_of_instances) return W_PASSED;

	w_buffer _staging_buffers[2];
	defer _(nullptr, [&](...)
	{
		for (size_t i = 0; i < 2; ++i)
		{
			if (_staging_buffers[i].get_is_released()) continue;
			_staging_buffers[i].release();
		}
	});

	auto _draw_counts = 1 + _number_of_instances;

	std::vector<vertex_instance_data> _vertex_instances_data(_draw_counts);
	std::vector<compute_instance_data> _compute_instances_data(_draw_counts);

	//first one is ref model
	int _index = 0;
	_vertex_instances_data[_index].pos[0] = this->transform.position[0];
	_vertex_instances_data[_index].pos[1] = this->transform.position[1];
	_vertex_instances_data[_index].pos[2] = this->transform.position[2];

	_vertex_instances_data[_index].rot[0] = this->transform.rotation[0];
	_vertex_instances_data[_index].rot[1] = this->transform.rotation[1];
	_vertex_instances_data[_index].rot[2] = this->transform.rotation[2];

	_compute_instances_data[_index].pos = glm::vec4(
		this->transform.position[0],
		this->transform.position[1],
		this->transform.position[2],
		1.0f);

	_index++;
	for (auto _ins : this->instnaces_transforms)
	{
		_vertex_instances_data[_index].pos[0] = _ins.position[0];
		_vertex_instances_data[_index].pos[1] = _ins.position[1];
		_vertex_instances_data[_index].pos[2] = _ins.position[2];

		_vertex_instances_data[_index].rot[0] = _ins.rotation[0];
		_vertex_instances_data[_index].rot[1] = _ins.rotation[1];
		_vertex_instances_data[_index].rot[2] = _ins.rotation[2];

		_compute_instances_data[_index].pos = glm::vec4(
			_ins.position[0],
			_ins.position[1],
			_ins.position[2],
			1.0f);

		_index++;
	}

#pragma region create vertex instances buffer
	auto _buffer_size = static_cast<uint32_t>(_draw_counts * sizeof(vertex_instance_data));
	if (_staging_buffers[0].allocate_as_staging(this->gDevice, _buffer_size) == W_FAILED)
	{
		V(W_FAILED, "loading staging buffer of vertex instances buffer", _trace_info, 3);
		return W_FAILED;
	}
	//if (_staging_buffers[0].bind() == W_FAILED)
	//{
	//	V(W_FAILED, "binding to staging buffer of vertex instances buffer", _trace_info, 3);
	//	return W_FAILED;
	//}
	if (_staging_buffers[0].set_data(_vertex_instances_data.data()) == W_FAILED)
	{
		V(W_FAILED, "setting data to staging buffer of vertex instances buffer", _trace_info, 3);
		return W_FAILED;
	}
	if (this->_instances_buffer.allocate(
		this->gDevice,
		_buffer_size,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		w_memory_usage_flag::MEMORY_USAGE_GPU_ONLY) == W_FAILED)
	{
		V(W_FAILED, "loading device buffer of vertex instances buffer", _trace_info, 2);
		return W_FAILED;
	}
	//if (this->_instances_buffer.bind() == W_FAILED)
	//{
	//	V(W_FAILED, "binding to device buffer of vertex instance buffer", _trace_info, 2);
	//	return W_FAILED;
	//}
	if (_staging_buffers[0].copy_to(this->_instances_buffer) == W_FAILED)
	{
		V(W_FAILED, "copying to device buffer of vertex instances buffer", _trace_info, 2);
		return W_FAILED;
	}
	//release buffer
	_staging_buffers[0].release();
#pragma endregion

#pragma region create compute instances buffer
	_buffer_size = _draw_counts * sizeof(compute_instance_data);
	if (_staging_buffers[1].allocate_as_staging(this->gDevice, _buffer_size) == W_FAILED)
	{
		V(W_FAILED, "loading staging buffer of compute instances buffer", _trace_info, 3);
		return W_FAILED;
	}
	//if (_staging_buffers[1].bind() == W_FAILED)
	//{
	//	V(W_FAILED, "binding to staging buffer of compute instances buffer", _trace_info, 3);
	//	return W_FAILED;
	//}
	if (_staging_buffers[1].set_data(_compute_instances_data.data()) == W_FAILED)
	{
		V(W_FAILED, "setting data to staging buffer of compute instances buffer", _trace_info, 2);
		return W_FAILED;
	}
	if (this->_cs.instances_buffer.allocate(
		this->gDevice,
		_buffer_size,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		w_memory_usage_flag::MEMORY_USAGE_GPU_ONLY) == W_FAILED)
	{
		V(W_FAILED, "loading device buffer of compute instances buffer", _trace_info, 2);
		return W_FAILED;
	}
	//if (_cs.instances_buffer.bind() == W_FAILED)
	//{
	//	V(W_FAILED, "binding to device buffer of compute instances buffer", _trace_info, 2);
	//	return W_FAILED;
	//}
	if (_staging_buffers[1].copy_to(_cs.instances_buffer) == W_FAILED)
	{
		V(W_FAILED, "copying to device buffer of compute instances buffer", _trace_info, 2);
		return W_FAILED;
	}
	_staging_buffers[1].release();
#pragma endregion

	return W_PASSED;
}

W_RESULT model_mesh::_create_lod_levels_buffer()
{
	const std::string _trace_info = this->_name + "::_create_lod_levels_buffer";

	auto _number_of_instances = static_cast<uint32_t>(this->instnaces_transforms.size());
	if (!_number_of_instances) return W_PASSED;

	w_buffer _staging_buffer;
	defer _(nullptr, [&](...)
	{		
		_staging_buffer.release();
	});

	auto _size = static_cast<uint32_t>(this->lods_info.size() * sizeof(lod_info));
	if (_staging_buffer.allocate_as_staging(this->gDevice, _size) == W_FAILED)
	{
		V(W_FAILED, "loading staging buffer for lod levels buffer", _trace_info, 3);
		return W_FAILED;
	}
	//if (_staging_buffer.bind() == W_FAILED)
	//{
	//	V(W_FAILED, "binding staging buffer for lod levels buffer", _trace_info, 3);
	//	return W_FAILED;
	//}
	if (_staging_buffer.set_data(this->lods_info.data()))
	{
		V(W_FAILED, "setting data to staging buffer of lod levels buffer", _trace_info, 3);
		return W_FAILED;
	}
	if (this->_cs.lod_levels_buffer.allocate(
		this->gDevice,
		_size,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		w_memory_usage_flag::MEMORY_USAGE_GPU_ONLY) == W_FAILED)
	{
		V(W_FAILED, "loading data to staging buffer of lod levels buffer", _trace_info, 3);
		return W_FAILED;
	}
	//if (this->_cs.lod_levels_buffer.bind() == W_FAILED)
	//{
	//	V(W_FAILED, "binding to device buffer of lod levels buffer", _trace_info, 3);
	//	return W_FAILED;
	//}
	if (_staging_buffer.copy_to(this->_cs.lod_levels_buffer) == W_FAILED)
	{
		V(W_FAILED, "copy staging buffer to device buffer of lod levels buffer", _trace_info, 3);
		return W_FAILED;
	}

	return W_PASSED;

}

W_RESULT model_mesh::_create_cs_out_buffer()
{
	const std::string _trace_info = this->_name + "::_create_cs_out_buffer";

	auto _number_of_instances = static_cast<uint32_t>(this->instnaces_transforms.size());
	if (!_number_of_instances) return W_PASSED;

	//create buffer of compute stage output
	auto _size = (uint32_t)sizeof(compute_stage_output);
	if (this->_cs_out_buffer.allocate(
		this->gDevice,
		_size,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		w_memory_usage_flag::MEMORY_USAGE_GPU_TO_CPU) == W_FAILED)
	{
		V(W_FAILED, "loading compute shader stage output buffer for model: " + this->model_name, _trace_info, 3);
		return W_FAILED;
	}
	//if (this->_cs_out_buffer.bind() == W_FAILED)
	//{
	//	V(W_FAILED, "binding to compute shader stage output buffer for model: " + this->model_name, _trace_info, 3);
	//	return W_FAILED;
	//}

	return W_PASSED;
}

W_RESULT model_mesh::_prepare_cs_path_uniform_based_on_local_size(
	_Inout_ w_shader_binding_param& pShaderBindingParam,
	_Inout_ std::wstring& pComputeShaderPath)
{
	const std::string _trace_info = this->_name + "::_prepare_compute_shader_based_on_batch_local_size";

	auto _lod_level = this->lods_info.size() ? this->lods_info.size() - 1 : 0;
	pComputeShaderPath = L"lod_" + std::to_wstring(_lod_level) + L"_local_size_x" + std::to_wstring(this->_cs.batch_local_size) + L".comp.spv";
	
	auto _hr = W_PASSED;
	switch (this->_cs.batch_local_size)
	{
	default:
		_hr = W_FAILED;
		V(_hr, "batch_local_size " + std::to_string(this->_cs.batch_local_size) +
			" not supported for model: " + this->model_name, _trace_info);
	case 2:
		this->visibilities.resize(1);
		this->_cs.unifrom_x2 = new w_uniform<compute_unifrom_x2>();
		if (this->_cs.unifrom_x2->load(this->gDevice) == W_FAILED)
		{
			_hr = W_FAILED;
			V(_hr, "loading compute shader unifrom_x2 for " + this->model_name, _trace_info);
		}
		else
		{
			pShaderBindingParam.buffer_info = this->_cs.unifrom_x2->get_descriptor_info();
		}
		break;
	case 4:
		this->visibilities.resize(1);
		this->_cs.unifrom_x4 = new w_uniform<compute_unifrom_x4>();
		if (this->_cs.unifrom_x4->load(this->gDevice) == W_FAILED)
		{
			_hr = W_FAILED;
			V(_hr, "loading compute shader unifrom_x4 for " + this->model_name, _trace_info);
		}
		else
		{
			pShaderBindingParam.buffer_info = this->_cs.unifrom_x4->get_descriptor_info();
		}
		break;
	case 8:
		this->visibilities.resize(8 / 4);
		this->_cs.unifrom_x8 = new w_uniform<compute_unifrom_x8>();
		if (this->_cs.unifrom_x8->load(this->gDevice) == W_FAILED)
		{
			_hr = W_FAILED;
			V(_hr, "loading compute shader unifrom_x8 for " + this->model_name, _trace_info);
		}
		else
		{
			pShaderBindingParam.buffer_info = this->_cs.unifrom_x8->get_descriptor_info();
		}
		break;
	case 16:
		this->visibilities.resize(16 / 4);
		this->_cs.unifrom_x16 = new w_uniform<compute_unifrom_x16>();
		if (this->_cs.unifrom_x16->load(this->gDevice) == W_FAILED)
		{
			_hr = W_FAILED;
			V(_hr, "loading compute shader unifrom_x16 for " + this->model_name, _trace_info);
		}
		else
		{
			pShaderBindingParam.buffer_info = this->_cs.unifrom_x16->get_descriptor_info();
		}
		break;
	case 32:
		this->visibilities.resize(32 / 4);
		this->_cs.unifrom_x32 = new w_uniform<compute_unifrom_x32>();
		if (this->_cs.unifrom_x32->load(this->gDevice) == W_FAILED)
		{
			_hr = W_FAILED;
			V(_hr, "loading compute shader unifrom_x32 for " + this->model_name, _trace_info);
		}
		else
		{
			pShaderBindingParam.buffer_info = this->_cs.unifrom_x32->get_descriptor_info();
		}
		break;
	case 64:
		this->visibilities.resize(64 / 4);
		this->_cs.unifrom_x64 = new w_uniform<compute_unifrom_x64>();
		if (this->_cs.unifrom_x64->load(this->gDevice) == W_FAILED)
		{
			_hr = W_FAILED;
			V(_hr, "loading compute shader unifrom_x64 for " + this->model_name, _trace_info);
		}
		else
		{
			pShaderBindingParam.buffer_info = this->_cs.unifrom_x64->get_descriptor_info();
		}
		break;
	case 128:
		this->visibilities.resize(128 / 4);
		this->_cs.unifrom_x128 = new w_uniform<compute_unifrom_x128>();
		if (this->_cs.unifrom_x128->load(this->gDevice) == W_FAILED)
		{
			_hr = W_FAILED;
			V(_hr, "loading compute shader uniform_x128 for " + this->model_name, _trace_info);
		}
		else
		{
			pShaderBindingParam.buffer_info = this->_cs.unifrom_x128->get_descriptor_info();
		}
		break;
	case 256:
		this->visibilities.resize(256 / 4);
		this->_cs.unifrom_x256 = new w_uniform<compute_unifrom_x256>();
		if (this->_cs.unifrom_x256->load(this->gDevice) == S_FALSE)
		{
			_hr = W_FAILED;
			V(_hr, "loading compute shader uniform_x256 for " + this->model_name, _trace_info);
		}
		else
		{
			pShaderBindingParam.buffer_info = this->_cs.unifrom_x256->get_descriptor_info();
		}
		break;
	case 512:
		this->visibilities.resize(512 / 4);
		this->_cs.unifrom_x512 = new w_uniform<compute_unifrom_x512>();
		if (this->_cs.unifrom_x512->load(this->gDevice) == S_FALSE)
		{
			_hr = W_FAILED;
			V(_hr, "loading compute shader uniform_x512 for " + this->model_name, _trace_info);
		}
		else
		{
			pShaderBindingParam.buffer_info = this->_cs.unifrom_x512->get_descriptor_info();
		}
		break;
	case 1024:
		this->visibilities.resize(1024 / 4);
		this->_cs.unifrom_x1024 = new w_uniform<compute_unifrom_x1024>();
		if (this->_cs.unifrom_x1024->load(this->gDevice) == S_FALSE)
		{
			_hr = W_FAILED;
			V(_hr, "loading compute shader uniform_x512 for " + this->model_name, _trace_info);
		}
		else
		{
			pShaderBindingParam.buffer_info = this->_cs.unifrom_x1024->get_descriptor_info();
		}
		break;
	}

	return _hr;
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
			V(W_FAILED, "loading vertex shader instance uniform for model: " + this->model_name, _trace_info, 3);
			return W_FAILED;
		}
		_shader_param.buffer_info = this->_instance_u0.get_descriptor_info();
	}
	else
	{
		_hr = this->_basic_u0.load(this->gDevice);
		if (_hr == W_FAILED)
		{
			V(W_FAILED, "loading vertex shader basic uniform for model: " + this->model_name, _trace_info, 3);
			return W_FAILED;
		}
		_shader_param.buffer_info = this->_basic_u0.get_descriptor_info();
	}
	_shader_params.push_back(_shader_param);

	//The texture lod index
	_hr = this->_u1.load(this->gDevice);
	if (_hr == W_FAILED)
	{
		V(W_FAILED, "loading vertex shader uniform 1 for model: " + this->model_name, _trace_info, 3);
		return W_FAILED;
	}
	_shader_param.index = 1;
	_shader_param.type = w_shader_binding_type::UNIFORM;
	_shader_param.stage = w_shader_stage_flag_bits::VERTEX_SHADER;
	_shader_param.buffer_info = this->_u1.get_descriptor_info();
	_shader_params.push_back(_shader_param);

	//The sampler2D of fragment shader
	_shader_param.index = 2;
	_shader_param.type = w_shader_binding_type::SAMPLER2D;
	_shader_param.stage = w_shader_stage_flag_bits::FRAGMENT_SHADER;
	_shader_param.image_info = this->_textures[0]->get_descriptor_info(w_sampler_type::MIPMAP_AND_ANISOTROPY);
	_shader_params.push_back(_shader_param);

	//the U1 uniform of fragment shader
	_hr = this->_u2.load(this->gDevice);
	if (_hr == W_FAILED)
	{
		V(W_FAILED, "loading fragment shader uniform 2 for model: " + this->model_name, _trace_info, 3);
		return W_FAILED;
	}
	//draw sky without applying light and mipmaps
	if (this->_is_sky)
	{
		this->_u2.data.cmds = 2;
		auto _hr = this->_u2.update();
		if (_hr == W_FAILED)
		{
			V(W_FAILED, "updating uniform u2(cmds) for sky: " + this->model_name, _trace_info, 3);
		}
	}

	_shader_param.index = 3;
	_shader_param.type = w_shader_binding_type::UNIFORM;
	_shader_param.stage = w_shader_stage_flag_bits::FRAGMENT_SHADER;
	_shader_param.buffer_info = this->_u2.get_descriptor_info();
	_shader_params.push_back(_shader_param);

	//the following shader parameters will be added for models which have at least one instance
	auto _number_of_instances = static_cast<uint32_t>(this->instnaces_transforms.size());
	if (_number_of_instances)
	{
		_shader_param.index = 0;
		_shader_param.type = w_shader_binding_type::STORAGE;
		_shader_param.stage = w_shader_stage_flag_bits::COMPUTE_SHADER;
		_shader_param.buffer_info = this->_cs.instances_buffer.get_descriptor_info();
		_shader_params.push_back(_shader_param);

		_shader_param.index = 1;
		_shader_param.type = w_shader_binding_type::STORAGE;
		_shader_param.stage = w_shader_stage_flag_bits::COMPUTE_SHADER;
		_shader_param.buffer_info = this->indirect_draws.buffer.get_descriptor_info();
		_shader_params.push_back(_shader_param);

		_shader_param.index = 2;
		_shader_param.type = w_shader_binding_type::UNIFORM;
		_shader_param.stage = w_shader_stage_flag_bits::COMPUTE_SHADER;

		std::wstring _compute_shader_path;
		if (_prepare_cs_path_uniform_based_on_local_size(_shader_param, _compute_shader_path) == W_FAILED)
		{
			V(W_FAILED, "getting compute shader uniform and filename path for model: " + this->model_name, _trace_info, 3);
			return W_FAILED;
		}
		_shader_params.push_back(_shader_param);

		_shader_param.index = 3;
		_shader_param.type = w_shader_binding_type::STORAGE;
		_shader_param.stage = w_shader_stage_flag_bits::COMPUTE_SHADER;
		_shader_param.buffer_info = this->_cs_out_buffer.get_descriptor_info();
		_shader_params.push_back(_shader_param);

		_shader_param.index = 4;
		_shader_param.type = w_shader_binding_type::STORAGE;
		_shader_param.stage = w_shader_stage_flag_bits::COMPUTE_SHADER;
		_shader_param.buffer_info = this->_cs.lod_levels_buffer.get_descriptor_info();
		_shader_params.push_back(_shader_param);

		//load shaders
		if (w_shader::load_shader(
			this->gDevice,
			"model_mesh_" + wolf::system::convert::wstring_to_string(_compute_shader_path),
			pVertexShaderPath,
			L"",
			L"",
			L"",
			pFragmentShaderPath,
			shared::scene_content_path + L"shaders/compute/" + _compute_shader_path,
			_shader_params,
			false,
			&this->_shader) == W_FAILED)
		{
			V(W_FAILED, "loading shader module for model: " + this->model_name, _trace_info, 3);
			return W_FAILED;
		}
	}
	else
	{
		//load shaders
		if (w_shader::load_shader(
			this->gDevice,
			"model_basic_mesh",
			pVertexShaderPath,
			L"",
			L"",
			L"",
			pFragmentShaderPath,
			L"",
			_shader_params,
			false,
			&this->_shader) == W_FAILED)
		{
			V(W_FAILED, "loading shader module for model: " + this->model_name, _trace_info, 3);
			return W_FAILED;
		}
	}

	_hr = this->_shader->set_shader_binding_params(_shader_params);
	if (_hr == W_FAILED)
	{
		V(W_FAILED, "setting shader binding param for model: " + this->model_name, _trace_info, 3);
		return W_FAILED;
	}
	_shader_params.clear();

	return W_PASSED;
}

W_RESULT model_mesh::_create_pipelines(
	_In_z_ const std::string& pComputePipelineCacheName,
	_In_ const w_render_pass& pRenderPass)
{
	const std::string _trace_info = this->_name + "_create_pipelines";

	if (this->_pipeline.load(
		this->gDevice,
		this->vertex_binding_attributes,
		w_primitive_topology::TRIANGLE_LIST,
		&pRenderPass,
		this->_shader,
		{ pRenderPass.get_viewport() },
		{ pRenderPass.get_viewport_scissor() }) == W_FAILED)
	{
		V(W_FAILED, "loading drawing pipeline for model: " + this->model_name, _trace_info, 3);
		return W_FAILED;
	}

	auto _number_of_instances = static_cast<uint32_t>(this->instnaces_transforms.size());
	if (_number_of_instances)
	{
		if (this->_cs.pipeline.load_compute(
			this->gDevice,
			this->_shader,
			5,
			pComputePipelineCacheName) == W_FAILED)
		{
			V(W_FAILED, "loading computing pipeline for model: " + this->model_name, _trace_info, 3);
			return W_FAILED;
		}
	}

	return W_PASSED;
}

#pragma endregion

ULONG model_mesh::release()
{
	if (_super::get_is_released()) return 1;

	this->_name.clear();
	this->model_name.clear();

	this->instnaces_transforms.clear();

	if (this->c_model)
	{
		this->c_model->release();
		this->c_model = nullptr;
	}

	this->_pipeline.release();
	SAFE_RELEASE(this->_shader);

	this->_basic_u0.release();
	this->_instance_u0.release();
	this->_u1.release();
	this->_u2.release();

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

	this->_cs.release();

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

const uint32_t model_mesh::get_instances_count() const
{
	return static_cast<uint32_t>(this->instnaces_transforms.size());
}

w_bounding_box model_mesh::get_global_bounding_box() const
{
	return this->merged_bounding_box;
}

bool model_mesh::get_enable_instances_colors() const
{
	return this->_u2.data.cmds == 1;
}

bool model_mesh::get_global_visiblity() const
{
	return this->global_visiblity;
}

bool model_mesh::get_visiblity(_In_ const uint32_t& pModelInstanceIndex) const
{
	auto _size = this->visibilities.size();

	uint32_t _base_index = pModelInstanceIndex % 4;
	uint32_t _sec_index = pModelInstanceIndex - _base_index * 4;

	return pModelInstanceIndex < _size ?
		this->visibilities[_base_index][_sec_index] == 1.0f : false;
}

w_semaphore* model_mesh::get_compute_semaphore()
{
	return &(this->_cs.semaphore);
}

compute_stage_output model_mesh::get_result_of_compute_shader()
{
	// Get draw count from compute
	auto _mapped = this->_cs_out_buffer.map();
	if (_mapped)
	{
		memcpy(&this->_cs_out_struct, _mapped, sizeof(compute_stage_output));
	}
	this->_cs_out_buffer.unmap();
	_mapped = nullptr;

	return this->_cs_out_struct;
}

#pragma endregion

#pragma region Setters

void model_mesh::set_view_projection_position(
	_In_ const glm::mat4& pView,
	_In_ const glm::mat4& pProjection,
	_In_ const glm::vec3& pPosition)
{
	const std::string _trace_info = this->_name + "::set_view_projection";

	this->_camera_position = glm::vec4(pPosition, 1.0f);
	if (this->instnaces_transforms.size())
	{
		this->_instance_u0.data.view = pView;
		this->_instance_u0.data.projection = pProjection;
		this->_instance_u0.data.camera_pos = this->_camera_position;

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
		this->_basic_u0.data.camera_pos = glm::vec4(pPosition, 1.0f);

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

	this->_u2.data.cmds = pEnable ? 1 : 0;
	auto _hr = this->_u2.update();
	if (_hr == W_FAILED)
	{
		V(W_FAILED, "updating uniform u2(cmds) for model: " + this->model_name, _trace_info, 3);
	}
}

void model_mesh::set_global_visiblity(_In_ const bool& pValue)
{
	this->global_visiblity = pValue ? 1.0f : 0.0f;
	std::fill(this->visibilities.begin(), this->visibilities.end(), glm::vec4(this->global_visiblity));
}

void model_mesh::set_visiblity(_In_ const bool& pValue, _In_ const uint32_t& pModelInstanceIndex)
{
	uint32_t _base_index = pModelInstanceIndex % 4;

	if (_base_index < this->visibilities.size())
	{
		uint32_t _sec_index = pModelInstanceIndex - _base_index * 4;
		this->visibilities[_base_index][_sec_index] = pValue ? 1.0f : 0.0f;
	}
}

void model_mesh::set_show_only_lods(_In_ const bool& pValue)
{
	this->_show_only_lod = pValue;
}

#pragma endregion

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++


