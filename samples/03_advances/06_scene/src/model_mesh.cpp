//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "pch.h"
#include "model_mesh.h"
#include <vulkan/w_shader.h>
#include <vulkan/w_pipeline.h>
#include <vulkan/w_uniform.h>

using namespace wolf::system;
using namespace wolf::render::vulkan;
using namespace wolf::content_pipeline;

class model_mesh_pimp
{
public:
	model_mesh_pimp(
		_In_ w_cpipeline_model* pContentPipelineModel,
		_In_ w_vertex_binding_attributes pVertexBindingAttributes) :
		_vertex_binding_attributes(pVertexBindingAttributes),
		_name("model"),
		_mesh(nullptr),
		_c_model(pContentPipelineModel),
		_visible(true),
        _transform(nullptr)
	{
	}

	W_RESULT load(
		_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
		_In_z_ const std::string& pPipelineCacheName, 
		_In_z_ const std::wstring& pVertexShaderPath,
		_In_z_ const std::wstring& pFragmentShaderPath,
		_In_ const w_render_pass& pRenderPass,
		_In_ const w_viewport& pViewPort,
		_In_ const w_viewport_scissor& pViewPortScissor)
	{
		if (!pGDevice || !_c_model) return W_FAILED;
		this->_gDevice = pGDevice;

		const std::string _trace_info = this->_name + "::load";

		this->_model_name = _c_model->get_name();
		this->_transform = _c_model->get_transform();


		std::vector<w_cpipeline_mesh*> _meshes;

		//get all meshes
		_c_model->get_meshes(_meshes);
		//get all instances
		_c_model->get_instances(this->_instnaces_transforms);
		
#pragma region collada from 3DMax 

		//3DMax is right handed Zup
		this->_transform->rotation[0] *= -1.0f;
		this->_transform->rotation[0] += glm::radians(90.0f);
		std::swap(this->_transform->position[1], this->_transform->position[2]);
		this->_transform->position[1] *= -1.0f;

		for (auto& _ins : this->_instnaces_transforms)
		{
			_ins.rotation[0] *= -1.0f;
			_ins.rotation[0] += glm::radians(90.0f);
			std::swap(_ins.position[1], _ins.position[2]);
			_ins.position[1] *= -1.0f;
		}

#pragma endregion 

		//get size of mesh
		size_t _meshes_count = _meshes.size();
		if (!_meshes_count)
		{
			V(W_FAILED,
				w_log_type::W_WARNING,
				"model {} does not have any mesh. graphics device: {} . trace info: {}", 
				this->_model_name, _gDevice->get_info(), _trace_info);
			return W_FAILED;
		}
		//prepare vertices and indices
		uint32_t _base_vertex_index = 0;
		std::vector<float> _batch_vertices;
		std::vector<uint32_t> _batch_indices;

		_store_to_batch(
			_meshes,
			_vertex_binding_attributes,
			_base_vertex_index,
			_batch_vertices,
			_batch_indices,
			this->_merged_bounding_box,
			this->_sub_meshes_bounding_box,
			this->_textures_paths);

		_meshes.clear();
		
		//create mesh
		this->_mesh = new (std::nothrow) w_mesh();
		if (!this->_mesh)
		{
			V(W_FAILED,
				w_log_type::W_WARNING,
				"allocating memory for w_mesh for model: {} . graphics device: {} . trace info: {}",
				this->_model_name, _gDevice->get_info(), _trace_info);
			_batch_vertices.clear();
			_batch_indices.clear();
			return W_FAILED;
		}
		_mesh->set_vertex_binding_attributes(_vertex_binding_attributes);
		
		//load all textures
		if (_load_textures() == W_PASSED)
		{
			//TODO: currently we are using one texture per mesh
			_mesh->set_texture(this->_textures[0]);
		}
		else
		{
			//set the default texture
			this->_textures.push_back(w_texture::default_texture);
			wolf::logger.warning("default texture will be used for model: {}", this->_model_name);
			_mesh->set_texture(this->_textures[0]);
		}

		//load mesh
		auto _v_size = static_cast<uint32_t>(_batch_vertices.size());
		auto _i_size = static_cast<uint32_t>(_batch_indices.size());

		auto _hr = _mesh->load(
			_gDevice,
			_batch_vertices.data(),
			static_cast<uint32_t>(_batch_vertices.size() * sizeof(float)),
			_v_size,
			_batch_indices.data(),
			_i_size);

		//clear vectors
		_batch_vertices.clear();
		_batch_vertices.clear();

		if (_hr == W_FAILED)
		{
			V(W_FAILED,
				w_log_type::W_WARNING,
				"loading mesh for model: {} . graphics device: {} . trace info: {}", 
				this->_model_name, _gDevice->get_info(), _trace_info);
			return W_FAILED;
		}

#pragma region create instnace buffer
		
		auto _instances_size = this->_instnaces_transforms.size();
		if (_instances_size)
		{
			auto _vertex_dec = _vertex_binding_attributes.binding_attributes.find(1);
			if (_vertex_dec != _vertex_binding_attributes.binding_attributes.end())
			{
				//set instance data
				uint32_t _size_of_instance_struct = 0;
				std::vector<float> _instances_data;

				//first one is ref model
				for (auto _dec : _vertex_dec->second)
				{
					if (_dec == w_vertex_attribute::W_POS)
					{
						_instances_data.push_back(this->_transform->position[0]);
						_instances_data.push_back(this->_transform->position[1]);
						_instances_data.push_back(this->_transform->position[2]);
						_size_of_instance_struct += 12;
					}
					if (_dec == w_vertex_attribute::W_ROT)
					{
						_instances_data.push_back(this->_transform->rotation[0]);
						_instances_data.push_back(this->_transform->rotation[1]);
						_instances_data.push_back(this->_transform->rotation[2]);
						_size_of_instance_struct += 12;
					}
					else if (_dec == w_vertex_attribute::W_SCALE)
					{
						_instances_data.push_back(this->_transform->scale[0]);
						_size_of_instance_struct += 4;
					}
				}

				//others are instances
				for (auto _ins : this->_instnaces_transforms)
				{
					for (auto _dec : _vertex_dec->second)
					{
						if (_dec == w_vertex_attribute::W_POS)
						{
							_instances_data.push_back(_ins.position[0]);
							_instances_data.push_back(_ins.position[1]);
							_instances_data.push_back(_ins.position[2]);
						}
						if (_dec == w_vertex_attribute::W_ROT)
						{
							_instances_data.push_back(_ins.rotation[0]);
							_instances_data.push_back(_ins.rotation[1]);
							_instances_data.push_back(_ins.rotation[2]);
						}
						else if (_dec == w_vertex_attribute::W_SCALE)
						{
							_instances_data.push_back(_ins.scale[0]);
						}
					}
				}

				auto _size_of_buffer = static_cast<uint32_t>((_instances_size + 1) * _size_of_instance_struct);
				_hr = _create_instance_buffer(_instances_data, _size_of_buffer);
				_instances_data.clear();

				if (_hr == W_FAILED)
				{
					V(W_FAILED,
						w_log_type::W_WARNING,
						"creating instance buffer for model: {} . graphics device: {} . trace info: {}",
						this->_model_name, _gDevice->get_info(), _trace_info);
					return W_FAILED;
				}
			}
		}
		
#pragma endregion

#pragma region create shader module
		if (_create_shader_module(pVertexShaderPath, pFragmentShaderPath) == W_FAILED)
		{
			V(W_FAILED,
				w_log_type::W_ERROR,
				"creating shader for model: {} . graphics device: {} . trace info: {}", 
				this->_model_name, _gDevice->get_info(), _trace_info);
			return W_FAILED;
		}
#pragma endregion

#pragma region create pipeline
		if (_create_pipeline(pPipelineCacheName, pRenderPass, pViewPort, pViewPortScissor) == W_FAILED)
		{
			V(W_FAILED,
				w_log_type::W_ERROR,
				"creating pipeline for model: {} . graphics device: {} . trace info: {}", 
				this->_model_name, _gDevice->get_info(), _trace_info);
			return W_FAILED;
		}
#pragma endregion

		//release content pipeline model
		this->_c_model->release();
		this->_c_model = nullptr;

		return W_PASSED;
	}

