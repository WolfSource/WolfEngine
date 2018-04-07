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

class model : public model_mesh
{
public:
	model(
		_In_ wolf::content_pipeline::w_cpipeline_model* pContentPipelineModel,
		_In_ wolf::graphics::w_vertex_binding_attributes pVertexBindingAttributes);

	virtual ~model();

	W_RESULT initialize();

	//release all resources
	ULONG release();

private:
	typedef	 model_mesh										_super;
	std::string												_name;
};

#endif
