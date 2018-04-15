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

#ifndef __W_MODEL_MESH_H__
#define __W_MODEL_MESH_H__

#include "w_graphics_device_manager.h"
#include <w_game_time.h>
#include <w_bounding.h>
#include <w_cpipeline_model.h>
#include <w_graphics/w_mesh.h>
#include <w_graphics/w_command_buffers.h>
//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++
#include <w_graphics/w_shader.h>
#include <w_graphics/w_pipeline.h>
#include <w_graphics/w_uniform.h>
#include "compute_stage.h"
//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++

class model_mesh : public wolf::system::w_object
{
public:
	model_mesh(
		_In_ wolf::content_pipeline::w_cpipeline_model* pContentPipelineModel,
		_In_ wolf::graphics::w_vertex_binding_attributes pVertexBindingAttributes);

	virtual ~model_mesh();
	
	W_RESULT load(
		_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
		_In_z_ const std::string& pPipelineCacheName,
		_In_z_ const std::string& pComputePipelineCacheName,
		_In_z_ const std::wstring& pVertexShaderPath,
		_In_z_ const std::wstring& pFragmentShaderPath,
		_In_ const wolf::graphics::w_render_pass& pRenderPass
	);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	W_RESULT submit_compute_shader();
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	W_RESULT draw(_In_ const wolf::graphics::w_command_buffer& pCommandBuffer);

	//release all resources
	ULONG release() override;

#pragma region Getters

	std::string												get_model_name() const;
	glm::vec3												get_position() const;
	glm::vec3												get_rotation() const;
	glm::vec3												get_scale() const;
	wolf::system::w_bounding_box							get_global_bounding_box() const;
	bool													get_enable_instances_colors() const;
	std::vector<wolf::content_pipeline::w_instance_info>	get_instances() const;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	const uint32_t											get_instances_count() const;
	bool													get_global_visiblity() const;
	bool													get_visiblity(_In_ const uint32_t& pModelInstanceIndex = 0) const;
	wolf::graphics::w_semaphore*							get_compute_semaphore();
	compute_stage_output									get_result_of_compute_shader();
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma endregion

#pragma region Setters

	void set_view_projection_position(
		_In_ const glm::mat4& pView, 
		_In_ const glm::mat4& pProjection,
		_In_ const glm::vec3& pCameraPosition);
	void set_enable_instances_colors(_In_ const bool& pEnable);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	void set_global_visiblity(_In_ const bool& pValue);
	void set_visiblity(_In_ const bool& pValue, _In_ const uint32_t& pModelInstanceIndex = 0);
	void set_show_only_lods(_In_ const bool& pValue);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma endregion

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
protected:
	static void _store_to_batch(
		_In_ const std::vector<wolf::content_pipeline::w_cpipeline_mesh*>& pModelMeshes,
		_In_ const wolf::graphics::w_vertex_binding_attributes& pVertexBindingAttributes,
		_Inout_ uint32_t& pBaseVertex,
		_Inout_ std::vector<float>& pBatchVertices,
		_Inout_ std::vector<uint32_t>& pBatchIndices,
		_Inout_ wolf::system::w_bounding_box* pMergedBoundingBox = nullptr,
		_Inout_ std::vector<wolf::system::w_bounding_box>* pSubMeshBoundingBoxes = nullptr,
		_Inout_ std::vector<std::string>* pTexturePathsToBeLoad = nullptr);

	std::shared_ptr<wolf::graphics::w_graphics_device> 		gDevice;

	wolf::content_pipeline::w_cpipeline_model*				c_model;
	std::string												model_name;
	
	wolf::content_pipeline::w_transform_info				transform;
	std::vector<wolf::content_pipeline::w_instance_info>	instnaces_transforms;

	std::vector<float>										tmp_batch_vertices;
	std::vector<uint32_t>									tmp_batch_indices;

	std::vector<std::string>								textures_paths;
	//global bounding box of all meshes
	wolf::system::w_bounding_box							merged_bounding_box;
	//sub bounding boxes for all meshes
	std::vector<wolf::system::w_bounding_box>				sub_meshes_bounding_box;

	wolf::graphics::w_vertex_binding_attributes				vertex_binding_attributes;

	struct lod_info
	{
		uint32_t											first_index;
		uint32_t											index_count;
		float												distance;
		float												_padding;
	};
	std::vector<lod_info>									lods_info;

	bool													global_visiblity;
	std::vector<glm::vec4>                                  visibilities;
private:

	W_RESULT	_load_textures();
	W_RESULT	_create_buffers(); 
	W_RESULT	_create_instance_buffers();
	W_RESULT	_create_lod_levels_buffer();
	W_RESULT	_create_cs_out_buffer();
	W_RESULT	_prepare_cs_path_uniform_based_on_local_size(
		_Inout_ wolf::graphics::w_shader_binding_param& pShaderBindingParam,
		_Inout_ std::wstring& pComputeShaderPath);
	W_RESULT	_create_shader_modules(
		_In_z_ const std::wstring& pVertexShaderPath,
		_In_z_ const std::wstring& pFragmentShaderPath);
	W_RESULT	_create_pipelines(
		_In_z_ const std::string& pPipelineCacheName,
		_In_ const wolf::graphics::w_render_pass& pRenderPass);
	
	W_RESULT   _build_compute_command_buffer();
	
	typedef	 wolf::system::w_object							_super;

	std::string												_name;

	glm::vec4												_camera_position;
	
	//uniform data
	struct basic_u0
	{
		glm::mat4											model;
		glm::mat4											view;
		glm::mat4											projection;
		glm::vec4											camera_pos;//w is padding
	};
	wolf::graphics::w_uniform<basic_u0>						_basic_u0;

	struct instance_u0
	{
		glm::mat4											view;
		glm::mat4											projection;
		glm::vec4											camera_pos;//w is padding
	};
	wolf::graphics::w_uniform<instance_u0>					_instance_u0;

	struct U1
	{
		float	texture_max_mip_maps = 10.0f;
		float	bounding_sphere_radius = 0.0f;
	};
	wolf::graphics::w_uniform<U1>                           _u1;

	struct u2
	{
		float												cmds = 0;
	};
	wolf::graphics::w_uniform<u2>							_u2;
	
	wolf::graphics::w_shader*								_shader;
	wolf::graphics::w_pipeline								_pipeline;

	struct vertex_instance_data
	{
		glm::vec3   pos;
		glm::vec3   rot;
	};
	wolf::graphics::w_mesh*									_mesh;
	wolf::graphics::w_buffer								_instances_buffer;

	std::vector<wolf::graphics::w_texture*>					_textures;
	
	wolf::graphics::w_indirect_draws_command_buffer         indirect_draws;

	compute_stage											_cs;
	compute_stage_output									_cs_out_struct;
	wolf::graphics::w_buffer								_cs_out_buffer;

	bool													_show_only_lod;

	uint32_t												_selected_lod_index;
};

#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++