	W_RESULT draw(_In_ const w_command_buffer& pCommandBuffer, _In_ const bool& pInDirectMode)
	{
		if (!this->_visible) return W_PASSED;

		const std::string _trace_info = this->_name + "::draw";

		if (!this->_mesh) return W_FAILED;

		//bind pipeline
		this->_pipeline.bind(pCommandBuffer, w_pipeline_bind_point::GRAPHICS);
		auto _buffer_handle = this->_instances_buffer.get_buffer_handle();
		return this->_mesh->draw(pCommandBuffer, _buffer_handle.handle ? &_buffer_handle : nullptr, this->_instnaces_transforms.size(), pInDirectMode);
	}
	
	void release()
	{
		this->_name.clear();
		this->_model_name.clear();

		this->_instnaces_transforms.clear();

		this->_pipeline.release();
		this->_shader.release();

		this->_basic_u0.release();
		this->_instance_u0.release();
		this->_u1.release();

		//release mesh resources
		SAFE_RELEASE(this->_mesh);
		this->_instances_buffer.release();
		this->_sub_meshes_bounding_box.clear();

		//release textures
		this->_textures_paths.clear();
		for (auto _t : this->_textures)
		{
			if (_t) _t->release();
		}
		this->_textures.clear();

		_gDevice = nullptr;
	}

#pragma region Setters

