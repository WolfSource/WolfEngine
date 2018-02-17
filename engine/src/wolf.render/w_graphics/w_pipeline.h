/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_pipeline.h
	Description		 : create pipeline
	Comment          :
*/

#ifndef __W_PIEPELINE_H__
#define __W_PIEPELINE_H__

#include "w_graphics_device_manager.h"
#include "w_shader.h"
#include "w_mesh.h"

namespace wolf
{
	namespace graphics
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
				_In_ const w_pipeline_rasterization_state_create_info* const pPipelineRasterizationStateCreateInfo = nullptr,
				_In_ const w_pipeline_multisample_state_create_info* const pPipelineMultisampleStateCreateInfo = nullptr,
				_In_ const w_pipeline_color_blend_attachment_state pBlendState = w_graphics_device::defaults_states::blend_states::premulitplied_alpha,
				_In_ const w_color& pBlendColors = w_color::TRANSPARENT_());

            //load pipeline for compute stage
            W_EXP W_RESULT load_compute(
                _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const w_shader* pShaderBinding,
                _In_ const uint32_t& pSpecializationData,
                _In_ const std::string& pPipelineCacheName = "compute_pipeline_cache",
                _In_ const std::vector<w_push_constant_range> pPushConstantRanges = {});
			
			//bind to pipeline
            W_EXP W_RESULT bind(_In_ const w_command_buffer* pCommandBuffer);

            //release all resources
            W_EXP virtual ULONG release() override;
            
            
#pragma region Getters

			W_EXP const VkPipeline get_handle() const;
			W_EXP const VkPipelineLayout get_layout_handle() const;
            
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

			bool w_pipeline::py_load(
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
					&pPipelineRasterizationStateCreateInfo,
					&pPipelineMultiSampleStateCreateInfo,
					pBlendState,
					pBlendColors);

				_viewports.clear();
				_viewport_scissor.clear();
				_dynamic_states.clear();
				_push_consts.clear();

				_gDevice.reset();
				return _hr == W_PASSED;
			}

			bool w_pipeline::py_boost_load(
				_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const w_vertex_binding_attributes& pVertexBindingAttributes,
				_In_ const w_primitive_topology& pPrimitiveTopology,
				_In_ const w_render_pass& pRenderPassBinding,
				_In_ const w_shader& pShaderBinding,
				_In_ const boost::python::list pViewPorts,
				_In_ const boost::python::list pViewPortScissors,
				_In_ const std::string& pPipelineCacheName)
			{
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

				auto _hr = load(_gDevice,
					pVertexBindingAttributes,
					pPrimitiveTopology,
					&pRenderPassBinding,
					&pShaderBinding,
					_viewports,
					_viewport_scissor,
					pPipelineCacheName);

				_viewports.clear();
				_viewport_scissor.clear();

				_gDevice.reset();
				return _hr == W_PASSED;
			}


			W_EXP bool py_load_compute(
				_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const w_shader& pShaderBinding,
				_In_ const uint32_t& pSpecializationData,
				_In_ const std::string& pPipelineCacheName,
				_In_ const boost::python::list pPushConstantRanges);

			W_EXP bool py_bind(_In_ const w_command_buffer& pCommandBuffer);

			W_EXP static bool py_create_pipeline_cache(
				_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
				_In_z_ const std::string& pPipelineCacheName);
			
			W_EXP static ULONG py_release_all_pipeline_caches(_In_ boost::shared_ptr<w_graphics_device>& pGDevice);
#endif

        private:
			typedef system::w_object						_super;
            w_pipeline_pimp*                                _pimp;
        };
    }
}

#include "python_exporter/py_pipeline.h"

#endif
