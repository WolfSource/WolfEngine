/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : model.h
	Description		 : The main class for computing and managing lods of model
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_MODEL_H__
#define __W_MODEL_H__

#include "model_mesh.h"
#include <w_framework/w_masked_occlusion_culling.h>

class model : public model_mesh
{
public:
	model(
		_In_ wolf::content_pipeline::w_cpipeline_model* pContentPipelineModel,
		_In_ wolf::render::vulkan::w_vertex_binding_attributes pVertexBindingAttributes);

	virtual ~model();

	W_RESULT initialize();

	bool	 check_is_in_sight(_In_ wolf::framework::w_first_person_camera* pCamera);
	W_RESULT pre_update(
		_In_ const glm::mat4& pProjectionView, 
		_Inout_ wolf::framework::w_masked_occlusion_culling& pMaskedOcclusionCulling);
	W_RESULT post_update(_In_ wolf::framework::w_masked_occlusion_culling& pMaskedOcclusionCulling, _In_ long& pVisibleMeshes);

	//release all resources
	ULONG release();

private:
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	void _add_to_mocs(_In_ const wolf::system::w_bounding_box& pBoundingBox);
	void _add_to_mocs(
		_In_ const std::vector<wolf::content_pipeline::w_vertex_struct>& pVertices, 
		_In_ const w_vector_uint32_t& pIndices);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	typedef	 model_mesh											_super;
	std::string													_name;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	//masked occlusion culling
	struct clipspace_vertex { float x, y, z, w; };
	struct moc_data
	{
		//vertex for masked occlusion culling
		std::vector<clipspace_vertex>                           vertices;
		std::vector<uint32_t>                                   indices;
		int                                                     num_of_tris_for_moc;
		glm::vec3                                               position;
		glm::vec3                                               rotation;

		void release()
		{
			this->vertices.clear();
			this->indices.clear();
		}
	};
	std::vector<moc_data>                                       _mocs;
	glm::mat4													_projection_view;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
};

#endif