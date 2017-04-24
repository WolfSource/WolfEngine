#include "w_render_pch.h"
#include "w_pipeline.h"

namespace wolf
{
    namespace graphics
    {
        class w_pipeline_pimp
        {
        public:
            w_pipeline_pimp() :
            _name("w_pipeline"),
            _pipeline(nullptr),
            _pipeline_layout(nullptr)
            {
                
            }
            
            HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                         _In_ const VkRenderPass pRenderPass,
                         _In_ const std::vector<VkPipelineShaderStageCreateInfo>* pShaderStages,
                         _In_ const std::vector<w_viewport> pViewPorts,
                         _In_ const std::vector<w_viewport_scissor> pViewPortsScissors,
                         _In_ const VkPipelineLayoutCreateInfo* const pPipelineLayoutCreateInfo,
                         _In_ const VkPipelineVertexInputStateCreateInfo* const pPipelineVertexInputStateCreateInfo,
                         _In_ const VkPipelineInputAssemblyStateCreateInfo* const pPipelineInputAssemblyStateCreateInfo,
                         _In_ const VkPipelineRasterizationStateCreateInfo* const pPipelineRasterizationStateCreateInfo,
                         _In_ const VkPipelineMultisampleStateCreateInfo* const pPipelineMultisampleStateCreateInfo,
                         _In_ const VkPipelineColorBlendAttachmentState* const pPipelineColorBlendAttachmentState,
                         _In_ const VkPipelineDynamicStateCreateInfo* const pPipelineDynamicStateCreateInfo,
                         _In_ const std::array<float,4> pBlendColors)
            {
                this->_gDevice = pGDevice;
                
                if(!pShaderStages)
                {
                    logger.error("Shader could not be nullptr");
                    return S_FALSE;
                }
                if(!pRenderPass)
                {
                    logger.error("Render pass could not be nullptr");
                    return S_FALSE;
                }
                
                //create viewports and scissors
                const VkPipelineViewportStateCreateInfo _viewport_state_create_info =
                {
                    
                    VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,                  // Type
                    nullptr,                                                                // Next
                    0,                                                                      // Flags
                    static_cast<uint32_t>(pViewPorts.size()),                               // ViewportCount
                    pViewPorts.data(),                                                      // Viewports
                    static_cast<uint32_t>(pViewPortsScissors.size()),                      // ScissorCount
                    pViewPortsScissors.data()  // Scissors
                };
                
                //create pipeline
                auto _hr = vkCreatePipelineLayout(this->_gDevice->vk_device,
                                                  pPipelineLayoutCreateInfo == nullptr ? &(w_graphics_device::defaults::vk_default_pipeline_layout_create_info) : pPipelineLayoutCreateInfo,
                                                  nullptr,
                                                  &this->_pipeline_layout);
                if(_hr)
                {
                    V(S_FALSE, "creating pipeline layout for graphics device: " +
                      this->_gDevice->device_name + " ID:" + std::to_string(this->_gDevice->device_id), this->_name, 3, false, true);
                    return S_FALSE;
                }
                
                //create blend state
                const VkPipelineColorBlendStateCreateInfo _color_blend_state_create_info =
                {
                    VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,                // Type
                    nullptr,                                                                 // Next
                    0,                                                                       // Flags
                    VK_FALSE,                                                                // logicOpEnable
                    VK_LOGIC_OP_COPY,                                                        // logicOp
                    1,                                                                       // attachmentCount
                    pPipelineColorBlendAttachmentState == nullptr ?
                    &(w_graphics_device::defaults::vk_default_pipeline_color_blend_attachment_state) :
                    pPipelineColorBlendAttachmentState,                                  // pAttachments
                    { pBlendColors[0], pBlendColors[1], pBlendColors[2], pBlendColors[3] }   // blendConstants[4]
                };
                
                VkGraphicsPipelineCreateInfo _pipeline_create_info =
                {
                    VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,                        // Type
                    nullptr,                                                                // Next
                    0,                                                                      // Flags
                    static_cast<uint32_t>((*pShaderStages).size()),                         // stageCount
                    (*pShaderStages).data(),                                                // Stages
                    
                    pPipelineVertexInputStateCreateInfo == nullptr ? &(w_graphics_device::defaults::vk_default_pipeline_vertex_input_state_create_info) : pPipelineVertexInputStateCreateInfo, //vertex input state
                    
                    pPipelineInputAssemblyStateCreateInfo == nullptr ? &(w_graphics_device::defaults::vk_default_pipeline_input_assembly_state_create_info) : pPipelineInputAssemblyStateCreateInfo, //input assembly state
                    
                    nullptr,                                                                // TessellationState
                    
                    &_viewport_state_create_info,                                           // ViewportState
                    
                    pPipelineRasterizationStateCreateInfo == nullptr ? &(w_graphics_device::defaults::vk_default_pipeline_rasterization_state_create_info) : pPipelineRasterizationStateCreateInfo, //RasterizationState
                    
                    pPipelineMultisampleStateCreateInfo == nullptr ? &(w_graphics_device::defaults::vk_default_pipeline_multisample_state_create_info) : pPipelineMultisampleStateCreateInfo, //pMultisampleState
                    
                    nullptr,                                                                // DepthStencilState
                    
                    &_color_blend_state_create_info,                                        //pColorBlendState
                    
                    pPipelineDynamicStateCreateInfo == nullptr ? nullptr : pPipelineDynamicStateCreateInfo,                                    // DynamicState
                    
                    this->_pipeline_layout,                                                       // Layout
                    pRenderPass,                                                            // VkRenderPass
                    0,                                                                      // subpass
                    VK_NULL_HANDLE,                                                         // basePipelineHandle
                    -1                                                                       // basePipelineIndex
                };
                
                _hr = vkCreateGraphicsPipelines(this->_gDevice->vk_device,
                                                VK_NULL_HANDLE,
                                                1,
                                                &_pipeline_create_info,
                                                nullptr,
                                                &this->_pipeline);
                if(_hr)
                {
                    V(S_FALSE, "creating pipeline for graphics device: " +
                      this->_gDevice->device_name + " ID:" + std::to_string(this->_gDevice->device_id),
                      this->_name, 3, false, true);
                    return S_FALSE;
                }
                
                return S_OK;
            }
            
