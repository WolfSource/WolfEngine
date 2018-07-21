/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_pipeline.h
	Description		 : create pipeline
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_PIEPELINE_H__
#define __W_PIEPELINE_H__

#include "w_graphics_device_manager.h"
#include "w_shader.h"
#include "w_mesh.h"

namespace wolf
{
	namespace render
	{
		namespace vulkan
		{
			class w_pipeline_pimp;
			class w_pipeline : public system::w_object
			{
			public:
				W_EXP w_pipeline();
				W_EXP virtual ~w_pipeline();

				W_EXP W_RESULT load(
					_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const w_vertex_binding_attributes& pVertexBindingAttributes,
					_In_ const w_primitive_topology pPrimitiveTopology,
					_In_ const w_render_pass* pRenderPassBinding,
					_In_ const w_shader* pShaderBinding,
					_In_ const std::vector<w_viewport>& pViewPorts,
					_In_ const std::vector<w_viewport_scissor>& pViewPortScissors,
					_In_ const std::string& pPipelineCacheName = "pipeline_cache",
					_In_ const std::vector<w_dynamic_state>& pDynamicStates = {},
					_In_ const std::vector<w_push_constant_range>& pPushConstantRanges = {},
					_In_ const uint32_t& pTessellationPatchControlPoints = 0,
					_In_ const w_pipeline_rasterization_state_create_info& pPipelineRasterizationStateCreateInfo = w_graphics_device::defaults_states::pipelines::rasterization_create_info,
					_In_ const w_pipeline_multisample_state_create_info& pPipelineMultisampleStateCreateInfo = w_graphics_device::defaults_states::pipelines::multisample_create_info,
					_In_ const w_pipeline_color_blend_attachment_state& pBlendState = w_graphics_device::defaults_states::blend_states::premulitplied_alpha,
					_In_ const w_color& pBlendColors = w_color::TRANSPARENT_());

				//load pipeline for compute stage
				W_EXP W_RESULT load_compute(
					_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const w_shader* pShaderBinding,
					_In_ const uint32_t& pSpecializationData,
					_In_ const std::string& pPipelineCacheName = "compute_pipeline_cache",
					_In_ const std::vector<w_push_constant_range> pPushConstantRanges = {});

				//bind to pipeline
				W_EXP W_RESULT bind(_In_ const w_command_buffer& pCommandBuffer,
					_In_ const w_pipeline_bind_point& pPipelineBindPoint);

				//release all resources
				W_EXP virtual ULONG release() override;


#pragma region Getters

				W_EXP const VkPipeline get_handle() const;
				W_EXP const VkPipelineLayout get_layout_handle() const;

#pragma endregion

#pragma region Setters

				W_EXP void set_push_constant_buffer(
					_In_ const w_command_buffer&		 pCommandBuffer,
					_In_ const w_shader_stage_flag_bits& pStageFlags,
					_In_ const uint32_t&				 pOffset,
					_In_ const uint32_t&				 pSize,
					_In_ const void*					 pValues);

#pragma endregion

				W_EXP static VkPipelineLayout create_pipeline_layout(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const VkPipelineLayoutCreateInfo* const pPipelineLayoutCreateInfo);
				//create pipeline cache
				W_EXP static W_RESULT create_pipeline_cache(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_z_ const std::string& pPipelineCacheName);
				W_EXP static VkPipelineCache get_pipeline_cache(_In_z_ const std::string& pPipelineCacheName);
				//release all pipeline caches
				W_EXP static ULONG release_all_pipeline_caches(_In_ const std::shared_ptr<w_graphics_device>& pGDevice);

#ifdef __PYTHON__

				W_RESULT py_load(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const w_vertex_binding_attributes& pVertexBindingAttributes,
					_In_ const w_primitive_topology& pPrimitiveTopology,
					_In_ const w_render_pass& pRenderPassBinding,
					_In_ const w_shader& pShaderBinding,
					_In_ const boost::python::list pViewPorts,
					_In_ const boost::python::list pViewPortScissors,
					_In_ const std::string& pPipelineCacheName,
					_In_ const boost::python::list pDynamicStates,
					_In_ const boost::python::list pPushConstantRanges,
					_In_ const uint32_t& pTessellationPatchControlPoints,
					_In_ const w_pipeline_rasterization_state_create_info& pPipelineRasterizationStateCreateInfo,
					_In_ const w_pipeline_multisample_state_create_info& pPipelineMultiSampleStateCreateInfo,
					_In_ const w_pipeline_color_blend_attachment_state& pBlendState,
					_In_ const w_color& pBlendColors)
				{
					if (!pGDevice.get()) return W_FAILED;
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

					//get viewports
					std::vector<w_viewport> _viewports;
					for (size_t i = 0; i < len(pViewPorts); ++i)
					{
						boost::python::extract<w_viewport> _vp(pViewPorts[i]);
						if (_vp.check())
						{
							_viewports.push_back(_vp());
						}
					}

					//get viewport scissor
					std::vector<w_viewport_scissor> _viewport_scissor;
					for (size_t i = 0; i < len(pViewPortScissors); ++i)
					{
						boost::python::extract<w_viewport_scissor> _vp(pViewPortScissors[i]);
						if (_vp.check())
						{
							_viewport_scissor.push_back(_vp());
						}
					}

					//get dynamic states
					std::vector<w_dynamic_state> _dynamic_states;
					for (size_t i = 0; i < len(pDynamicStates); ++i)
					{
						boost::python::extract<w_dynamic_state> _d(pDynamicStates[i]);
						if (_d.check())
						{
							_dynamic_states.push_back(_d());
						}
					}

					//get push constant range
					std::vector<w_push_constant_range> _push_consts;
					for (size_t i = 0; i < len(pPushConstantRanges); ++i)
					{
						boost::python::extract<w_push_constant_range> _p(pPushConstantRanges[i]);
						if (_p.check())
						{
							_push_consts.push_back(_p());
						}
					}

					auto _hr = load(_gDevice,
						pVertexBindingAttributes,
						pPrimitiveTopology,
						&pRenderPassBinding,
						&pShaderBinding,
						_viewports,
						_viewport_scissor,
						pPipelineCacheName,
						_dynamic_states,
						_push_consts,
						pTessellationPatchControlPoints,
						pPipelineRasterizationStateCreateInfo,
						pPipelineMultiSampleStateCreateInfo,
						pBlendState,
						pBlendColors);

					_viewports.clear();
					_viewport_scissor.clear();
					_dynamic_states.clear();
					_push_consts.clear();

					_gDevice.reset();
					return _hr;
				}

