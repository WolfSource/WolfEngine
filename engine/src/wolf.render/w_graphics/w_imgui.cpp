#include "w_render_pch.h"
#include "w_imgui.h"
#include "w_buffer.h"
#include "w_shader.h"
#include "w_render_pass.h"
#include "w_pipeline.h"
#include "w_command_buffer.h"
#include "w_render_pass.h"

using namespace wolf::graphics;

static int64_t  g_Time = 0;

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
                _font_texture(nullptr)
            {
            }

			W_RESULT load(
				_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
				_In_ const w_output_presentation_window* pOutputPresentationWindow,
				_In_ const w_viewport& pViewport,
				_In_ const w_viewport_scissor& pViewportScissor,
				_In_ w_texture* pIconTexture,
				_In_ w_texture** pStagingMediaTexture,
				_In_ const char* pFontPath,
				_In_ const float& pFontPixelSize)
			{
				const std::string _trace_info = this->_name + "::load";

#ifdef __WIN32
				this->_hwnd = pOutputPresentationWindow->hwnd;
#endif
				this->_gDevice = pGDevice;
				this->_screen_size.x = pOutputPresentationWindow->width;
				this->_screen_size.y = pOutputPresentationWindow->height;
				this->_images_texture = pIconTexture;
				this->_media_player_texture = pStagingMediaTexture ? *pStagingMediaTexture : nullptr;

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
				_io.DisplaySize = ImVec2((float)this->_screen_size.x, (float)this->_screen_size.y);
				_io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

				if (pFontPath)
				{
					_io.Fonts->AddFontFromFileTTF(pFontPath, pFontPixelSize);
				}

#pragma endregion


#ifdef __WIN32
				_io.ImeWindowHandle = this->_hwnd;

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
#pragma endregion

#endif
				_io.KeyMap[ImGuiKey_A] = 'A';
				_io.KeyMap[ImGuiKey_C] = 'C';
				_io.KeyMap[ImGuiKey_V] = 'V';
				_io.KeyMap[ImGuiKey_X] = 'X';
				_io.KeyMap[ImGuiKey_Y] = 'Y';
				_io.KeyMap[ImGuiKey_Z] = 'Z';

				_io.RenderDrawListsFn = NULL;

				using namespace wolf::graphics;

				std::vector<std::vector<w_image_view>> _attachments =
				{
					{ pOutputPresentationWindow->swap_chain_image_views[0], pOutputPresentationWindow->depth_buffer_image_view },
					{ pOutputPresentationWindow->swap_chain_image_views[1], pOutputPresentationWindow->depth_buffer_image_view },
				};

				//initialize attachment buffers
				_attachments[0][0].attachment_desc.desc.loadOp = _attachments[1][0].attachment_desc.desc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				//make sure use output presentation window format
				_attachments[0][0].attachment_desc.desc.format = _attachments[1][0].attachment_desc.desc.format = pOutputPresentationWindow->vk_swap_chain_selected_format.format;

				//do not clear frame buffer
				_attachments[1][0].attachment_desc.desc.loadOp = _attachments[1][1].attachment_desc.desc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				_attachments[1][0].attachment_desc.desc.storeOp = _attachments[1][1].attachment_desc.desc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				
				//create render pass
				auto __hr = this->_render_pass.load(_gDevice,
					pViewport,
					pViewportScissor,
					_attachments);
				if (__hr == W_FAILED)
				{
					V(W_FAILED, "creating render pass", _trace_info, 3);
					release();
					return W_FAILED;
				}
				_attachments.clear();

				__hr = this->_command_buffers.load(_gDevice, pOutputPresentationWindow->swap_chain_image_views.size());
				if (__hr == W_FAILED)
				{
					release();
					V(W_FAILED, "creating command buffers", _trace_info, 3);
					return W_FAILED;
				}

				// Create fonts texture
				uint8_t* _font_data = nullptr;
				int _texture_width, _texture_height;
				_io.Fonts->GetTexDataAsRGBA32(&_font_data, &_texture_width, &_texture_height);
				uint32_t _upload_size = _texture_width * _texture_height * 4 * sizeof(uint8_t);

				if (_font_texture)
				{
					SAFE_RELEASE(this->_font_texture);
				}
				this->_font_texture = new w_texture();
				this->_font_texture->initialize(_gDevice,
					static_cast<uint32_t>(_texture_width),
					static_cast<uint32_t>(_texture_height),
					false);
				std::vector<uint8_t> _texture_data(_font_data, _font_data + _upload_size);
				_font_texture->load_texture_from_memory_rgba(
					_texture_data.data());

				__hr = this->_shader.load(pGDevice, content_path + L"shaders/imgui.vert.spv", w_shader_stage::VERTEX_SHADER);
				if (__hr != W_PASSED)
				{
					V(__hr, "loading vertex shader", _trace_info, 3);
					release();
					return W_FAILED;
				}
				__hr = this->_shader.load(pGDevice, content_path + L"shaders/imgui.frag.spv", w_shader_stage::FRAGMENT_SHADER);
				if (__hr != W_PASSED)
				{
					V(__hr, "loading fragment shader", _trace_info, 3);
					release();
					return W_FAILED;
				}

				std::vector<w_shader_binding_param> _shader_params;

				w_shader_binding_param _param;

				//binding 0 for font's image
				_param.index = 0;
				_param.stage = w_shader_stage::FRAGMENT_SHADER;
				_param.type = w_shader_binding_type::SAMPLER2D;
				_param.image_info = this->_font_texture->get_descriptor_info();
				_shader_params.push_back(_param);

				//binding 1 for icon's image
				_param.index = 1;
				_param.stage = w_shader_stage::FRAGMENT_SHADER;
				_param.type = w_shader_binding_type::SAMPLER2D;
				_param.image_info = this->_images_texture ? this->_images_texture->get_descriptor_info()
					: w_texture::default_texture->get_descriptor_info();
				_shader_params.push_back(_param);

				//binding 2 for media player's data
				_param.index = 2;
				_param.stage = w_shader_stage::FRAGMENT_SHADER;
				_param.type = w_shader_binding_type::SAMPLER2D;
				_param.image_info = this->_media_player_texture ? this->_media_player_texture->get_descriptor_info()
					: w_texture::default_texture->get_descriptor_info();
				_shader_params.push_back(_param);

				this->_shader.set_shader_binding_params(_shader_params);

				auto _descriptor_set_layout = this->_shader.get_descriptor_set_layout();

				// Push constants for UI rendering parameters
				VkPushConstantRange _push_constant_range =
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
				_pipeline_layout_create_info.pPushConstantRanges = &_push_constant_range;

				this->_pipeline_layout = w_pipeline::create_pipeline_layout(_gDevice, &_pipeline_layout_create_info);
				if (!this->_pipeline_layout)
				{
					V(W_FAILED, "creating pipeline layout", _trace_info, 3);
					release();
					return W_FAILED;
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
				dynamicState.dynamicStateCount = (uint32_t)dynamicStateEnables.size();

				auto _shader_stages = this->_shader.get_shader_stages();

				VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
				pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineCreateInfo.layout = this->_pipeline_layout;
				pipelineCreateInfo.renderPass = this->_render_pass.get_handle();
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
				pipelineCreateInfo.stageCount = static_cast<uint32_t>(_shader_stages->size());
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
				if (_hr == W_FAILED)
				{
					V(W_FAILED, "creating graphics pipeline", _trace_info);
					release();
					return W_FAILED;
				}

				return _hr == VK_SUCCESS ? W_PASSED : W_FAILED;
			}

            void new_frame(_In_ const float& pDeltaTime, _In_ const std::function<void(void)>& pMakeGuiWork)
            {
                ImGuiIO& _io = ImGui::GetIO();

                _io.MouseDown[0] = inputs_manager.mouse.left_button_pressed;
                _io.MouseDown[1] = inputs_manager.mouse.right_button_pressed;
                _io.MouseDown[2] = inputs_manager.mouse.middle_button_pressed;
                _io.MouseWheel = inputs_manager.mouse.wheel;
                _io.MousePos.x = inputs_manager.mouse.pos_x;
                _io.MousePos.y = inputs_manager.mouse.pos_y;

                for (auto& _key : inputs_manager.keyboard.keys_pressed)
                {
                    if (_key < 256)
                    {
                        _io.KeysDown[_key] = true;
                    }
                }
                for (auto& _key : inputs_manager.keyboard.keys_released)
                {
                    if (_key < 256)
                    {
                        _io.KeysDown[_key] = false;
                    }
                }
                for (auto& _char : inputs_manager.keyboard.inputed_chars)
                {
                    _io.AddInputCharacter(_char);
                }

                //check for new size
                _io.DisplaySize = ImVec2((float)this->_screen_size.x, (float)this->_screen_size.y);
                _io.DeltaTime = pDeltaTime / 100.0f;

                // Read keyboard modifiers inputs
#ifdef __WIN32
                _io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
                _io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                _io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
#endif
                _io.KeySuper = false;
                // io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
                // io.MousePos : filled by WM_MOUSEMOVE events
                // io.MouseDown : filled by WM_*BUTTON* events
                // io.MouseWheel : filled by WM_MOUSEWHEEL events

                // Hide OS mouse cursor if ImGui is drawing it
                if (_io.MouseDrawCursor)
                {
#ifdef __WIN32
                    SetCursor(NULL);
#endif
                }

                ImGui::NewFrame();
                pMakeGuiWork();
                ImGui::Render();
            }

			W_RESULT render()
            {
				const std::string _trace_info = this->_name + "::render";
				W_RESULT _hr = W_PASSED;

				auto _size = this->_command_buffers.get_commands_size();
				for (uint32_t i = 0; i < _size; ++i)
				{
					this->_command_buffers.begin(i);
					{
						this->_render_pass.begin(i, &this->_command_buffers);
						{
							if (_update_buffers() == W_PASSED)
							{
								_draw(this->_command_buffers.get_active_command());
							}
							else
							{
								_hr = W_FAILED;
							}
						}
						this->_render_pass.end(&this->_command_buffers);
					}
					this->_command_buffers.end(i);
				}
				return _hr;
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

				this->_command_buffers.release();
				this->_render_pass.release();

#ifdef __WIN32
                this->_hwnd = NULL;
#endif
                this->_gDevice = nullptr;

                return 0;
            }
            
#pragma region Getters
            uint32_t get_width() const
            {
                return this->_screen_size.x;
            }

            uint32_t get_height() const
            {
                return this->_screen_size.y;
            }

			VkCommandBuffer get_command_buffer_at(_In_ const uint32_t pFrameIndex) const
			{
				return this->_command_buffers.get_command_at(pFrameIndex);
			}

#pragma endregion

#pragma region Setters
            void set_width(_In_ const uint32_t& pWidth)
            {
                this->_screen_size.x = pWidth;
            }

            void set_height(_In_ const uint32_t& pHeight)
            {
                this->_screen_size.y = pHeight;
            }
#pragma endregion

        private:
			W_RESULT _update_buffers()
			{
				ImDrawData* _im_draw_data = ImGui::GetDrawData();
				if (!_im_draw_data || !_im_draw_data->CmdListsCount) return W_PASSED;

				// Note: Alignment is done inside buffer creation
				uint32_t _vertex_buffer_size = _im_draw_data->TotalVtxCount * sizeof(ImDrawVert);
				uint32_t _index_buffer_size = _im_draw_data->TotalIdxCount * sizeof(ImDrawIdx);

				// Update buffers only if vertex or index count has been changed compared to current buffer size
				W_RESULT _hr;

				//Vertex buffer
				if (!this->_vertex_buffer || this->_vertex_buffer->get_size() != _vertex_buffer_size)
				{
					SAFE_RELEASE(this->_vertex_buffer);

					//vertex bufer as property host visible memory
					this->_vertex_buffer = new wolf::graphics::w_buffer();
					_hr = this->_vertex_buffer->load(
						this->_gDevice,
						_vertex_buffer_size,
						VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
					if (_hr == W_FAILED)
					{
						V(_hr, "loading staging vertex buffer", this->_name);
						return _hr;
					}
					_hr = this->_vertex_buffer->bind();
					if (_hr == W_FAILED)
					{
						V(_hr, "binding staging vertex buffer", this->_name);
						return _hr;
					}
				}

				// Index buffer
				if (!this->_index_buffer || this->_index_buffer->get_size() != _index_buffer_size)
				{
					SAFE_RELEASE(this->_index_buffer);

					this->_index_buffer = new wolf::graphics::w_buffer();
					_hr = this->_index_buffer->load(
						this->_gDevice,
						_index_buffer_size,
						VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
					if (_hr == W_FAILED)
					{
						V(_hr, "loading staging index buffer", this->_name);
						return _hr;
					}
					_hr = this->_index_buffer->bind();
					if (_hr == W_FAILED)
					{
						V(_hr, "binding staging index buffer", this->_name);
						return _hr;
					}
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
				if (_hr == W_FAILED)
				{
					V(_hr, "flushing staging index buffer", this->_name);
					return _hr;
				}
				_hr = this->_index_buffer->flush();
				if (_hr == W_FAILED)
				{
					V(_hr, "flushing staging index buffer", this->_name);
					return _hr;
				}

				this->_vertex_buffer->unmap();
				this->_index_buffer->unmap();

				vtxDst = nullptr;
				idxDst = nullptr;

				return W_PASSED;
			}

			void _draw(_In_ VkCommandBuffer pCommandBuffer)
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
						const char* _tex_id = static_cast<char*>(_draw_cmds->TextureId);
						if (_tex_id == nullptr)
						{
							//font
							_push_constant_block.image_index = 0;
							if (_last_texture_id != 0)
							{
								_last_texture_id = 0;
								_need_to_update_push = true;
							}
						}
						else if (strcmp(_tex_id, "#i") == 0)
						{
							_push_constant_block.image_index = 1;
							if (_last_texture_id != 1)
							{
								_last_texture_id = 1;
								_need_to_update_push = true;
							}
						}
						else if (strcmp(_tex_id, "#s") == 0)
						{
							_push_constant_block.image_index = 2;
							if (_last_texture_id != 2)
							{
								_last_texture_id = 2;
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

            std::string                                         _name;
            std::shared_ptr<wolf::graphics::w_graphics_device>  _gDevice;

#ifdef __WIN32
            HWND _hwnd;
#endif

            w_buffer*                                               _vertex_buffer;
            w_buffer*                                               _index_buffer;
            VkPipelineCache                                         _pipeline_cache;
            VkPipelineLayout                                        _pipeline_layout;
            VkPipeline                                              _pipeline;
            w_shader                                                _shader;
            w_texture*                                              _font_texture;
            w_texture*                                              _images_texture;
            w_texture*                                              _media_player_texture;
            w_point_t                                               _screen_size;

            struct push_constant_block
            {
                float scale[2];
                float translate[2];
                int image_index;
            } _push_constant_block;

			//for rendering
			w_command_buffer										_command_buffers;
			w_render_pass											_render_pass;
        };
    }
}

using namespace wolf::graphics;

bool w_imgui::_is_released = false;
w_imgui_pimp* w_imgui::_pimp = nullptr;

W_RESULT w_imgui::load(
	_In_ const std::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
	_In_ const w_output_presentation_window* pOutputPresentationWindow,
	_In_ const w_viewport& pViewport,
	_In_ const w_viewport_scissor& pViewportScissor,
	_In_ w_texture* pIconTexture,
	_In_ w_texture** pStagingMediaTexture,
	_In_ const char* pFontPath,
	_In_ const float& pFontPixelSize)
{
	if (!_pimp)
	{
		_pimp = new w_imgui_pimp();
	}
	return _pimp->load(
		pGDevice,
		pOutputPresentationWindow,
		pViewport,
		pViewportScissor,
		pIconTexture,
		pStagingMediaTexture,
		pFontPath,
		pFontPixelSize);
}

void w_imgui::new_frame(_In_ const float& pDeltaTime, _In_ const std::function<void(void)>& pMakeGuiWork)
{
    if (!_pimp) return;
    _pimp->new_frame(pDeltaTime, pMakeGuiWork);
}

void w_imgui::render()
{
    if (!_pimp) return;
    _pimp->render();
}

ULONG w_imgui::release()
{
    if (_is_released) return 1;

    SAFE_RELEASE(w_imgui::_pimp);

    return 0;
}

#pragma region Getters

uint32_t w_imgui::get_width()
{
    return _pimp ? _pimp->get_width() : 0;
}

uint32_t w_imgui::get_height()
{
    return _pimp ? _pimp->get_height() : 0;
}

VkCommandBuffer w_imgui::get_command_buffer_at(_In_ const uint32_t pFrameIndex)
{
	return _pimp ? _pimp->get_command_buffer_at(pFrameIndex) : nullptr;
}

#pragma endregion

#pragma region Setters

void w_imgui::set_width(_In_ const uint32_t& pWidth)
{
    if (!_pimp) return;
    _pimp->set_width(pWidth);
}

void w_imgui::set_height(_In_ const uint32_t& pHeight)
{
    if (!_pimp) return;
    _pimp->set_width(pHeight);
}

#pragma endregion

