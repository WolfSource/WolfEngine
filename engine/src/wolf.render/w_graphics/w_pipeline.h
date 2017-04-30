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

            W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                               _In_ const VkRenderPass pRenderPass,
                               _In_ const std::vector<VkPipelineShaderStageCreateInfo>* pShaderStages,
                               _In_ const std::vector<w_viewport> pViewPorts,
                               _In_ const std::vector<w_viewport_scissor> pViewPortsScissors,
                               _In_ const VkPipelineLayoutCreateInfo* const pPipelineLayoutCreateInfo = nullptr,
                               _In_ const VkPipelineVertexInputStateCreateInfo* const pPipelineVertexInputStateCreateInfo = nullptr,
                               _In_ const VkPipelineInputAssemblyStateCreateInfo* const pPipelineInputAssemblyStateCreateInfo = nullptr,
                               _In_ const VkPipelineRasterizationStateCreateInfo* const pPipelineRasterizationStateCreateInfo = nullptr,
                               _In_ const VkPipelineMultisampleStateCreateInfo* const pPipelineMultisampleStateCreateInfo = nullptr,
                               _In_ const VkPipelineColorBlendAttachmentState* const pPipelineColorBlendAttachmentState = nullptr,
                               _In_ const VkPipelineDynamicStateCreateInfo* const pPipelineDynamicStateCreateInfo = nullptr,
                               _In_ const std::array<float,4> pBlendColors = { 0.0f, 0.0f, 0.0f, 0.0f });
            
            //release all resources
            W_EXP virtual ULONG release() override;
            
            
#pragma region Getters

			W_EXP const VkPipeline get_handle() const;
			W_EXP const VkPipelineLayout get_layout_handle() const;
            
#pragma endregion

#pragma region Setters

 

#pragma endregion

		private:
			typedef system::w_object						_super;
            w_pipeline_pimp*                                _pimp;
        };
    }
}

#endif
