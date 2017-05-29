#include "w_render_pch.h"
#include "w_imgui.h"
#include "w_buffer.h"
#include "w_shader.h"
#include "w_render_pass.h"
#include "w_pipeline.h"

using namespace wolf::graphics;

namespace wolf
{
    namespace graphics
    {

        class w_imgui_pimp
        {
        public:
            w_imgui_pimp() :
                _gDevice(nullptr),
                _vertex_buffer(nullptr),
                _index_buffer(nullptr),
                _images_texture(nullptr)
            {
            }

            HRESULT load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
                _In_ HWND pHWND,
                _In_ const float& pWidth,
                _In_ const float& pHeight,
                _In_ VkRenderPass& pRenderPass,
                _In_ w_texture* pImageTexure)
            {
                this->_gDevice = pGDevice;
                this->_hwnd = pHWND;
                this->_width = pWidth;
                this->_height = pHeight;
                this->_images_texture = pImageTexure;

#pragma region Set Style
                //Color scheme
                ImGuiStyle& style = ImGui::GetStyle();
                style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 1.0f, 0.6f);
                style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.9098039215686275f, 0.4431372549019608f, 0.3254901960784314f, 1.0f);
                style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
                style.Colors[ImGuiCol_Header] = ImVec4(0.0f, 1.0f, 0.0f, 0.4f);
                style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
                //Dimensions
                ImGuiIO& _io = ImGui::GetIO();
                _io.DisplaySize = ImVec2(this->_width, this->_height);
                _io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
#pragma endregion

#pragma region Set Key Maps

                /*
                    Keyboard mapping.
                    ImGui will use those indices to peek into the io.KeyDown[] array that
                    we will update during the application lifetime.
                */
                _io.KeyMap[ImGuiKey_Tab] = VK_TAB;
                _io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
                _io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
                _io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
                _io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
                _io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
                _io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
                _io.KeyMap[ImGuiKey_Home] = VK_HOME;
                _io.KeyMap[ImGuiKey_End] = VK_END;
                _io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
                _io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
                _io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
                _io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
                _io.KeyMap[ImGuiKey_A] = 'A';
                _io.KeyMap[ImGuiKey_C] = 'C';
                _io.KeyMap[ImGuiKey_V] = 'V';
                _io.KeyMap[ImGuiKey_X] = 'X';
                _io.KeyMap[ImGuiKey_Y] = 'Y';
                _io.KeyMap[ImGuiKey_Z] = 'Z';

                _io.RenderDrawListsFn = NULL;
                _io.ImeWindowHandle = this->_hwnd;

#pragma endregion

                using namespace wolf::graphics;

                // Create fonts texture
                uint8_t* _font_data = nullptr;
                int _texture_width, _texture_height;
                _io.Fonts->GetTexDataAsRGBA32(&_font_data, &_texture_width, &_texture_height);
                UINT _upload_size = _texture_width * _texture_height * 4 * sizeof(uint8_t);

                if (_font_texture)
                {
                    SAFE_RELEASE(this->_font_texture);
                }
                this->_font_texture = new w_texture();
                this->_font_texture->load(_gDevice);

                std::vector<uint8_t> _texture_data(_font_data, _font_data + _upload_size);
                _font_texture->initialize_texture_from_memory(
                    _texture_data,
                    (UINT)_texture_width,
                    (UINT)_texture_height);

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

                w_shader_binding_param _font_sampler_param;
                _font_sampler_param.index = 0;
                _font_sampler_param.stage = w_shader_stage::FRAGMENT_SHADER;
                _font_sampler_param.type = w_shader_binding_type::SAMPLER;
                _font_sampler_param.sampler_info = this->_font_texture->get_descriptor_info();

                w_shader_binding_param _image_sampler_param;
                _image_sampler_param.index = 1;
                _image_sampler_param.stage = w_shader_stage::FRAGMENT_SHADER;
                _image_sampler_param.type = w_shader_binding_type::SAMPLER;
                _image_sampler_param.sampler_info = this->_images_texture->get_descriptor_info();

