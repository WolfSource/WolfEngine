#include "w_render_pch.h"
#include "w_pipeline.h"
#include <w_cpipeline_model.h>

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
                _In_ w_mesh::w_vertex_declaration pVertexDeclaration,
                _In_ const VkPrimitiveTopology pPrimitiveTopology,
                _In_ const std::string& pPipelineCacheName,
                _In_ const VkRenderPass pRenderPass,
                _In_ const std::vector<VkPipelineShaderStageCreateInfo>* pShaderStages,
                _In_ const VkDescriptorSetLayout* const pShaderDescriptorSetLayoutBinding,
                _In_ const std::vector<w_viewport>& pViewPorts,
                _In_ const std::vector<w_viewport_scissor>& pViewPortsScissors,
                _In_ const std::vector<VkDynamicState>& pDynamicStates,
                _In_ const UINT& pTessellationPatchControlPoints,
                _In_ const VkPipelineRasterizationStateCreateInfo* const pPipelineRasterizationStateCreateInfo,
                _In_ const VkPipelineMultisampleStateCreateInfo* const pPipelineMultiSampleStateCreateInfo,
                _In_ const bool pEnableDepthStencilState,
                _In_ const VkPipelineColorBlendAttachmentState pBlendState,
                _In_ const std::array<float, 4> pBlendColors)
            {
                this->_gDevice = pGDevice;

                if (pVertexDeclaration == w_mesh::w_vertex_declaration::VERTEX_UNKNOWN)
                {
                    logger.error("Unknown vertex type");
                    return S_FALSE;
                }

                if (!pShaderStages)
                {
                    logger.error("Shader stages could not be nullptr");
                    return S_FALSE;
                }
                if (!pRenderPass)
                {
                    logger.error("Render pass could not be nullptr");
                    return S_FALSE;
                }

                VkPipelineVertexInputStateCreateInfo* _vertex_input_state_create_info = nullptr;
                VkPipelineInputAssemblyStateCreateInfo* _input_assembly_state_create_info = nullptr;
                VkPipelineDynamicStateCreateInfo* _pipeline_dynamic_state_create_info = nullptr;
                auto _pipeline_layout_create_info = _generate_pipeline_layout_create_info(
                    pVertexDeclaration,
                    pPrimitiveTopology,
                    pShaderDescriptorSetLayoutBinding,
                    pDynamicStates,
                    &_vertex_input_state_create_info,
                    &_input_assembly_state_create_info,
                    &_pipeline_dynamic_state_create_info);
                this->_pipeline_layout = w_pipeline::create_pipeline_layout(this->_gDevice, &_pipeline_layout_create_info);

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

                //create blend state
                VkPipelineColorBlendStateCreateInfo _color_blend_state_create_info = {};
                _color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                _color_blend_state_create_info.attachmentCount = 1;
                _color_blend_state_create_info.pAttachments = &pBlendState;
                _color_blend_state_create_info.blendConstants[0] = pBlendColors[0];
                _color_blend_state_create_info.blendConstants[1] = pBlendColors[1];
                _color_blend_state_create_info.blendConstants[2] = pBlendColors[2];
                _color_blend_state_create_info.blendConstants[3] = pBlendColors[3];

                VkPipelineDepthStencilStateCreateInfo _depth_stencil_state = {};
                if (pEnableDepthStencilState)
                {
                    _depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                    _depth_stencil_state.depthTestEnable = VK_TRUE;
                    _depth_stencil_state.depthWriteEnable = VK_TRUE;
                    _depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
                    _depth_stencil_state.minDepthBounds = 0.0f;
                    _depth_stencil_state.maxDepthBounds = 1.0f;
                    _depth_stencil_state.stencilTestEnable = VK_FALSE;
                    _depth_stencil_state.front = {};
                    _depth_stencil_state.back = {};
                }
                VkGraphicsPipelineCreateInfo _pipeline_create_info;
                _pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                _pipeline_create_info.pNext = nullptr;
                _pipeline_create_info.flags = 0;
                _pipeline_create_info.stageCount = static_cast<uint32_t>((*pShaderStages).size());
                _pipeline_create_info.pStages = (*pShaderStages).data();

                _pipeline_create_info.pVertexInputState =
                    _vertex_input_state_create_info == nullptr ?
                    &(w_graphics_device::defaults::vk_default_pipeline_vertex_input_state_create_info) :
                    _vertex_input_state_create_info;

                _pipeline_create_info.pInputAssemblyState =
                    _input_assembly_state_create_info == nullptr ?
                    &(w_graphics_device::defaults::vk_default_pipeline_input_assembly_state_create_info) :
                    _input_assembly_state_create_info;


                if (pTessellationPatchControlPoints == 0)
                {
                    _pipeline_create_info.pTessellationState = nullptr;
                }
                else
                {
                    VkPipelineTessellationStateCreateInfo _tessellation_state_create_info = {};
                    _tessellation_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
                    _tessellation_state_create_info.patchControlPoints = pTessellationPatchControlPoints;

                    _pipeline_create_info.pTessellationState = &_tessellation_state_create_info;
                }

                _pipeline_create_info.pRasterizationState = pPipelineRasterizationStateCreateInfo == nullptr ? 
                    &(w_graphics_device::defaults::vk_default_pipeline_rasterization_state_create_info) : pPipelineRasterizationStateCreateInfo;

                _pipeline_create_info.pMultisampleState = pPipelineMultiSampleStateCreateInfo == nullptr ? 
                    &(w_graphics_device::defaults::vk_default_pipeline_multisample_state_create_info) : pPipelineMultiSampleStateCreateInfo;

                _pipeline_create_info.pDepthStencilState = pEnableDepthStencilState ? &_depth_stencil_state : nullptr;
                _pipeline_create_info.pColorBlendState = &_color_blend_state_create_info;
                _pipeline_create_info.pDynamicState = _pipeline_dynamic_state_create_info == nullptr ? nullptr : _pipeline_dynamic_state_create_info;
                _pipeline_create_info.pViewportState = &_viewport_state_create_info;
                _pipeline_create_info.layout = this->_pipeline_layout;
                _pipeline_create_info.renderPass = pRenderPass;
                _pipeline_create_info.subpass = 0;
                _pipeline_create_info.basePipelineHandle = 0;
                _pipeline_create_info.basePipelineIndex = -1;

                auto _pipeline_cache = w_pipeline::get_pipeline_cache(pPipelineCacheName);

                auto _hr = vkCreateGraphicsPipelines(this->_gDevice->vk_device,
                    _pipeline_cache == nullptr ? VK_NULL_HANDLE : _pipeline_cache,
                    1,
                    &_pipeline_create_info,
                    nullptr,
                    &this->_pipeline);
                if (_hr)
                {
                    V(S_FALSE, "creating pipeline for graphics device: " +
                        this->_gDevice->device_name + " ID:" + std::to_string(this->_gDevice->device_id),
                        this->_name, 3, false, true);
                    return S_FALSE;
                }

                return S_OK;
            }

            void bind(_In_ const VkCommandBuffer& pCommandBuffer, _In_ VkDescriptorSet* pDescriptorSet)
            {
                if (pDescriptorSet)
                {
                    vkCmdBindDescriptorSets(pCommandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        this->_pipeline_layout,
                        0,
                        1,
                        pDescriptorSet,
                        0,
                        nullptr);
                }
                vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->_pipeline);
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

            static std::map<std::string, VkPipelineCache> pipeline_caches;

        private:

            const VkPipelineLayoutCreateInfo _generate_pipeline_layout_create_info(
                _In_ const w_mesh::w_vertex_declaration pVertexDeclaration,
                _In_ const VkPrimitiveTopology pPrimitiveTopology,
                _In_ const VkDescriptorSetLayout* const pDescriptorSetLayoutBinding,
                _In_ const std::vector<VkDynamicState>& pDynamicStates,
                _Out_ VkPipelineVertexInputStateCreateInfo** pVertexInputStateCreateInfo,
                _Out_ VkPipelineInputAssemblyStateCreateInfo** pInputAssemblyStateCreateInfo,
                _Out_ VkPipelineDynamicStateCreateInfo** pDynamicStateCreateInfo)
            {
                HRESULT _hr = S_OK;

                using namespace wolf::content_pipeline;

                auto _vertex_binding_descriptions = new std::vector<VkVertexInputBindingDescription>();
                auto _vertex_attribute_descriptions = new std::vector<VkVertexInputAttributeDescription>();

                switch (pVertexDeclaration)
                {
                default:
                case w_mesh::w_vertex_declaration::VERTEX_POSITION:
                {

                }
                break;
                case w_mesh::w_vertex_declaration::VERTEX_POSITION_UV:
                {
                    //create pipeline for basic shader
                    _vertex_binding_descriptions->push_back(
                    {
                        0,                                                                                        // Binding
                        sizeof(vertex_declaration_structs::vertex_position_uv),                                   // Stride
                        VK_VERTEX_INPUT_RATE_VERTEX                                                               // InputRate
                    });
                    _vertex_attribute_descriptions->push_back(
                    {
                        0,                                                                                        // Location
                        _vertex_binding_descriptions->at(0).binding,                                                  // Binding
                        VK_FORMAT_R32G32B32A32_SFLOAT,                                                            // Format
                        offsetof(vertex_declaration_structs::vertex_position_uv, position)                        // Offset
                    });
                    _vertex_attribute_descriptions->push_back(
                    {
                        1,                                                                                        // Location
                        _vertex_binding_descriptions->at(0).binding,                                                  // Binding
                        VK_FORMAT_R32G32_SFLOAT,                                                                  // Format
                        offsetof(vertex_declaration_structs::vertex_position_uv, uv)                              // Offset
                    });
                }
                break;
                case  w_mesh::w_vertex_declaration::VERTEX_POSITION_UV_INSTANCE_VEC7_INT:
                {
                    //create pipeline for instance shader
                    _vertex_binding_descriptions->push_back(
                    {
                        0,                                                                                        // Binding
                        sizeof(vertex_declaration_structs::vertex_position_uv),                                   // Stride
                        VK_VERTEX_INPUT_RATE_VERTEX                                                               // InputRate
                    });
                    _vertex_binding_descriptions->push_back(
                    {
                        1,                                                                                        // Binding
                        sizeof(wolf::content_pipeline::w_cpipeline_model::w_instance_info),                       // Stride
                        VK_VERTEX_INPUT_RATE_INSTANCE                                                             // InputRate
                    });

                    _vertex_attribute_descriptions->push_back(
                    {
                        0,                                                                                        // Location
                        _vertex_binding_descriptions->at(0).binding,                                                  // Binding
                        VK_FORMAT_R32G32B32_SFLOAT,                                                               // Format
                        0                                                                                         // Offset
                    });
                    _vertex_attribute_descriptions->push_back(
                    {
                        1,                                                                                        // Location
                        _vertex_binding_descriptions->at(0).binding,                                                  // Binding
                        VK_FORMAT_R32G32_SFLOAT,                                                                  // Format
                        sizeof(float) * 3                                                                         // Offset
                    });

                    /*
                         Per instance attributes:
                            vec3        i_instance_pos;
                            vec3        i_instance_rot;
                            float       i_instance_scale;
                            int         i_instance_uv_index;
                    */
                    _vertex_attribute_descriptions->push_back(
                    {
                        2,                                                                                        // Location
                        _vertex_binding_descriptions->at(1).binding,                                                  // Binding
                        VK_FORMAT_R32G32B32_SFLOAT,                                                               // Format
                        0                                                                                         // Offset
                    });
                    _vertex_attribute_descriptions->push_back(
                    {
                        3,                                                                                        // Location
                        _vertex_binding_descriptions->at(1).binding,                                                  // Binding
                        VK_FORMAT_R32G32B32_SFLOAT,                                                               // Format
                        sizeof(float) * 3                                                                         // Offset
                    });
                    _vertex_attribute_descriptions->push_back(
                    {
                        4,                                                                                        // Location
                        _vertex_binding_descriptions->at(1).binding,                                                  // Binding
                        VK_FORMAT_R32_SFLOAT,                                                                     // Format
                        sizeof(float) * 6                                                                         // Offset
                    });
                    _vertex_attribute_descriptions->push_back(
                    {
                        5,                                                                                        // Location
                        _vertex_binding_descriptions->at(1).binding,                                                  // Binding
                        VK_FORMAT_R32_SINT,                                                                       // Format
                        sizeof(float) * 7                                                                         // Offset
                    });
                }
                break;
                case w_mesh::w_vertex_declaration::VERTEX_POSITION_UV_COLOR:
                {
                    //create pipeline for basic shader
                    _vertex_binding_descriptions->push_back(
                    {
                        0,                                                                                              // Binding
                        sizeof(vertex_declaration_structs::vertex_position_uv_color),                                   // Stride
                        VK_VERTEX_INPUT_RATE_VERTEX                                                                     // InputRate
                    });
                    _vertex_attribute_descriptions->push_back(
                    {
                        0,                                                                       // Location
                        _vertex_binding_descriptions->at(0).binding,                                 // Binding
                        VK_FORMAT_R32G32B32_SFLOAT,                                              // Format
                        offsetof(vertex_declaration_structs::vertex_position_uv_color, position) // Offset
                    });
                    _vertex_attribute_descriptions->push_back(
                    {
                        1,                                                                       // Location
                        _vertex_binding_descriptions->at(0).binding,                                 // Binding
                        VK_FORMAT_R32G32_SFLOAT,                                           // Format
                        offsetof(vertex_declaration_structs::vertex_position_uv_color, uv)    // Offset
                    });
                    _vertex_attribute_descriptions->push_back(
                    {
                        2,                                                                       // Location
                        _vertex_binding_descriptions->at(0).binding,                                 // Binding
                        VK_FORMAT_R32G32B32A32_SFLOAT,                                                 // Format
                        offsetof(vertex_declaration_structs::vertex_position_uv_color, color)       // Offset
                    });
                }
                break;
                }

                auto _vis_ptr = new VkPipelineVertexInputStateCreateInfo();
                _vis_ptr->sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                _vis_ptr->pNext = nullptr;
                _vis_ptr->vertexBindingDescriptionCount = static_cast<uint32_t>(_vertex_binding_descriptions->size());
                _vis_ptr->pVertexBindingDescriptions = _vertex_binding_descriptions->data();
                _vis_ptr->vertexAttributeDescriptionCount = static_cast<uint32_t>(_vertex_attribute_descriptions->size());
                _vis_ptr->pVertexAttributeDescriptions = _vertex_attribute_descriptions->data();
                *pVertexInputStateCreateInfo = _vis_ptr;
                
                auto _vas_ptr = new VkPipelineInputAssemblyStateCreateInfo();
                _vas_ptr->sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                _vas_ptr->pNext = nullptr;
                _vas_ptr->flags = 0;
                _vas_ptr->topology = pPrimitiveTopology;
                _vas_ptr->primitiveRestartEnable = VK_FALSE;
                *pInputAssemblyStateCreateInfo = _vas_ptr;
                
                auto _dys_ptr = new VkPipelineDynamicStateCreateInfo();
                _dys_ptr->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                _dys_ptr->pNext = nullptr;
                _dys_ptr->flags = 0;
                _dys_ptr->dynamicStateCount = static_cast<uint32_t>(pDynamicStates.size());
                _dys_ptr->pDynamicStates = pDynamicStates.data();
                *pDynamicStateCreateInfo = _dys_ptr;

                VkPipelineLayoutCreateInfo _pipeline_layout_create_info =
                {
                    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,                          // Type
                    nullptr,                                                                // Next
                    0,                                                                      // Flags
                    static_cast<uint32_t>(pDescriptorSetLayoutBinding == nullptr ? 0 : 1),       // SetLayoutCount
                    pDescriptorSetLayoutBinding == nullptr ? nullptr : pDescriptorSetLayoutBinding,  // SetLayouts
                    0,                                                                      // PushConstantRangeCount
                    nullptr                                                                 // PushConstantRanges
                };

                return _pipeline_layout_create_info;
            }


            std::string                                     _name;
            std::shared_ptr<w_graphics_device>              _gDevice;
            VkPipeline                                      _pipeline;
            VkPipelineLayout                                _pipeline_layout;
        };
    }
}