	void set_view_projection(_In_ const glm::mat4& pView, _In_ const glm::mat4& pProjection)
	{
		const std::string _trace_info = this->_name + "::set_view_projection";

		if (this->_instnaces_transforms.size())
		{
			this->_instance_u0.data.view = pView;
			this->_instance_u0.data.projection = pProjection;
			auto _hr = this->_instance_u0.update();
			if (_hr == W_FAILED)
			{
				V(W_FAILED,
					w_log_type::W_ERROR,
					"updating instance uniform ViewProjection for model: {} . graphics device: {} . trace info: {}", 
					this->_model_name, _gDevice->get_info(), _trace_info);
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
				V(W_FAILED,
					w_log_type::W_ERROR,
					"updating basic uniform ViewProjection for model: {} . graphics device: {} . trace info: {}",
					this->_model_name, _gDevice->get_info(), _trace_info);
			}
		}
	}

	void set_enable_instances_colors(_In_ const bool& pEnable)
	{
		const std::string _trace_info = this->_name + "::set_enable_instances_colors";

		this->_u1.data.cmds = pEnable ? 1 : 0;
		auto _hr = this->_u1.update();
		if (_hr == W_FAILED)
		{
			V(W_FAILED,
				w_log_type::W_ERROR,
				"updating u1 uniform for model: {} . graphics device: {} . trace info: {}",
				this->_model_name, _gDevice->get_info(), _trace_info);
		}
	}

	void set_visible(_In_ const bool& pValue)
	{
		this->_visible = pValue;
	}

#pragma endregion

#pragma region Getters

	std::string get_model_name() const
	{
		return this->_model_name;
	}

	glm::vec3 get_position() const
	{
		return glm::vec3(this->_transform->position[0], this->_transform->position[1], this->_transform->position[2]);
	}

	glm::vec3 get_rotation() const
	{
		return glm::vec3(this->_transform->rotation[0], this->_transform->rotation[1], this->_transform->rotation[2]);
	}

	glm::vec3 get_scale() const
	{
		return glm::vec3(this->_transform->scale[0], this->_transform->scale[1], this->_transform->scale[2]);
	}

	std::vector<w_instance_info> get_instances() const
	{
		return this->_instnaces_transforms;
	}

	w_bounding_box get_global_bounding_box() const
	{
		return this->_merged_bounding_box;
	}

	bool get_enable_instances_colors() const
	{
		return this->_u1.data.cmds == 1;
	}