            ULONG release()
            {
                if (this->_pipeline)
                {
                    vkDestroyPipeline(this->_gDevice->vk_device,
                                      this->_pipeline,
                                      nullptr);
                    this->_pipeline = nullptr;
                }
                if (this->_pipeline_layout)
                {
                    vkDestroyPipelineLayout(this->_gDevice->vk_device,
                                            _pipeline_layout,
                                            nullptr);
                    _pipeline_layout = nullptr;
                }
             
                this->_gDevice = nullptr;
                
                return 1;
            }
            
            
#pragma region Getters
            
            const VkPipeline get_handle() const
            {
                return this->_pipeline;
            }
            
            const VkPipelineLayout get_layout_handle() const
            {
                return this->_pipeline_layout;
            }
            
#pragma endregion
       
#pragma region Setters
            
        
            
#pragma endregion
            
        private:
            std::string                                     _name;
            std::shared_ptr<w_graphics_device>              _gDevice;
            VkPipeline                                      _pipeline;
            VkPipelineLayout                                _pipeline_layout;
        };
    }
}

using namespace wolf::graphics;

w_pipeline::w_pipeline() : _pimp(new w_pipeline_pimp())
{
	_super::set_class_name("w_pipeline");
}

w_pipeline::~w_pipeline()
{
	release();
}

HRESULT w_pipeline::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                         _In_ const VkRenderPass pRenderPass,
                         _In_ const std::vector<VkPipelineShaderStageCreateInfo>* pShaderStages,
                         _In_ const std::vector<w_viewport> pViewPorts,
                         _In_ const std::vector<w_viewport_scissor> pViewPortsScissors,
                         _In_ const VkPipelineLayoutCreateInfo* const pPipelineLayoutCreateInfo,
                         _In_ const VkPipelineVertexInputStateCreateInfo* const pPipelineVertexInputStateCreateInfo,
                         _In_ const VkPipelineInputAssemblyStateCreateInfo* const pPipelineInputAssemblyStateCreateInfo,
                         _In_ const VkPipelineRasterizationStateCreateInfo* const pPipelineRasterizationStateCreateInfo,
                         _In_ const VkPipelineMultisampleStateCreateInfo* const pPipelineMultisampleStateCreateInfo,
                         _In_ const VkPipelineColorBlendAttachmentState* const pPipelineColorBlendAttachmentState,
                         _In_ const VkPipelineDynamicStateCreateInfo* const pPipelineDynamicStateCreateInfo,
                         _In_ const std::array<float,4> pBlendColors)
{
    if(!this->_pimp) return S_FALSE;
    return this->_pimp->load(pGDevice,
                             pRenderPass,
                             pShaderStages,
                             pViewPorts,
                             pViewPortsScissors,
                             pPipelineLayoutCreateInfo,
                             pPipelineVertexInputStateCreateInfo,
                             pPipelineInputAssemblyStateCreateInfo,
                             pPipelineRasterizationStateCreateInfo,
                             pPipelineMultisampleStateCreateInfo,
                             pPipelineColorBlendAttachmentState,
                             pPipelineDynamicStateCreateInfo,
                             pBlendColors);
}

ULONG w_pipeline::release()
{
	if (_super::get_is_released()) return 0;
    
    SAFE_RELEASE(this->_pimp);
    
	return _super::release();
}

#pragma region Getters

const VkPipeline w_pipeline::get_handle() const
{
    if(!this->_pimp) return 0;
    return this->_pimp->get_handle();
}

const VkPipelineLayout w_pipeline::get_layout_handle() const
{
    if(!this->_pimp) return 0;
    return this->_pimp->get_layout_handle();
}

#pragma endregion

#pragma region Setters



#pragma endregion