                this->_shader.load_shader_binding_params({ _font_sampler_param,_image_sampler_param });

                auto _descriptor_set_layout = this->_shader.get_descriptor_set_layout_binding();

                // Push constants for UI rendering parameters
                VkPushConstantRange pushConstantRange =
                {
                    VK_SHADER_STAGE_VERTEX_BIT,
                    0,
                    sizeof(push_constant_block)
                };


                VkPipelineLayoutCreateInfo _pipeline_layout_create_info = {};
                _pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                _pipeline_layout_create_info.setLayoutCount = 1;
                _pipeline_layout_create_info.pSetLayouts = &_descriptor_set_layout;
                _pipeline_layout_create_info.pushConstantRangeCount = 1;
                _pipeline_layout_create_info.pPushConstantRanges = &pushConstantRange;

                this->_pipeline_layout = w_pipeline::create_pipeline_layout(_gDevice, &_pipeline_layout_create_info);
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

#ifdef __WIN32
            LRESULT on_msg_proc(
                _In_ const HWND pHWND,
                _In_ const UINT pMessage,
                _In_ const WPARAM pWParam,
                _In_ const LPARAM pLParam)
            {
                ImGuiIO& io = ImGui::GetIO();
                switch (pMessage)
                {
                case WM_LBUTTONDOWN:
                    io.MouseDown[0] = true;
                    return true;
                case WM_LBUTTONUP:
                    io.MouseDown[0] = false;
                    return true;
                case WM_RBUTTONDOWN:
                    io.MouseDown[1] = true;
                    return true;
                case WM_RBUTTONUP:
                    io.MouseDown[1] = false;
                    return true;
                case WM_MBUTTONDOWN:
                    io.MouseDown[2] = true;
                    return true;
                case WM_MBUTTONUP:
                    io.MouseDown[2] = false;
                    return true;
                case WM_MOUSEWHEEL:
                    io.MouseWheel += GET_WHEEL_DELTA_WPARAM(pWParam) > 0 ? +1.0f : -1.0f;
                    return true;
                case WM_MOUSEMOVE:
                    io.MousePos.x = (signed short)(pLParam);
                    io.MousePos.y = (signed short)(pLParam >> 16);
                    return true;
                case WM_KEYDOWN:
                    if (pWParam < 256)
                    {
                        io.KeysDown[pWParam] = 1;
                    }
                    return true;
                case WM_KEYUP:
                    if (pWParam < 256)
                    {
                        io.KeysDown[pWParam] = 0;
                    }
                    return true;
                case WM_CHAR:
                    // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
                    if (pWParam > 0 && pWParam < 0x10000)
                        io.AddInputCharacter((unsigned short)pWParam);
                    return true;
                }
                return 0;
            }

#endif

            HRESULT update_buffers(_In_ wolf::graphics::w_render_pass& pRenderPass)
            {
                ImDrawData* _im_draw_data = ImGui::GetDrawData();
                if (!_im_draw_data || !_im_draw_data->CmdListsCount) return S_OK;

                // Note: Alignment is done inside buffer creation
                UINT _vertex_buffer_size = _im_draw_data->TotalVtxCount * sizeof(ImDrawVert);
                UINT _index_buffer_size = _im_draw_data->TotalIdxCount * sizeof(ImDrawIdx);
                
                // Update buffers only if vertex or index count has been changed compared to current buffer size
                HRESULT _hr;

                //Vertex buffer
                if (!this->_vertex_buffer || this->_vertex_buffer->get_size() != _vertex_buffer_size)
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

                _hr = this->_vertex_buffer->flush();
                if (_hr == S_FALSE)
                {
                    V(_hr, "flushing staging index buffer", this->_name);
                    return _hr;
                }
                _hr = this->_index_buffer->flush();
                if (_hr == S_FALSE)
                {
                    V(_hr, "flushing staging index buffer", this->_name);
                    return _hr;
                }

                this->_vertex_buffer->unmap();
                this->_index_buffer->unmap();

                vtxDst = nullptr;
                idxDst = nullptr;

                return S_OK;
            }