	bool get_visible() const
	{
		return this->_visible;
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
			//check for finding uv index
			int _texture_uv_index = 0;
			auto _find = _textures_index.find(_mesh_data->textures_path);
			if (_textures_index.empty() && _find == _textures_index.end())
			{
				_texture_uv_index = _texture_index;
				_textures_index.insert({ _mesh_data->textures_path, _texture_index++ });
				pTexturePathsToBeLoad.push_back(_mesh_data->textures_path);
			}
			else
			{
				_texture_uv_index = _find->second;
			}

			//set bounding boxes
			pMergedBoundingBox.merge(_mesh_data->bounding_box);
			pSubMeshBoundingBoxes.push_back(_mesh_data->bounding_box);

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

	W_RESULT _load_textures()
	{
		const std::string _trace_info = this->_name + "_load_textures";

		bool _problem = false;
		for (auto& _path : this->_textures_paths)
		{
            //load texture from sponza folder
            auto _file_path = wolf::system::io::get_file_name(_path);
            auto _index =  _file_path.find("_");
            auto _texture_name = _file_path.substr(_index + 1, _file_path.size() - _index);
            
			auto _texture = new (std::nothrow) w_texture();
			if (_texture)
			{
				if (w_texture::load_to_shared_textures(
					this->_gDevice,
					wolf::content_path + L"models/sponza/textures/" +
					wolf::system::convert::string_to_wstring(_texture_name),
					false,
					&_texture) == W_PASSED)
				{
					this->_textures.push_back(_texture);
				}
				else
				{
					V(W_FAILED,
						w_log_type::W_ERROR,
						"loading texture\"{}\" for model: {} . graphics device: {} . trace info: {}",
						_path, this->_model_name, _gDevice->get_info(), _trace_info);

					_problem = true;
					//release texture
					SAFE_DELETE(_texture);
				}
			}
			else
			{
				V(W_FAILED,
					w_log_type::W_ERROR,
					"allocating memory of texture\"{}\" for model: {} . graphics device: {} . trace info: {}",
					_path, this->_model_name, _gDevice->get_info(), _trace_info);
				_problem = true;
			}
		}
		return _problem ? W_FAILED : W_PASSED;
	}

	W_RESULT _create_instance_buffer(_In_ const std::vector<float>& pData, _In_ uint32_t& pSizeOfBuffer)
	{
		const std::string _trace_info = this->_name + "::_create_instance_buffer";

		w_buffer _staging_buffers;

		_staging_buffers.allocate_as_staging(_gDevice, pSizeOfBuffer);
		//if (_staging_buffers.bind() == W_FAILED)
		//{
		//	V(W_FAILED, "binding to staging buffer of vertex_instance_buffer", _trace_info, 2);
		//	return W_FAILED;
		//}

		if (_staging_buffers.set_data(pData.data()) == W_FAILED)
		{
			V(W_FAILED,
				w_log_type::W_ERROR,
				"setting data to staging buffer of vertex_instance_buffer for model: {} . graphics device: {} . trace info: {}",
				this->_model_name, _gDevice->get_info(), _trace_info);
			return W_FAILED;
		}

		if (this->_instances_buffer.allocate(
			_gDevice,
			pSizeOfBuffer,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			w_memory_usage_flag::MEMORY_USAGE_GPU_ONLY) == W_FAILED)
		{
			V(W_FAILED,
				w_log_type::W_ERROR,
				"loading device buffer of vertex_instance_buffer for model: {} . graphics device: {} . trace info: {}",
				this->_model_name, _gDevice->get_info(), _trace_info);
			return W_FAILED;
		}

		//if (this->_instances_buffer.bind() == W_FAILED)
		//{
		//	V(W_FAILED, "binding to device buffer of vertex_instance_buffer", _trace_info, 2);
		//	return W_FAILED;
		//}
		if (_staging_buffers.copy_to(this->_instances_buffer) == W_FAILED)
		{
			V(W_FAILED,
				w_log_type::W_ERROR,
				"copying to device buffer of vertex_instance_buffer for model: {} . graphics device: {} . trace info: {}",
				this->_model_name, _gDevice->get_info(), _trace_info);
			return W_FAILED;
		}
		_staging_buffers.release();

		return W_PASSED;
	}

	W_RESULT _create_shader_module(
		_In_z_ const std::wstring& pVertexShaderPath,
		_In_z_ const std::wstring& pFragmentShaderPath)
	{
		const std::string _trace_info = this->_name + "_create_shader_module";

		//loading vertex shaders
		auto _hr = this->_shader.load(_gDevice,
			pVertexShaderPath,
			w_shader_stage_flag_bits::VERTEX_SHADER);
		if (_hr == W_FAILED)
		{
			this->_shader.release();
			V(W_FAILED,
				w_log_type::W_ERROR,
				"loading vertex shader for model: {} . graphics device: {} . trace info: {}",
				this->_model_name, _gDevice->get_info(), _trace_info);
			return W_FAILED;
		}

		//loading fragment shader
		_hr = this->_shader.load(_gDevice,
			pFragmentShaderPath,
			w_shader_stage_flag_bits::FRAGMENT_SHADER);
		if (_hr == W_FAILED)
		{
			this->_shader.release();
			V(W_FAILED,
				w_log_type::W_ERROR,
				"loading fragment shader for model: {} . graphics device: {} . trace info: {}",
				this->_model_name, _gDevice->get_info(), _trace_info);
			return W_FAILED;
		}

		std::vector<w_shader_binding_param> _shader_params;

		w_shader_binding_param _shader_param;
		_shader_param.index = 0;
		_shader_param.type = w_shader_binding_type::UNIFORM;
		_shader_param.stage = w_shader_stage_flag_bits::VERTEX_SHADER;
		
		//load vertex shader uniform
		if (this->_instnaces_transforms.size())
		{
			_hr = this->_instance_u0.load(_gDevice);
			if (_hr == W_FAILED)
			{
				this->_shader.release();
				V(W_FAILED,
					w_log_type::W_ERROR,
					"loading vertex shader instance uniform for model: {} . graphics device: {} . trace info: {}",
					this->_model_name, _gDevice->get_info(), _trace_info);
				return W_FAILED;
			}
			_shader_param.buffer_info = this->_instance_u0.get_descriptor_info();
		}
		else
		{
			_hr = this->_basic_u0.load(_gDevice);
			if (_hr == W_FAILED)
			{
				this->_shader.release();
				V(W_FAILED,
					w_log_type::W_ERROR,
					"loading vertex shader basic uniform for model: {} . graphics device: {} . trace info: {}",
					this->_model_name, _gDevice->get_info(), _trace_info);
				return W_FAILED;
			}
			_shader_param.buffer_info = this->_basic_u0.get_descriptor_info();
		}

		_shader_params.push_back(_shader_param);

		_shader_param.index = 1;
		_shader_param.type = w_shader_binding_type::SAMPLER2D;
		_shader_param.stage = w_shader_stage_flag_bits::FRAGMENT_SHADER;
		_shader_param.image_info = this->_textures[0]->get_descriptor_info();

		_shader_params.push_back(_shader_param);

		_hr = this->_u1.load(_gDevice);
		if (_hr == W_FAILED)
		{
			this->_shader.release();
			V(W_FAILED,
				w_log_type::W_ERROR,
				"loading fragment shader uniform for model: {} . graphics device: {} . trace info: {}",
				this->_model_name, _gDevice->get_info(), _trace_info);
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
			V(W_FAILED,
				w_log_type::W_ERROR,
				"setting shader binding param for model: {} . graphics device: {} . trace info: {}",
				this->_model_name, _gDevice->get_info(), _trace_info);
		}
		_shader_params.clear();

		return W_PASSED;
	}

	W_RESULT _create_pipeline(
		_In_z_ const std::string& pPipelineCacheName,
		_In_ const w_render_pass& pRenderPass,
		_In_ const w_viewport& pViewPort,
		_In_ const w_viewport_scissor& pViewPortScissor)
	{
		return this->_pipeline.load(
			this->_gDevice,
			this->_vertex_binding_attributes,
			w_primitive_topology::TRIANGLE_LIST,
			&pRenderPass,
			&this->_shader,
			{ pViewPort },
			{ pViewPortScissor },
			pPipelineCacheName);
	}

	std::shared_ptr<w_graphics_device> 		_gDevice;
	std::string                             _name;
	std::string								_model_name;
	
	w_transform_info*						_transform;
	std::vector<w_instance_info>			_instnaces_transforms;

	w_shader								_shader;
	w_pipeline								_pipeline;

#pragma region Mesh Data	
	//vertex binding attributes
	w_vertex_binding_attributes             _vertex_binding_attributes;
	w_cpipeline_model*						_c_model;
	//mesh
	w_mesh*									_mesh;
	w_buffer								_instances_buffer;
	//global bounding box of all meshes
	w_bounding_box							_merged_bounding_box;
	//sub bounding boxes for all meshes
	std::vector<w_bounding_box>				_sub_meshes_bounding_box;
#pragma endregion

	std::vector<std::string>                _textures_paths;
	std::vector<w_texture*>					_textures;

	//uniform data
	struct basic_u0
	{
		glm::mat4							model;
		glm::mat4							view;
		glm::mat4							projection;
	};
	w_uniform<basic_u0>						_basic_u0;

	struct instance_u0
	{
		glm::mat4							view;
		glm::mat4							projection;
	};
	w_uniform<instance_u0>					_instance_u0;

	struct u1
	{
		float								cmds = 0.0f;
		float								padding_0 = 0.0f;
		float								padding_1 = 0.0f;
		float								padding_2 = 0.0f;
	};
	w_uniform<u1>							_u1;

	bool									_visible;
};

model_mesh::model_mesh(
	_In_ w_cpipeline_model* pContentPipelineModel,
	_In_ w_vertex_binding_attributes pVertexBindingAttributes) :
	_pimp(new model_mesh_pimp(pContentPipelineModel, pVertexBindingAttributes))
{
	_super::set_class_name("w_model");
}

model_mesh::~model_mesh()
{
	release();
}

W_RESULT model_mesh::load(
	_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const std::string& pPipelineCacheName,
	_In_z_ const std::wstring& pVertexShaderPath,
	_In_z_ const std::wstring& pFragmentShaderPath,
	_In_ const w_render_pass& pDrawRenderPass,
	_In_ const wolf::render::vulkan::w_viewport& pViewPort,
	_In_ const wolf::render::vulkan::w_viewport_scissor& pViewPortScissor)
{
	return !this->_pimp ? W_FAILED : this->_pimp->load(
		pGDevice, 
		pPipelineCacheName, 
		pVertexShaderPath, 
		pFragmentShaderPath,
		pDrawRenderPass,
		pViewPort,
		pViewPortScissor);
}

W_RESULT model_mesh::draw(_In_ const w_command_buffer& pCommandBuffer, _In_ const bool& pInDirectDraw)
{
	return !this->_pimp ? W_FAILED : this->_pimp->draw(pCommandBuffer, pInDirectDraw);
}

ULONG model_mesh::release()
{
	if (_super::get_is_released()) return 1;

	//release the private implementation
	SAFE_RELEASE(this->_pimp);

	return _super::release();
}

#pragma region Getters

std::string model_mesh::get_model_name() const
{
	return (!this->_pimp) ? "" : this->_pimp->get_model_name();
}

glm::vec3 model_mesh::get_position() const
{
	return (!this->_pimp) ? glm::vec3(0) : this->_pimp->get_position();
}

glm::vec3 model_mesh::get_rotation() const
{
	return (!this->_pimp) ? glm::vec3(0) : this->_pimp->get_rotation();
}

glm::vec3 model_mesh::get_scale() const
{
	return (!this->_pimp) ? glm::vec3(0) : this->_pimp->get_scale();
}

std::vector<w_instance_info> model_mesh::get_instances() const
{
	return (!this->_pimp) ? std::vector<w_instance_info>() : this->_pimp->get_instances();
}

w_bounding_box model_mesh::get_global_bounding_box() const
{
	return (!this->_pimp) ? w_bounding_box() : this->_pimp->get_global_bounding_box();
}

bool model_mesh::get_enable_instances_colors() const
{
	return (!this->_pimp) ? false : this->_pimp->get_enable_instances_colors();
}

bool model_mesh::get_visible() const
{
	return (!this->_pimp) ? false : this->_pimp->get_visible();
}

#pragma endregion

#pragma region Setters

void model_mesh::set_view_projection(_In_ const glm::mat4& pView, _In_ const glm::mat4& pProjection)
{
	if (this->_pimp)
	{
		this->_pimp->set_view_projection(pView, pProjection);
	}
}

void model_mesh::set_enable_instances_colors(_In_ const bool& pEnable)
{
	if (this->_pimp)
	{
		this->_pimp->set_enable_instances_colors(pEnable);
	}
}

void model_mesh::set_visible(_In_ const bool& pValue)
{
	if (this->_pimp)
	{
		this->_pimp->set_visible(pValue);
	}
}

#pragma endregion

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++