using namespace wolf::graphics;

std::map<std::string, VkPipelineCache> w_pipeline_pimp::pipeline_caches;

w_pipeline::w_pipeline() : _pimp(new w_pipeline_pimp())
{
	_super::set_class_name("w_pipeline");
}

w_pipeline::~w_pipeline()
{
	release();
}

HRESULT w_pipeline::load(
    _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ w_mesh::w_vertex_declaration pVertexDeclaration,
    _In_ const VkPrimitiveTopology pPrimitiveTopology,
    _In_ const std::string& pPipelineCacheName,
    _In_ const VkRenderPass pRenderPass,
    _In_ const std::vector<VkPipelineShaderStageCreateInfo>* pShaderStages,
    _In_ const VkDescriptorSetLayout* pShaderDescriptorSetLayoutBinding,
    _In_ const std::vector<w_viewport>& pViewPorts,
    _In_ const std::vector<w_viewport_scissor>& pViewPortsScissors,
    _In_ const std::vector<VkDynamicState>& pDynamicStates,
    _In_ const UINT& pTessellationPatchControlPoints,
    _In_ const VkPipelineRasterizationStateCreateInfo* const pPipelineRasterizationStateCreateInfo,
    _In_ const VkPipelineMultisampleStateCreateInfo* const pPipelineMultiSampleStateCreateInfo,
    _In_ const bool pEnableDepthStencilState,
    _In_ const VkPipelineColorBlendAttachmentState pBlendState,
    _In_ const std::array<float, 4> pBlendColors)
{
    if (!this->_pimp) return S_FALSE;

    return this->_pimp->load(
        pGDevice,
        pVertexDeclaration,
        pPrimitiveTopology,
        pPipelineCacheName,
        pRenderPass,
        pShaderStages,
        pShaderDescriptorSetLayoutBinding,
        pViewPorts,
        pViewPortsScissors,
        pDynamicStates,
        pTessellationPatchControlPoints,
        pPipelineRasterizationStateCreateInfo,
        pPipelineMultiSampleStateCreateInfo,
        pEnableDepthStencilState,
        pBlendState,
        pBlendColors);
}

