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
                _In_ const std::string& pPipelineCacheName,
                _In_ const VkRenderPass pRenderPass,
                _In_ const std::vector<VkPipelineShaderStageCreateInfo>* pShaderStages,
                _In_ const std::vector<w_viewport> pViewPorts,
                _In_ const std::vector<w_viewport_scissor> pViewPortsScissors,
                _In_ const VkPipelineLayoutCreateInfo* const pPipelineLayoutCreateInfo,
                _In_ const VkPipelineVertexInputStateCreateInfo* const pPipelineVertexInputStateCreateInfo,
                _In_ const VkPipelineInputAssemblyStateCreateInfo* const pPipelineInputAssemblyStateCreateInfo,
                _In_ const VkPipelineRasterizationStateCreateInfo* const pPipelineRasterizationStateCreateInfo,
                _In_ const VkPipelineMultisampleStateCreateInfo* const pPipelineMultisampleStateCreateInfo,
                _In_ const VkPipelineDynamicStateCreateInfo* const pPipelineDynamicStateCreateInfo,
                _In_ const bool pEnableDepthStencilState,
                _In_ const VkPipelineColorBlendAttachmentState pBlendState,
                _In_ const std::array<float, 4> pBlendColors)
            {
                this->_gDevice = pGDevice;

                if (!pShaderStages)
                {
                    logger.error("Shader could not be nullptr");
                    return S_FALSE;
                }
                if (!pRenderPass)
                {
                    logger.error("Render pass could not be nullptr");
                    return S_FALSE;
                }
                
                this->_pipeline_layout = w_pipeline::create_pipeline_layout(this->_gDevice, pPipelineLayoutCreateInfo);

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
                {   _depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                    _depth_stencil_state.depthTestEnable = VK_TRUE;
                    _depth_stencil_state.depthWriteEnable = VK_TRUE;
                    _depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
                    _depth_stencil_state.front = _depth_stencil_state.back;
                    _depth_stencil_state.back.compareOp = VK_COMPARE_OP_ALWAYS;
                }
                VkGraphicsPipelineCreateInfo _pipeline_create_info;
                _pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                _pipeline_create_info.pNext = nullptr;
                _pipeline_create_info.flags = 0;
                _pipeline_create_info.stageCount = static_cast<uint32_t>((*pShaderStages).size());
                _pipeline_create_info.pStages = (*pShaderStages).data();
                _pipeline_create_info.pVertexInputState = pPipelineVertexInputStateCreateInfo == nullptr ? &(w_graphics_device::defaults::vk_default_pipeline_vertex_input_state_create_info) : pPipelineVertexInputStateCreateInfo;
                _pipeline_create_info.pInputAssemblyState = pPipelineInputAssemblyStateCreateInfo == nullptr ? &(w_graphics_device::defaults::vk_default_pipeline_input_assembly_state_create_info) : pPipelineInputAssemblyStateCreateInfo;
                _pipeline_create_info.pTessellationState = nullptr;
                _pipeline_create_info.pRasterizationState = pPipelineRasterizationStateCreateInfo == nullptr ? &(w_graphics_device::defaults::vk_default_pipeline_rasterization_state_create_info) : pPipelineRasterizationStateCreateInfo;
                _pipeline_create_info.pMultisampleState = pPipelineMultisampleStateCreateInfo == nullptr ? &(w_graphics_device::defaults::vk_default_pipeline_multisample_state_create_info) : pPipelineMultisampleStateCreateInfo;
                _pipeline_create_info.pDepthStencilState = pEnableDepthStencilState ? &_depth_stencil_state : nullptr;
                _pipeline_create_info.pColorBlendState = &_color_blend_state_create_info;
                _pipeline_create_info.pDynamicState = pPipelineDynamicStateCreateInfo == nullptr ? nullptr : pPipelineDynamicStateCreateInfo;
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

HRESULT w_pipeline::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const std::string& pPipelineCacheName,
    _In_ const VkRenderPass pRenderPass,
    _In_ const std::vector<VkPipelineShaderStageCreateInfo>* pShaderStages,
    _In_ const std::vector<w_viewport> pViewPorts,
    _In_ const std::vector<w_viewport_scissor> pViewPortsScissors,
    _In_ const VkPipelineLayoutCreateInfo* const pPipelineLayoutCreateInfo,
    _In_ const VkPipelineVertexInputStateCreateInfo* const pPipelineVertexInputStateCreateInfo,
    _In_ const VkPipelineInputAssemblyStateCreateInfo* const pPipelineInputAssemblyStateCreateInfo,
    _In_ const VkPipelineRasterizationStateCreateInfo* const pPipelineRasterizationStateCreateInfo,
    _In_ const VkPipelineMultisampleStateCreateInfo* const pPipelineMultisampleStateCreateInfo,
    _In_ const VkPipelineDynamicStateCreateInfo* const pPipelineDynamicStateCreateInfo,
    _In_ const bool pEnableDepthStencilState,
    _In_ const VkPipelineColorBlendAttachmentState pBlendState,
    _In_ const std::array<float, 4> pBlendColors)
{
    if (!this->_pimp) return S_FALSE;

    return this->_pimp->load(
        pGDevice,
        pPipelineCacheName,
        pRenderPass,
        pShaderStages,
        pViewPorts,
        pViewPortsScissors,
        pPipelineLayoutCreateInfo,
        pPipelineVertexInputStateCreateInfo,
        pPipelineInputAssemblyStateCreateInfo,
        pPipelineRasterizationStateCreateInfo,
        pPipelineMultisampleStateCreateInfo,
        pPipelineDynamicStateCreateInfo,
        pEnableDepthStencilState,
        pBlendState,
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
