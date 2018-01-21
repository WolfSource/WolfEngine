#include "w_render_pch.h"
#include "w_model.h"
#include <w_bounding.h>
#include <tbb/atomic.h>

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

namespace wolf
{
	namespace framework
	{
		class w_model_pimp
		{
		public:
			w_model_pimp(
				_In_ w_cpipeline_model* pContentPipelineModel,
				_In_ w_vertex_binding_attributes pVertexBindingAttributes) :
				_vertex_binding_attributes(pVertexBindingAttributes),
				_name("w_model")
			{
				this->_model_name = pContentPipelineModel->get_name();
				this->_transform = pContentPipelineModel->get_transform();
				pContentPipelineModel->get_instances(this->_instnaces_transforms);
				pContentPipelineModel->get_meshes(this->_model_meshes);
			}

			HRESULT pre_load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
			{
				this->_loaded = false;

				if (!pGDevice) return S_FALSE;
				const std::string _trace_info = this->_name + "::pre_load";
				
				this->_gDevice = pGDevice;

				if (_vertex_binding_attributes.declaration == w_vertex_declaration::NOT_DEFINED)
				{
					V(S_FALSE, "w_vertex_declaration type is needed for model \'" + this->_model_name + "\'", _trace_info, 3);
					return S_FALSE;
				}
				else if (_vertex_binding_attributes.declaration == w_vertex_declaration::USER_DEFINED &&
					_batch_vertices.size() == 0)
				{
					V(S_FALSE, "do not use this this class for \'w_vertex_declaration::USER_DEFINED\' type, instead you can use low level classes such as \'w_mesh\' and \'w_buffer\'. See \'03_advances::03_instancing\' sample. Model \'" + this->_model_name + "\'", _trace_info, 3);
					return S_FALSE;
				}

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

				if (_meshes_count)
				{
					_store_to_batch(
						_model_meshes,
						_vertex_binding_attributes,
						_base_vertex_index,
						this->_batch_vertices,
						this->_batch_indices);
				}

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

				if (!this->_batch_vertices.size())
				{
					V(S_FALSE, "model \'" + this->_model_name + "\' does not have vertices", _trace_info, 3);
					return S_FALSE;
				}

				return S_OK;
			}

			void release()
			{

			}

#pragma region Setters

#pragma endregion
			
		private:
			
			static void _store_to_batch(
				_In_ const std::vector<wolf::content_pipeline::w_cpipeline_model::w_mesh*>& pModelMeshes,
				_In_ const w_vertex_binding_attributes& pVertexBindingAttributes,
				_Inout_ uint32_t& pBaseVertex,
				_Inout_ std::vector<float>& pBatchVertices,
				_Inout_ std::vector<uint32_t>& pBatchIndices)
			{
				for (auto& _mesh_data : pModelMeshes)
				{
					for (size_t i = 0; i < _mesh_data->indices.size(); ++i)
					{
						pBatchIndices.push_back(pBaseVertex + _mesh_data->indices[i]);
					}

					switch (pVertexBindingAttributes.declaration)
					{
					default:
						break;
					case w_vertex_declaration::VERTEX_POSITION:
					{
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
					}
					case w_vertex_declaration::VERTEX_POSITION_COLOR:
					{
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
					}
					case w_vertex_declaration::VERTEX_POSITION_UV:
					{
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
					}
					case w_vertex_declaration::VERTEX_POSITION_UV_COLOR:
					{
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
					}
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
					};
				}
			}

			std::shared_ptr<w_graphics_device> 		_gDevice;
			tbb::atomic<bool>                       _loaded;
			std::string                             _name;
			w_vertex_binding_attributes             _vertex_binding_attributes;

			std::string								_model_name;
			w_transform_info						_transform;
			std::vector<w_instance_info>			_instnaces_transforms;

			//one big vertex buffer for root model and LODs vertices
			std::vector<float>                      _batch_vertices;
			//one big index buffer for root model and LODs indices
			std::vector<uint32_t>                   _batch_indices;
			//model meshes
			std::vector<w_cpipeline_model::w_mesh*> _model_meshes;

			//global bounding box of all meshes
			w_bounding_box							_merged_bounding_box;
			//sub bounding boxes for all meshes
			std::vector<w_bounding_box>				_sub_meshes_bounding_box;
		};
	}
}

using namespace wolf::framework;

w_model::w_model(
	_In_ w_cpipeline_model* pContentPipelineModel,
	_In_ w_vertex_binding_attributes pVertexBindingAttributes) :
	_pimp(new w_model_pimp(pContentPipelineModel, pVertexBindingAttributes))
{

}

w_model::~w_model()
{
	release();
}

HRESULT w_model::pre_load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
	return (!this->_pimp) ? S_FALSE : this->_pimp->pre_load(pGDevice);
}

HRESULT w_model::post_load()
{
	return S_OK;
}

void w_model::draw(_In_ const VkCommandBuffer& pCommandBuffer)
{

}

void w_model::indirect_draw(_In_ const VkCommandBuffer& pCommandBuffer)
{

}

ULONG w_model::release()
{
	if (_super::get_is_released()) return 1;

	//release the private implementation
	SAFE_RELEASE(this->_pimp);

	return _super::release();
}