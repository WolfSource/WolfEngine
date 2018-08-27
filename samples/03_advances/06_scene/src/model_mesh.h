/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : model_mesh.h
	Description		 : The main class for drawing wolf::content_pipeline::w_cpipeline_model
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_MODEL_MESH_H__
#define __W_MODEL_MESH_H__

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++

#include "w_graphics_device_manager.h"
#include <w_game_time.h>
#include <w_bounding.h>
#include <w_cpipeline_model.h>
#include <vulkan/w_mesh.h>
#include <vulkan/w_command_buffers.h>
#include <vulkan/w_viewport.h>

class model_mesh_pimp;
//Provides basic functions for rendering wolf::content_pipeline::w_cpipeline_model 
class model_mesh : public wolf::system::w_object
{
public:
	model_mesh(
		_In_ wolf::content_pipeline::w_cpipeline_model* pContentPipelineModel,
		_In_ wolf::render::vulkan::w_vertex_binding_attributes pVertexBindingAttributes);
	virtual ~model_mesh();

	//load model resources
	W_RESULT load(
		_In_ const std::shared_ptr<wolf::render::vulkan::w_graphics_device>& pGDevice,
		_In_ const std::string& pPipelineCacheName,
		_In_z_ const std::wstring& pVertexShaderPath,
		_In_z_ const std::wstring& pFragmentShaderPath,
		_In_ const wolf::render::vulkan::w_render_pass& pDrawRenderPass,
		_In_ const wolf::render::vulkan::w_viewport& pViewPort,
		_In_ const wolf::render::vulkan::w_viewport_scissor& pViewPortScissor);
	
	/*
		direct draw to graphics device or indirect draw.
		Unlike direct drawing function, indirect drawing functions take their draw commands from a
		buffer object containing information like index count, index offset and number of instances to draw.
	*/
	W_RESULT draw(_In_ const wolf::render::vulkan::w_command_buffer& pCommandBuffer, _In_ const bool& pInDirectDraw = false);

	//release all resources
	ULONG release() override;

#pragma region Getters

	std::string												get_model_name() const;
	glm::vec3												get_position() const;
	glm::vec3												get_rotation() const;
	glm::vec3												get_scale() const;
	std::vector<wolf::content_pipeline::w_instance_info>	get_instances() const;
	wolf::system::w_bounding_box							get_global_bounding_box() const;
	bool													get_enable_instances_colors() const;
	bool													get_visible() const;

#pragma endregion

#pragma region Setters

	void													set_view_projection(_In_ const glm::mat4& pView, _In_ const glm::mat4& pProjection);
	void													set_enable_instances_colors(_In_ const bool& pEnable);
	void													set_visible(_In_ const bool& pValue);

#pragma endregion

private:
	typedef	 wolf::system::w_object							_super;
	model_mesh_pimp*										_pimp;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif
