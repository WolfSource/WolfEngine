//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++
#include "pch.h"
#include "model.h"

using namespace wolf::system;
using namespace wolf::render::vulkan;
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

#pragma region collada from 3DMax 

		//3DMax is right handed Zup
		this->transform->rotation[0] += glm::radians(90.0f);
		std::swap(this->transform->position[1], this->transform->position[2]);
		this->transform->position[1] *= -1.0f;

		for (auto& _ins : this->instnaces_transforms)
		{
			_ins.rotation[0] += glm::radians(90.0f);
			std::swap(_ins.position[1], _ins.position[2]);
			_ins.position[1] *= -1.0f;
		}

#pragma endregion 
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
		V(W_FAILED,
			w_log_type::W_ERROR,
			"model {} does not have any mesh: graphics device: {} . trace info: {}",
			this->model_name, this->gDevice->get_info(), _trace_info);

		return W_FAILED;
	}
	//prepare vertices and indices
	uint32_t _base_vertex_offset = 0;

	//store the indices and vertices of mesh to batches
	const uint32_t _lod_distance_offset = 700;
	_store_to_batch(
		_meshes,
		this->vertex_binding_attributes,
		_lod_distance_offset,
		_base_vertex_offset,
		this->tmp_batch_vertices,
		this->tmp_batch_indices,
		this->lods_info,
		&this->merged_bounding_box,
		&this->sub_meshes_bounding_box,
		&this->textures_paths);

	_meshes.clear();

	return W_PASSED;
}

W_RESULT model::update()
{
	std::fill(this->visibilities.begin(), this->visibilities.end(), glm::vec4(1.0f));//set visible all for test
	return W_PASSED;
}

ULONG model::release()
{
	return 0;
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++