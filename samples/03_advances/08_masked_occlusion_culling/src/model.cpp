#include "pch.h"
#include "model.h"

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

model::model(
	_In_ w_cpipeline_model* pContentPipelineModel,
	_In_ w_vertex_binding_attributes pVertexBindingAttributes) :
	_super(pContentPipelineModel, pVertexBindingAttributes),
	_name("model")
{
	if (this->c_model)
	{
		this->model_name = this->c_model->get_name();
		this->transform = this->c_model->get_transform();
		//get all instances
		this->c_model->get_instances(this->instances_transforms);
	}
}

model::~model()
{
	release();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++

W_RESULT model::initialize()
{
	const std::string _trace_info = this->_name + "::initialize";

	std::vector<w_cpipeline_mesh*> _meshes;

	//get all meshes
	this->c_model->get_meshes(_meshes);

	//get size of mesh
	size_t _meshes_count = _meshes.size();
	if (!_meshes_count)
	{
		V(W_FAILED, "model " + this->model_name + " does not have any mesh", _trace_info, 2);
		return W_FAILED;
	}
	//prepare vertices and indices
	uint32_t _base_vertex_offset = 0;

	//store the indices and vertices of mesh to batches
	_store_to_batch(
		_meshes,
		this->vertex_binding_attributes,
		_base_vertex_offset,
		this->tmp_batch_vertices,
		this->tmp_batch_indices,
		&this->merged_bounding_box,
		&this->sub_meshes_bounding_box,
		&this->textures_paths);

	uint32_t _lod_distance_index = 1;
	const uint32_t _lod_distance_offset = 700;
	lod_info _lod_info;

	auto _number_of_meshes = _meshes.size();
	if (_number_of_meshes)
	{
		//add first lod
		_lod_info.first_index = 0;// this->tmp_batch_indices.size();// First index for this LOD
		_lod_info.index_count = _meshes[0]->indices.size();// Index count for this LOD
		_lod_info.distance = _lod_distance_index * _lod_distance_offset;
		_lod_distance_index++;

		this->lods_info.push_back(_lod_info);
	}

	auto _number_of_ch = this->c_model->get_convex_hulls_count();
	auto _number_of_lods = this->c_model->get_lods_count();

	//use ch for masked occlusion culling if avaiable
	if (_number_of_ch)
	{
		std::vector<w_cpipeline_model*> _chs;
		this->c_model->get_convex_hulls(_chs);

		for (auto& _iter : _chs)
		{
			//generate vertices and indices of bounding box
			_add_to_mocs(_iter);
		}
	}
	//use first lod for masked occlusion culling if avaiable
	else if (_number_of_lods > 0)
	{
		std::vector<w_cpipeline_model*> _lods;
		this->c_model->get_lods(_lods);

	}
	//-ch and -lod not found, so we will use default bounding box
	else if (_number_of_meshes)
	{
		_meshes[0]->bounding_box.generate_vertices();
		_add_to_mocs(_meshes[0]->bounding_box);
	}

	if (_number_of_lods > 0)
	{
		//append all lods to _batch_vertices and _batch_indices
		std::vector<w_cpipeline_model*> _lods;
		this->c_model->get_lods(_lods);
		for (auto _lod_model : _lods)
		{
			_meshes.clear();
			_lod_model->get_meshes(_meshes);
			if (_meshes.size())
			{
				_lod_info.first_index = this->tmp_batch_indices.size();// First index for this LOD
				_lod_info.index_count = _meshes[0]->indices.size();// Index count for this LOD
				_lod_info.distance = _lod_distance_index * _lod_distance_offset;
				_lod_distance_index++;

				this->lods_info.push_back(_lod_info);

				_store_to_batch(
					_meshes,
					this->vertex_binding_attributes,
					_base_vertex_offset,
					this->tmp_batch_vertices,
					this->tmp_batch_indices);
			}
		}

		_lods.clear();
	}
	_meshes.clear();
}

void model::_add_to_mocs(_In_ const w_bounding_box& pBoundingBox)
{
	moc_data _moc_data;

	auto _bb_vertices_size = W_ARRAY_SIZE(pBoundingBox.vertices);
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

void model::_add_to_mocs(_In_  w_cpipeline_model* pConvexHull)
{
	auto _size = pConvexHull->get_meshes_count();
	if (!_size) return;

	std::vector<w_cpipeline_mesh*> _meshes;
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

W_RESULT model::pre_update(_In_ wolf::framework::w_first_person_camera& pCamera,
	_Inout_ wolf::framework::w_masked_occlusion_culling& pMaskedOcclusionCulling)
{
	const std::string _trace_info = this->_name + "::update";

	//set visibility to false
	std::fill(this->visibilities.begin(), this->visibilities.end(), false);

	W_RESULT _hr = W_FAILED;
	auto _view = pCamera.get_view();
	auto _projection = pCamera.get_projection();
	this->_view_projection = _projection * _view;
	
	set_view_projection(_view, _projection);
	
	glm::mat4 _model_to_clip_matrix;
	//draw root model to Masked Occlusion culling
	for (auto& _iter : this->_mocs)
	{
		//world view projection for bounding box of masked occlusion culling
		_model_to_clip_matrix = this->_view_projection *
			glm::translate(_iter.position) *
			glm::rotate(_iter.rotation);

		pMaskedOcclusionCulling.set_matrix((float*)(&_model_to_clip_matrix[0]));
		if (pMaskedOcclusionCulling.render_triangles_async(
			(float*)&_iter.vertices[0],
			_iter.indices.data(),
			_iter.num_of_tris_for_moc) == W_PASSED)
		{
			_hr = W_PASSED;
		}
		else
		{
			V(W_FAILED, "rendering to masked occlusion culling buffer for model: " + this->model_name, _trace_info, 3);
		}
	}

	//draw instances to masked occlusion culling
	size_t _index = 1;
	glm::vec3 _pos, _rot, _dif_pos, _dif_rot;
	for (auto& _ins : this->instances_transforms)
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
				_dif_pos = _pos - glm::vec3(this->transform.position[0], this->transform.position[1], this->transform.position[2]);
				_dif_rot = _rot - glm::vec3(this->transform.rotation[0], this->transform.rotation[1], this->transform.rotation[2]);
				_model_to_clip_matrix = _view_projection * glm::translate(_iter.position + _dif_pos) * glm::rotate(_iter.rotation + _dif_rot);
			}

			pMaskedOcclusionCulling.set_matrix((float*)(&_model_to_clip_matrix[0]));
			if (pMaskedOcclusionCulling.render_triangles_async(
				(float*)&_iter.vertices[0],
				_iter.indices.data(),
				_iter.num_of_tris_for_moc) == W_PASSED)
			{
				_hr = W_PASSED;
			}
			else
			{
				V(W_FAILED, "rendering instance to masked occlusion culling buffer for model: " + this->model_name, _trace_info, 3);
			}
		}
	}

	return _hr;
}