				W_RESULT py_load_min_args_7(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const w_vertex_binding_attributes& pVertexBindingAttributes,
					_In_ const w_primitive_topology& pPrimitiveTopology,
					_In_ const w_render_pass& pRenderPassBinding,
					_In_ const w_shader& pShaderBinding,
					_In_ const boost::python::list pViewPorts,
					_In_ const boost::python::list pViewPortScissors)
				{
					return py_load(
						pGDevice,
						pVertexBindingAttributes,
						pPrimitiveTopology,
						pRenderPassBinding,
						pShaderBinding,
						pViewPorts,
						pViewPortScissors,
						"pipeline_cache",
						{},
						{},
						0,
						w_graphics_device::defaults_states::pipelines::rasterization_create_info,
						w_graphics_device::defaults_states::pipelines::multisample_create_info,
						w_graphics_device::defaults_states::blend_states::premulitplied_alpha,
						w_color::TRANSPARENT_());
				}

				W_RESULT py_load_min_args_8(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const w_vertex_binding_attributes& pVertexBindingAttributes,
					_In_ const w_primitive_topology& pPrimitiveTopology,
					_In_ const w_render_pass& pRenderPassBinding,
					_In_ const w_shader& pShaderBinding,
					_In_ const boost::python::list pViewPorts,
					_In_ const boost::python::list pViewPortScissors,
					_In_ const std::string& pPipelineCacheName)
				{
					return py_load(
						pGDevice,
						pVertexBindingAttributes,
						pPrimitiveTopology,
						pRenderPassBinding,
						pShaderBinding,
						pViewPorts,
						pViewPortScissors,
						pPipelineCacheName,
						{},
						{},
						0,
						w_graphics_device::defaults_states::pipelines::rasterization_create_info,
						w_graphics_device::defaults_states::pipelines::multisample_create_info,
						w_graphics_device::defaults_states::blend_states::premulitplied_alpha,
						w_color::TRANSPARENT_());
				}

				W_RESULT py_load_compute(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const w_shader& pShaderBinding,
					_In_ const uint32_t& pSpecializationData,
					_In_ const std::string& pPipelineCacheName,
					_In_ const boost::python::list pPushConstantRanges)
				{
					if (!pGDevice.get()) return W_FAILED;
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);

					//get push constant range
					std::vector<w_push_constant_range> _push_consts;
					for (size_t i = 0; i < len(pPushConstantRanges); ++i)
					{
						boost::python::extract<w_push_constant_range> _p(pPushConstantRanges[i]);
						if (_p.check())
						{
							_push_consts.push_back(_p());
						}
					}

					auto _hr = load_compute(_gDevice,
						&pShaderBinding,
						pSpecializationData,
						pPipelineCacheName,
						_push_consts);

					_push_consts.clear();

					_gDevice.reset();
					return _hr;
				}

				W_RESULT py_bind(_In_ const w_command_buffer& pCommandBuffer)
				{
					return bind(pCommandBuffer);
				}

				void py_set_push_constant_buffer(
					_In_ const w_command_buffer&	pCommandBuffer,
					_In_ w_shader_stage_flag_bits   pStageFlags,
					_In_ const uint32_t&			pOffset,
					_In_ boost::python::list		pValues)
				{
					//get push constant range
					std::vector<float> _push_consts_values;
					for (size_t i = 0; i < len(pValues); ++i)
					{
						boost::python::extract<float> _p(pValues[i]);
						if (_p.check())
						{
							_push_consts_values.push_back(_p());
						}
					}
					set_push_constant_buffer(pCommandBuffer, pStageFlags, pOffset, _push_consts_values.size() * sizeof(float), _push_consts_values.data());
				}

				static W_RESULT py_create_pipeline_cache(
					_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
					_In_z_ const std::string& pPipelineCacheName)
				{
					if (!pGDevice.get()) return W_FAILED;
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);
					auto _hr = create_pipeline_cache(_gDevice, pPipelineCacheName);
					_gDevice.reset();
					return _hr;
				}

				static ULONG py_release_all_pipeline_caches(_In_ boost::shared_ptr<w_graphics_device>& pGDevice)
				{
					if (!pGDevice.get()) return W_FAILED;
					auto _gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);
					auto _hr = release_all_pipeline_caches(_gDevice);
					_gDevice.reset();
					return _hr;
				}
#endif

			private:
				typedef system::w_object						_super;
				w_pipeline_pimp*                                _pimp;
			};
		}
	}
}

#include "python_exporter/py_pipeline.h"

#endif