            void new_frame(_In_ float pDeltaTime, _In_ const std::function<void(void)>& pMakeGuiWork)
            {
                ImGuiIO& _io = ImGui::GetIO();
                //check for new size
                _io.DisplaySize = ImVec2((float)this->_width, (float)this->_height);
                _io.DeltaTime = pDeltaTime;

                // Read keyboard modifiers inputs
                _io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                _io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                _io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
                _io.KeySuper = false;
                // io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
                // io.MousePos : filled by WM_MOUSEMOVE events
                // io.MouseDown : filled by WM_*BUTTON* events
                // io.MouseWheel : filled by WM_MOUSEWHEEL events

                // Hide OS mouse cursor if ImGui is drawing it
                if (_io.MouseDrawCursor)
                {
                    SetCursor(NULL);
                }

                ImGui::NewFrame();

                pMakeGuiWork();

                ImGui::Render();
            }

            void render(_In_ VkCommandBuffer pCommandBuffer)
            {
                ImGuiIO& _io = ImGui::GetIO();

                ImDrawData* imDrawData = ImGui::GetDrawData();
                if (!imDrawData || !imDrawData->CmdListsCount) return;
                
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
                viewport.height = ImGui::GetIO().DisplaySize.y;
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                vkCmdSetViewport(pCommandBuffer, 0, 1, &viewport);

                // Render commands
                bool _need_to_update_push = true;
                int _last_texture_id = 0;

                int32_t vertexOffset = 0;
                int32_t indexOffset = 0;
                for (int32_t i = 0; i < imDrawData->CmdListsCount; i++)
                {
                    const ImDrawList* cmd_list = imDrawData->CmdLists[i];
                    for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; j++)
                    {
                        const ImDrawCmd* _draw_cmds = &cmd_list->CmdBuffer[j];
                        if (_draw_cmds->TextureId == nullptr)
                        {
                            //font
                            _push_constant_block.image_index = 0;
                            if (_last_texture_id != 0)
                            {
                                _last_texture_id = 0;
                                _need_to_update_push = true;
                            }
                        }
                        else
                        {
                            //image
                            _push_constant_block.image_index = 1;
                            if (_last_texture_id != 1)
                            {
                                _last_texture_id = 1;
                                _need_to_update_push = true;
                            }
                        }

                        if (_need_to_update_push)
                        {
                            _push_constant_block.scale[0] = 2.0f / _io.DisplaySize.x;
                            _push_constant_block.scale[1] = 2.0f / _io.DisplaySize.y;
                            _push_constant_block.translate[0] = -1.0f;
                            _push_constant_block.translate[1] = -1.0f;

                            vkCmdPushConstants(
                                pCommandBuffer,
                                this->_pipeline_layout,
                                VK_SHADER_STAGE_VERTEX_BIT,
                                0,
                                sizeof(_push_constant_block),
                                &_push_constant_block);

                            _need_to_update_push = false;
                        }


                        VkRect2D scissorRect;
                        scissorRect.offset.x = std::max((int32_t)(_draw_cmds->ClipRect.x), 0);
                        scissorRect.offset.y = std::max((int32_t)(_draw_cmds->ClipRect.y), 0);
                        scissorRect.extent.width = (uint32_t)(_draw_cmds->ClipRect.z - _draw_cmds->ClipRect.x);
                        scissorRect.extent.height = (uint32_t)(_draw_cmds->ClipRect.w - _draw_cmds->ClipRect.y);
                        vkCmdSetScissor(pCommandBuffer, 0, 1, &scissorRect);
                        vkCmdDrawIndexed(pCommandBuffer, _draw_cmds->ElemCount, 1, indexOffset, vertexOffset, 0);
                        indexOffset += _draw_cmds->ElemCount;
                    }
                    vertexOffset += cmd_list->VtxBuffer.Size;
                }
            }

            ULONG release()
            {
                ImGui::Shutdown();

                SAFE_RELEASE(this->_vertex_buffer);
                SAFE_RELEASE(this->_index_buffer);

                SAFE_RELEASE(this->_images_texture);
                SAFE_RELEASE(this->_font_texture);


                vkDestroyPipelineCache(this->_gDevice->vk_device, this->_pipeline_cache, nullptr);
                this->_pipeline_cache = nullptr;

                vkDestroyPipelineLayout(this->_gDevice->vk_device, this->_pipeline_layout, nullptr);
                this->_pipeline_layout = nullptr;

                vkDestroyPipeline(this->_gDevice->vk_device, this->_pipeline, nullptr);
                this->_pipeline = nullptr;

                _shader.release();

                this->_width = 0;
                this->_height = 0;

#ifdef __WIN32
                this->_hwnd = NULL;
#endif
                this->_gDevice = nullptr;

                return 1;
            }