W_RESULT model::post_update(_In_ wolf::framework::w_masked_occlusion_culling& pMaskedOcclusionCulling)
{
	W_RESULT _add_to_render_models_queue = W_FAILED;

	glm::mat4 _model_to_clip_matrix;
	//check bounding boxes of root model from Masked Occlusion culling
	MaskedOcclusionCulling::CullingResult _culling_result;
	for (auto& _iter : this->_mocs)
	{
		_model_to_clip_matrix = this->_view_projection * glm::translate(_iter.position) * glm::rotate(_iter.rotation);
		pMaskedOcclusionCulling.set_matrix((float*)(&_model_to_clip_matrix[0]));
		_culling_result = pMaskedOcclusionCulling.test_triangles_async(
			(float*)&_iter.vertices[0],
			_iter.indices.data(),
			_iter.num_of_tris_for_moc);

		//if at least one of the bounding boxes is visible, break this loop
		if (_culling_result == MaskedOcclusionCulling::VISIBLE)
		{
			this->visibilities[0] = true;
			_add_to_render_models_queue = W_PASSED;
			//pVisibleSubModels++;
			break;
		}
	}

	//check all instnaces
	glm::vec3 _pos, _rot, _dif_pos, _dif_rot;
	for (size_t i = 0; i < this->instances_transforms.size(); ++i)
	{
		auto _ins = &this->instances_transforms[i];
		_pos = glm::vec3(
			_ins->position[0],
			_ins->position[1],
			_ins->position[2]);

		_rot = glm::vec3(
			_ins->rotation[0],
			_ins->rotation[1],
			_ins->rotation[2]);

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
				_dif_pos = _pos - glm::vec3(this->transform.position[0], this->transform.position[1], this->transform.position[2]);
				_dif_rot = _rot - glm::vec3(this->transform.rotation[0], this->transform.rotation[1], this->transform.rotation[2]);
				_model_to_clip_matrix = _view_projection * glm::translate(_iter.position + _dif_pos) * glm::rotate(_iter.rotation + _dif_rot);
			}

			pMaskedOcclusionCulling.set_matrix((float*)(&_model_to_clip_matrix[0]));
			_culling_result = pMaskedOcclusionCulling.test_triangles_async(
				(float*)&_iter.vertices[0],
				_iter.indices.data(),
				_iter.num_of_tris_for_moc);

			if (_culling_result == MaskedOcclusionCulling::VISIBLE)
			{
				this->visibilities[i + 1] = true;
				_add_to_render_models_queue = W_PASSED;
				//pVisibleSubModels++;
				break;
			}
		}
	}

	return _add_to_render_models_queue;
}

ULONG model::release()
{
	if (_super::get_is_released()) return 1;

	for (auto _moc : this->_mocs)
	{
		_moc.release();
	}
	this->_mocs.clear();
	return _super::release();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++