void w_pipeline::bind(_In_ const VkCommandBuffer& pCommandBuffer, _In_ VkDescriptorSet* pDescriptorSet)
{
    if (!this->_pimp) return;
    return this->_pimp->bind(pCommandBuffer, pDescriptorSet);
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

VkPipelineLayout w_pipeline::create_pipeline_layout(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const VkPipelineLayoutCreateInfo* const pPipelineLayoutCreateInfo)
{
    //create pipeline layout
    VkPipelineLayout _pipeline_layout = 0;
    auto _hr = vkCreatePipelineLayout(pGDevice->vk_device,
        pPipelineLayoutCreateInfo == nullptr ?
        &(w_graphics_device::defaults::vk_default_pipeline_layout_create_info) : pPipelineLayoutCreateInfo,
        nullptr,
        &_pipeline_layout);
    if (_hr)
    {
        V(S_FALSE, "creating pipeline layout for graphics device: " +
            pGDevice->device_name + " ID:" + std::to_string(pGDevice->device_id), "w_pipeline", 3, false, true);
        return 0;
    }
    return _pipeline_layout;
}

HRESULT w_pipeline::create_pipeline_cache(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_z_ const std::string& pPipelineCacheName)
{
    VkPipelineCache _pipeline_cache;
    VkPipelineCacheCreateInfo _pipeline_cache_create_info = {};
    _pipeline_cache_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    
    auto _hr = vkCreatePipelineCache(pGDevice->vk_device, &_pipeline_cache_create_info, nullptr, &_pipeline_cache);
    if (_hr)
    {
        V(S_FALSE, "Error on creating pipeline cache with graphics device: " +
            pGDevice->device_name + " ID:" + std::to_string(pGDevice->device_id), "w_pipeline", 3, false, true);
        return S_FALSE;
    }

    auto _old_pipline_cache = get_pipeline_cache(pPipelineCacheName);
    if (_old_pipline_cache)
    {
        logger.warning("Pipeline cache " + pPipelineCacheName + "  already exists. The new one will be replaced with old one.");
        vkDestroyPipelineCache(pGDevice->vk_device, _old_pipline_cache, nullptr);
    }

    w_pipeline_pimp::pipeline_caches[pPipelineCacheName] = _pipeline_cache;
    
    return S_OK;
}

VkPipelineCache w_pipeline::get_pipeline_cache(_In_z_ const std::string& pPipelineCacheName)
{
    auto _iter = w_pipeline_pimp::pipeline_caches.find(pPipelineCacheName);
    if (_iter != w_pipeline_pimp::pipeline_caches.end())
    {
        return _iter->second;
    }
    return 0;
}

ULONG w_pipeline::release_all_pipeline_caches(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    if (!w_pipeline_pimp::pipeline_caches.size()) return 0;
    
    for (auto& _iter : w_pipeline_pimp::pipeline_caches)
    {
        vkDestroyPipelineCache(pGDevice->vk_device, _iter.second, nullptr);
        _iter.second = 0;
    }
    w_pipeline_pimp::pipeline_caches.clear();
    
    return 1;
}
