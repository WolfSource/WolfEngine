#include "w_render_pch.h"
#include "imgui_impl.h"
#include "imgui.h"
#include <w_graphics/w_buffer.h>
#include <w_graphics/w_texture.h>
#include <w_graphics/w_shader.h>
#include <w_graphics/w_render_pass.h>
#include <w_graphics/w_pipeline.h>

class imgui_pimp
{
public:
    imgui_pimp() :
        _gDevice(nullptr),
        _vertex_buffer(nullptr),
        _index_buffer(nullptr)
    {
    }

    HRESULT load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
        _In_ const float& pWidth,
        _In_ const float& pHeight,
        _In_ VkRenderPass& pRenderPass)
    {
        this->_gDevice = pGDevice;

#pragma region Set Style
        //Color scheme
        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
        style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
        //Dimensions
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(pWidth, pHeight);
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
#pragma endregion
        
        using namespace wolf::graphics;

        // Create font texture
        uint8_t* _font_data = nullptr;
        int _width, _height;
        io.Fonts->GetTexDataAsRGBA32(&_font_data, &_width, &_height);
        VkDeviceSize _upload_size = _width * _height * 4 * sizeof(uint8_t);

        std::vector<uint8_t> _texture_data(_font_data, _font_data + _upload_size);
        _texture.load(_gDevice);
        _texture.initialize_texture_from_memory(
            _texture_data,
            (UINT)_width,
            (UINT)_height);
        /*VK_IMAGE_TYPE_2D,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        VK_SHARING_MODE_EXCLUSIVE);*/

        
        auto __hr = this->_shader.load(pGDevice, content_path + L"shaders/imgui.vert.spv", w_shader_stage::VERTEX_SHADER);
        if (__hr != S_OK)
        {
            V(__hr, "loading vertex shader", this->_name);
            return S_FALSE;
        }
        __hr = this->_shader.load(pGDevice, content_path + L"shaders/imgui.frag.spv", w_shader_stage::FRAGMENT_SHADER);
        if (__hr != S_OK)
        {
            V(__hr, "loading fragment shader", this->_name);
            return S_FALSE;
        }
        
        w_shader_binding_param _sampler_param;
        _sampler_param.index = 0;
        _sampler_param.stage = w_shader_stage::FRAGMENT_SHADER;
        _sampler_param.type = w_shader_binding_type::SAMPLER;
        _sampler_param.sampler_info = _texture.get_descriptor_info();
        
        this->_shader.set_shader_binding_params({_sampler_param});
        
        auto _descriptor_set_layout = this->_shader.get_descriptor_set_layout_binding();
        const VkPipelineLayoutCreateInfo _pipeline_layout_create_info =
        {
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,                          // Type
            nullptr,                                                                // Next
            0,                                                                      // Flags
            static_cast<uint32_t>(_descriptor_set_layout == nullptr ? 0 : 1),       // SetLayoutCount
            _descriptor_set_layout == nullptr ? nullptr : &_descriptor_set_layout,  // SetLayouts
            0,                                                                      // PushConstantRangeCount
            nullptr                                                                 // PushConstantRanges
        };
        
        this->_pipeline_layout =  w_pipeline::create_pipeline_layout(_gDevice, &_pipeline_layout_create_info);
        if (!this->_pipeline_layout)
        {
            V(S_FALSE, "creating pipeline layout", this->_name);
            return S_FALSE;
        }
        // Pipeline cache
        VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
        pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
        auto _hr = vkCreatePipelineCache(
            _gDevice->vk_device,
            &pipelineCacheCreateInfo,
            nullptr,
            &this->_pipeline_cache);

        // Pipeline layout
        // Push constants for UI rendering parameters
        VkPushConstantRange pushConstantRange =
        {
            VK_SHADER_STAGE_VERTEX_BIT,
            0,
            sizeof(push_constant_block)
        };

        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = 
        {
            VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            nullptr,
            0,
            1,
            &_descriptor_set_layout,
            1,
            &pushConstantRange
        };
        _hr = vkCreatePipelineLayout(
            _gDevice->vk_device, 
            &pipelineLayoutCreateInfo, 
            nullptr, 
            &_pipeline_layout);

        // Setup graphics pipeline for UI rendering
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
        {
            VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            nullptr,
            0,
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            VK_FALSE
        };
        VkPipelineRasterizationStateCreateInfo rasterizationState = {};
        rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizationState.cullMode = VK_CULL_MODE_NONE;
        rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationState.depthClampEnable = VK_FALSE;
        rasterizationState.lineWidth = 1.0f;

        // Enable blending
        VkPipelineColorBlendAttachmentState blendAttachmentState{};
        blendAttachmentState.blendEnable = VK_TRUE;
        blendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlendState = {};
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendState.attachmentCount = 1;
        colorBlendState.pAttachments = &blendAttachmentState;
        colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

        VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
        depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilState.depthTestEnable = VK_FALSE;
        depthStencilState.depthWriteEnable = VK_FALSE;
        depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencilState.front = depthStencilState.back;
        depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;

        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
        viewportState.flags = 0;

        VkPipelineMultisampleStateCreateInfo multisampleState = {};
        multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampleState.flags = 0;

        std::vector<VkDynamicState> dynamicStateEnables = 
        {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicState = {};
        dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.pDynamicStates = dynamicStateEnables.data();
        dynamicState.dynamicStateCount = (UINT)dynamicStateEnables.size();

        auto _shader_stages = this->_shader.get_shader_stages();

        VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.layout = this->_pipeline_layout;
        pipelineCreateInfo.renderPass = pRenderPass;
        pipelineCreateInfo.renderPass = pRenderPass;
        pipelineCreateInfo.flags = 0;
        pipelineCreateInfo.basePipelineIndex = -1;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
        pipelineCreateInfo.pRasterizationState = &rasterizationState;
        pipelineCreateInfo.pColorBlendState = &colorBlendState;
        pipelineCreateInfo.pMultisampleState = &multisampleState;
        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pDepthStencilState = &depthStencilState;
        pipelineCreateInfo.pDynamicState = &dynamicState;
        pipelineCreateInfo.stageCount = static_cast<UINT>(_shader_stages->size());
        pipelineCreateInfo.pStages = _shader_stages->data();

        // Vertex bindings an attributes based on ImGui vertex definition
        std::vector<VkVertexInputBindingDescription> _vertex_binding_descriptions =
        {
            {
                0,                                                                                        // Binding
                sizeof(ImDrawVert),                                                                       // Stride
                VK_VERTEX_INPUT_RATE_VERTEX                                                               // InputRate
            }
        };
        std::vector<VkVertexInputAttributeDescription> _vertex_attribute_descriptions =
        {
            {
                0,                                                                          // Location
                _vertex_binding_descriptions[0].binding,                                             // Binding
                VK_FORMAT_R32G32_SFLOAT,                                                    // Format
                offsetof(ImDrawVert, pos)                                                   // Offset
            },
            {
                1,                                                                          // Location
                _vertex_binding_descriptions[0].binding,                                             // Binding
                VK_FORMAT_R32G32_SFLOAT,                                                    // Format
                offsetof(ImDrawVert, uv)                                                   // Offset
            },
            {
                2,                                                                          // Location
                _vertex_binding_descriptions[0].binding,                                             // Binding
                VK_FORMAT_R8G8B8A8_UNORM,                                                    // Format
                offsetof(ImDrawVert, col)                                                   // Offset
            },
        };
        VkPipelineVertexInputStateCreateInfo _vertex_input_state_create_info =
        {
            VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,    // Type
            nullptr,                                                      // Next
            0,                                                            // Flags
            static_cast<uint32_t>(_vertex_binding_descriptions.size()),   // VertexBindingDescriptionCount
            _vertex_binding_descriptions.size() ?
            &_vertex_binding_descriptions[0] : nullptr,                   // VertexBindingDescriptions
            static_cast<uint32_t>(_vertex_attribute_descriptions.size()), // VertexAttributeDescriptionCount
            _vertex_attribute_descriptions.size() ?
            &_vertex_attribute_descriptions[0] : nullptr                  // VertexAttributeDescriptions
        };

        pipelineCreateInfo.pVertexInputState = &_vertex_input_state_create_info;

        _hr = vkCreateGraphicsPipelines(
            pGDevice->vk_device, 
            this->_pipeline_cache, 
            1, 
            &pipelineCreateInfo, 
            nullptr, 
            &this->_pipeline);

        return _hr == VK_SUCCESS ? S_OK : S_FALSE;
    }

    HRESULT update_buffers(_In_ wolf::graphics::w_render_pass& pRenderPass)
    {
        
        ImDrawData* _im_draw_data = ImGui::GetDrawData();

        // Note: Alignment is done inside buffer creation
        UINT _vertex_buffer_size = _im_draw_data->TotalVtxCount * sizeof(ImDrawVert);
        UINT _index_buffer_size = _im_draw_data->TotalIdxCount * sizeof(ImDrawIdx);

        // Update buffers only if vertex or index count has been changed compared to current buffer size
        HRESULT _hr;
        
        //Vertex buffer
        if ( !this->_vertex_buffer || this->_vertex_buffer->get_size() != _vertex_buffer_size)
        {
            SAFE_RELEASE(this->_vertex_buffer);
            this->_vertex_buffer = new wolf::graphics::w_buffer();
            _hr = this->_vertex_buffer->load_as_staging(this->_gDevice, _vertex_buffer_size);
            if (_hr == S_FALSE)
            {
                V(_hr, "loading staging index buffer", this->_name);
                return _hr;
            }
        }

        // Index buffer
        if (!this->_index_buffer || this->_index_buffer->get_size() != _index_buffer_size)
        {
            SAFE_RELEASE(this->_index_buffer);
            this->_index_buffer = new wolf::graphics::w_buffer();
            _hr = this->_index_buffer->load_as_staging(this->_gDevice, _index_buffer_size);
            if (_hr == S_FALSE)
            {
                V(_hr, "loading staging vertex buffer", this->_name);
                return _hr;
            }
        }

        _hr = this->_vertex_buffer->bind();
        if (_hr == S_FALSE)
        {
            V(_hr, "binding staging vertex buffer", this->_name);
            return _hr;
        }
        _hr = this->_index_buffer->bind();
        if (_hr == S_FALSE)
        {
            V(_hr, "binding staging index buffer", this->_name);
            return _hr;
        }
        
        ImDrawVert* vtxDst = (ImDrawVert*)this->_vertex_buffer->map();
        ImDrawIdx* idxDst = (ImDrawIdx*)this->_index_buffer->map();

        for (int n = 0; n < _im_draw_data->CmdListsCount; n++)
        {
            const ImDrawList* cmd_list = _im_draw_data->CmdLists[n];
            memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
            vtxDst += cmd_list->VtxBuffer.Size;
            idxDst += cmd_list->IdxBuffer.Size;
        }
        
        this->_vertex_buffer->flush();
        this->_index_buffer->flush();

        this->_vertex_buffer->unmap();
        this->_index_buffer->unmap();
        
        vtxDst = nullptr;
        idxDst = nullptr;

        return S_OK;
    }

    void new_frame()
    {
        ImGui::NewFrame();

        ImGui::Text("Test");

        ImGui::Render();
    }

    void render(_In_ VkCommandBuffer pCommandBuffer, _In_ float pDeltaTimeTicks)
    {
        ImGuiIO& io = ImGui::GetIO();
        
        io.DisplaySize = ImVec2((float)800, (float)600);
        io.DeltaTime = pDeltaTimeTicks;

        auto _descriptor_set = this->_shader.get_descriptor_set();
        vkCmdBindDescriptorSets(
            pCommandBuffer, 
            VK_PIPELINE_BIND_POINT_GRAPHICS, 
            this->_pipeline_layout, 
            0, 
            1, 
            &_descriptor_set, 
            0, 
            nullptr);
        vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, this->_pipeline);

        // Bind vertex and index buffer
        auto _vertex_buffer_handle = this->_vertex_buffer->get_handle();
        auto _index_buffer_handle = this->_index_buffer->get_handle();

        VkDeviceSize offsets[1] = { 0 };
        vkCmdBindVertexBuffers(pCommandBuffer, 0, 1, &_vertex_buffer_handle, offsets);
        vkCmdBindIndexBuffer(pCommandBuffer, _index_buffer_handle, 0, VK_INDEX_TYPE_UINT16);

        VkViewport viewport;
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = ImGui::GetIO().DisplaySize.x;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(pCommandBuffer, 0, 1, &viewport);

        // UI scale and translate via push constants
        _push_constant_block.scale[0] = 2.0f / io.DisplaySize.x;
        _push_constant_block.scale[1] = 2.0f / io.DisplaySize.y;
        _push_constant_block.translate[0] = -1.0f;
        _push_constant_block.translate[1] = -1.0f;

        vkCmdPushConstants(
            pCommandBuffer, 
            this->_pipeline_layout, 
            VK_SHADER_STAGE_VERTEX_BIT, 
            0, 
            sizeof(_push_constant_block),
            &_push_constant_block);

        // Render commands
        ImDrawData* imDrawData = ImGui::GetDrawData();
        int32_t vertexOffset = 0;
        int32_t indexOffset = 0;
        for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
        {
            const ImDrawList* cmd_list = imDrawData->CmdLists[i];
            for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
            {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                VkRect2D scissorRect;
                scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
                scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
                scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
                scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
                vkCmdSetScissor(pCommandBuffer, 0, 1, &scissorRect);
                vkCmdDrawIndexed(pCommandBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
                indexOffset += pcmd->ElemCount;
            }
            vertexOffset += cmd_list->VtxBuffer.Size;
        }
    }

    ULONG release()
    {   
        this->_gDevice = nullptr;
        return 1;
    }

private:
    std::string                                         _name;
    std::shared_ptr<wolf::graphics::w_graphics_device>  _gDevice;
    
    wolf::graphics::w_buffer* _vertex_buffer;
    wolf::graphics::w_buffer* _index_buffer;
    VkPipelineCache _pipeline_cache;
    VkPipelineLayout _pipeline_layout;
    VkPipeline _pipeline;
    wolf::graphics::w_shader _shader;
    wolf::graphics::w_texture _texture;

    struct push_constant_block 
    {
        float scale[2];
        float translate[2];
    } _push_constant_block;
};

imgui_imp::imgui_imp() : _pimp(new imgui_pimp())
{

}

imgui_imp::~imgui_imp()
{

}

HRESULT imgui_imp::load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
    _In_ const float& pWidth,
    _In_ const float& pHeight,
    _In_ VkRenderPass& pRenderPass)
{
    return this->_pimp ? this->_pimp->load(pGDevice, pWidth, pHeight, pRenderPass) : S_FALSE;
}

HRESULT imgui_imp::update_buffers(_In_ wolf::graphics::w_render_pass& pRenderPass)
{
    return this->_pimp ? this->_pimp->update_buffers(pRenderPass) : S_FALSE;
}

void imgui_imp::new_frame()
{
    if (!this->_pimp) return;
    this->_pimp->new_frame();
}

void imgui_imp::render(_In_ VkCommandBuffer pCommandBuffer, _In_ float pTotalTime)
{
    if (!this->_pimp) return;
    this->_pimp->render(pCommandBuffer, pTotalTime);
}

ULONG imgui_imp::release()
{
    if (_super::get_is_released()) return 0;

    SAFE_RELEASE(this->_pimp);

    return _super::release();
}

