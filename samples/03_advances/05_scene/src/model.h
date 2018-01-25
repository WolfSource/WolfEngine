/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : model.h
	Description		 : The main class for drawing wolf::content_pipeline::w_cpipeline_model
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_MODEL_H__
#define __W_MODEL_H__

#include "w_graphics_device_manager.h"
#include <w_game_time.h>
#include <w_cpipeline_model.h>
#include <w_graphics/w_mesh.h>
#include <glm/vec3.hpp>

class model_pimp;
//Provides basic functions for rendering wolf::content_pipeline::w_cpipeline_model 
class model : public wolf::system::w_object
{
public:
	W_EXP model(
		_In_ wolf::content_pipeline::w_cpipeline_model* pContentPipelineModel,
		_In_ wolf::graphics::w_vertex_binding_attributes pVertexBindingAttributes);
	W_EXP virtual ~model();

	//first part of loading process which can be call from any thread
	HRESULT pre_load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice);
	//final part of loading process which must be call from main thread
	HRESULT post_load();

	//direct draw 
	void draw(_In_ const VkCommandBuffer& pCommandBuffer);
	/*
		Unlike direct drawing function, indirect drawing functions take their draw commands from a
		buffer object containing information like index count, index offset and number of instances to draw.
	*/
	void indirect_draw(_In_ const VkCommandBuffer& pCommandBuffer);

	//release all resources
	W_EXP ULONG release() override;

#pragma region Setters


#pragma endregion

#pragma region Getters

	glm::vec3 get_position() const;
	glm::vec3 get_rotation() const;
	glm::vec3 get_scale() const;

#pragma endregion

private:
	typedef	 wolf::system::w_object						  _super;
	model_pimp*                                           _pimp;
};


#endif