#pragma region Setters
            UINT get_width() const
            {
                return this->_width;
            }

            UINT get_height() const
            {
                return this->_height;
            }
#pragma endregion

#pragma region Setters
            void set_width(_In_ const UINT& pWidth)
            {
                this->_width = pWidth;
            }

            void set_height(_In_ const UINT& pHeight)
            {
                this->_height = pHeight;
            }
#pragma endregion

        private:
            std::string                                         _name;
            std::shared_ptr<wolf::graphics::w_graphics_device>  _gDevice;

#ifdef __WIN32
            HWND _hwnd;
#endif

            w_buffer*                   _vertex_buffer;
            w_buffer*                   _index_buffer;
            VkPipelineCache             _pipeline_cache;
            VkPipelineLayout            _pipeline_layout;
            VkPipeline                  _pipeline;
            w_shader                    _shader;
            w_texture*                  _font_texture;
            w_texture*                  _images_texture;
            UINT                        _width;
            UINT                        _height;

            struct push_constant_block
            {
                float scale[2];
                float translate[2];
                int image_index;
            } _push_constant_block;
        };
    }
}

using namespace wolf::graphics;

bool w_imgui::_is_released = false;
w_imgui_pimp* w_imgui::_pimp = new w_imgui_pimp();

HRESULT w_imgui::load(_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
    _In_ HWND pHWND,
    _In_ const float& pWidth,
    _In_ const float& pHeight,
    _In_ VkRenderPass& pRenderPass,
    _In_ w_texture* pImageTexure)
{
    return _pimp ? _pimp->load(pGDevice, pHWND, pWidth, pHeight, pRenderPass, pImageTexure) : S_FALSE;
}

LRESULT w_imgui::on_msg_proc(
    _In_ const HWND pHWND,
    _In_ const UINT pMessage,
    _In_ const WPARAM pWParam,
    _In_ const LPARAM pLParam)
{
    return _pimp ? _pimp->on_msg_proc(pHWND, pMessage, pWParam, pLParam) : S_FALSE;
}

HRESULT w_imgui::update_buffers(_In_ wolf::graphics::w_render_pass& pRenderPass)
{
    return _pimp ? _pimp->update_buffers(pRenderPass) : S_FALSE;
}

void w_imgui::new_frame(_In_ float pDeltaTime, _In_ const std::function<void(void)>& pMakeGuiWork)
{
    if (!_pimp) return;
    _pimp->new_frame(pDeltaTime, pMakeGuiWork);
}

void w_imgui::render(_In_ VkCommandBuffer pCommandBuffer)
{
    if (!_pimp) return;
    _pimp->render(pCommandBuffer);
}

ULONG w_imgui::release()
{
    if (_is_released) return 0;

    SAFE_RELEASE(w_imgui::_pimp);

    return 1;
}

#pragma region Getters

UINT w_imgui::get_width()
{
    return _pimp ? _pimp->get_width() : 0;
}

UINT w_imgui::get_height()
{
    return _pimp ? _pimp->get_height() : 0;
}

#pragma endregion

#pragma region Setters

void w_imgui::set_width(_In_ const UINT& pWidth)
{
    if (!_pimp) return;
    _pimp->set_width(pWidth);
}

void w_imgui::set_height(_In_ const UINT& pHeight)
{
    if (!_pimp) return;
    _pimp->set_width(pHeight);
}

#pragma endregion

