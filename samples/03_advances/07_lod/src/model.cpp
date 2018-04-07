//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "pch.h"
#include "model.h"

using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

model::model(
	_In_ w_cpipeline_model* pContentPipelineModel,
	_In_ w_vertex_binding_attributes pVertexBindingAttributes) :
	_super(pContentPipelineModel, pVertexBindingAttributes)
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
		_lod_info.first_index = this->tmp_batch_indices.size();// First index for this LOD
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

ULONG model::release()
{
	return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++