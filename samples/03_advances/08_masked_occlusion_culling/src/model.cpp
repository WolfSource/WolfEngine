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
		this->c_model->get_instances(this->instnaces_transforms);
	}
}

model::~model()
{
	release();
}

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

	if (_meshes.size())
	{
		//add first lod
		_lod_info.first_index = 0;// this->tmp_batch_indices.size();// First index for this LOD
		_lod_info.index_count = _meshes[0]->indices.size();// Index count for this LOD
		_lod_info.distance = _lod_distance_index * _lod_distance_offset;
		_lod_distance_index++;

		this->lods_info.push_back(_lod_info);
	}

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
	_meshes.clear();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++

void model::add_to_mocs(_In_ const w_bounding_box& pBoundingBox)
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

W_RESULT model::pre_update(_In_ wolf::framework::w_first_person_camera& pCamera, _Inout_ wolf::framework::w_masked_occlusion_culling& pMaskedOcclusionCulling)
{
	const std::string _trace_info = this->_name + "::update";

	std::fill(this->visibilities.begin(), this->visibilities.end(), 0.0f);//reset visibilities of all models

	W_RESULT _hr = W_FAILED;
	auto _view = pCamera.get_view();
	auto _projection = pCamera.get_projection();

	set_view_projection(_view, _projection);
	auto _projection_view = _projection * _view;

	//draw bounding boxes of root model to Masked Occlusion culling
	for (auto& _iter : this->_mocs)
	{
		//world view projection for bounding box of masked occlusion culling
		auto _model_to_clip_matrix = _projection_view *
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
			V(W_FAILED, "rendering model to masked occlusion culling buffer for model: " + this->model_name, _trace_info, 3);
		}
	}

	return _hr;
}


W_RESULT model::post_update(_In_ wolf::framework::w_masked_occlusion_culling& pMaskedOcclusionCulling)
{
	return W_PASSED;
}

ULONG model::release()
{
	return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++