#include "w_render_pch.h"
#include "w_pipeline.h"
//#include <w_cpipeline_model.h>

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
                _pipeline_layout(nullptr),
				_shader_descriptor_set(nullptr),
				_compute_shader_descriptor_set(nullptr)
            {
            }

            W_RESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const w_vertex_binding_attributes& pVertexBindingAttributes,
                _In_ const w_primitive_topology pPrimitiveTopology,
				_In_ const w_render_pass* pRenderPassBinding,
				_In_ const w_shader* pShaderBinding,
				_In_ const std::vector<w_viewport>& pViewPorts,
				_In_ const std::vector<w_viewport_scissor>& pViewPortScissors,
                _In_ const std::string& pPipelineCacheName,
				_In_ const std::vector<w_dynamic_state>& pDynamicStates,
                _In_ const std::vector<w_push_constant_range>& pPushConstantRanges,
                _In_ const uint32_t& pTessellationPatchControlPoints,
                _In_ const w_pipeline_rasterization_state_create_info& pPipelineRasterizationStateCreateInfo,
                _In_ const w_pipeline_multisample_state_create_info& pPipelineMultiSampleStateCreateInfo,
                _In_ const w_pipeline_color_blend_attachment_state& pBlendState,
                _In_ const w_color& pBlendColors)
            {
				const char* _trace_info = (this->_name + "::load").c_str();

                this->_gDevice = pGDevice;

                if (pVertexBindingAttributes.declaration == w_vertex_declaration::NOT_DEFINED)
                {
					V(W_FAILED, 
						w_log_type::W_WARNING,
						"vertex type not defined for graphics device: {}. trace info: {}",
						this->_gDevice->get_info(),
						_trace_info);
                }
				if (!pShaderBinding)
				{
					V(W_FAILED,
						w_log_type::W_WARNING,
						"shader can not be nullptr. graphics device: {}. trace info: {}",
						this->_gDevice->get_info(),
						_trace_info);
					return W_FAILED;
				}
				if (!pShaderBinding->get_shader_stages())
				{
					V(W_FAILED,
						w_log_type::W_ERROR,
						"shader stages can not be nullptr. graphics device: {}. trace info: {}",
						this->_gDevice->get_info(),
						_trace_info);
					return W_FAILED;
				}
				if (!pRenderPassBinding)
				{
					V(W_FAILED,
						w_log_type::W_ERROR,
						"render pass can not be nullptr. graphics device: {}. trace info: {}",
						this->_gDevice->get_info(),
						_trace_info);
					return W_FAILED;
				}
				
                VkPipelineVertexInputStateCreateInfo* _vertex_input_state_create_info = nullptr;
                VkPipelineInputAssemblyStateCreateInfo* _input_assembly_state_create_info = nullptr;
                VkPipelineDynamicStateCreateInfo* _pipeline_dynamic_state_create_info = nullptr;

				auto _shader_des_set = pShaderBinding->get_descriptor_set().handle;
				this->_shader_descriptor_set = _shader_des_set  ? _shader_des_set : nullptr;
				
				const auto _shader_descriptor_set_layout = pShaderBinding->get_descriptor_set_layout().handle;
                auto _pipeline_layout_create_info = _generate_pipeline_layout_create_info(
                    pVertexBindingAttributes,
                    pPrimitiveTopology,
					_shader_descriptor_set_layout ? &_shader_descriptor_set_layout : nullptr,
                    pDynamicStates,
                    pPushConstantRanges,
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
                    static_cast<uint32_t>(pViewPortScissors.size()),                        // ScissorCount
					pViewPortScissors.data()  // Scissors
                };

                //create blend state
                VkPipelineColorBlendStateCreateInfo _color_blend_state_create_info = {};
                _color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                _color_blend_state_create_info.attachmentCount = 1;
                _color_blend_state_create_info.pAttachments = &pBlendState;
                _color_blend_state_create_info.blendConstants[0] = pBlendColors.r / 255.f;
                _color_blend_state_create_info.blendConstants[1] = pBlendColors.g / 255.f;
                _color_blend_state_create_info.blendConstants[2] = pBlendColors.b / 255.f;
                _color_blend_state_create_info.blendConstants[3] = pBlendColors.a / 255.f;

				auto _depth_stencil_enabled = pRenderPassBinding->get_depth_stencil_enabled();
                VkPipelineDepthStencilStateCreateInfo _depth_stencil_state = {};
                if (_depth_stencil_enabled)
                {
                    _depth_stencil_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                    _depth_stencil_state.depthTestEnable = VK_TRUE;
                    _depth_stencil_state.depthWriteEnable = VK_TRUE;
                    _depth_stencil_state.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
                    _depth_stencil_state.minDepthBounds = 0.0f;
                    _depth_stencil_state.maxDepthBounds = 1.0f;
                    _depth_stencil_state.stencilTestEnable = VK_FALSE;
                    _depth_stencil_state.depthBoundsTestEnable = VK_FALSE;
                    _depth_stencil_state.front = _depth_stencil_state.back;
                    _depth_stencil_state.back.compareOp = VK_COMPARE_OP_ALWAYS;
                    _depth_stencil_state.back.failOp = VK_STENCIL_OP_KEEP;
                    _depth_stencil_state.back.passOp = VK_STENCIL_OP_KEEP;
                }

				const auto _shader_stages = pShaderBinding->get_shader_stages();
                VkGraphicsPipelineCreateInfo _pipeline_create_info;
                _pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                _pipeline_create_info.pNext = nullptr;
                _pipeline_create_info.flags = 0;
				_pipeline_create_info.stageCount = static_cast<uint32_t>(_shader_stages->size());
				_pipeline_create_info.pStages = _shader_stages->data();
				
                _pipeline_create_info.pVertexInputState =
                    _vertex_input_state_create_info == nullptr ?
                    &(w_graphics_device::defaults_states::pipelines::vertex_input_create_info) :
                    _vertex_input_state_create_info;

                _pipeline_create_info.pInputAssemblyState =
                    _input_assembly_state_create_info == nullptr ?
                    &(w_graphics_device::defaults_states::pipelines::input_assembly_create_info) :
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

				auto _render_pass_handle = pRenderPassBinding->get_handle();

                _pipeline_create_info.pRasterizationState = &pPipelineRasterizationStateCreateInfo;
				_pipeline_create_info.pMultisampleState = &pPipelineMultiSampleStateCreateInfo;

                _pipeline_create_info.pDepthStencilState = _depth_stencil_enabled ? &_depth_stencil_state : nullptr;
                _pipeline_create_info.pColorBlendState = &_color_blend_state_create_info;
                _pipeline_create_info.pDynamicState = _pipeline_dynamic_state_create_info == nullptr ? nullptr : _pipeline_dynamic_state_create_info;
                _pipeline_create_info.pViewportState = &_viewport_state_create_info;
                _pipeline_create_info.layout = this->_pipeline_layout;
                _pipeline_create_info.renderPass = _render_pass_handle.handle;
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
					V(W_FAILED,
						w_log_type::W_ERROR,
						"creating pipeline for graphics device: {}. trace info: {}",
						this->_gDevice->get_info(),
						_trace_info);
                    return W_FAILED;
                }

                return W_PASSED;
            }

			W_RESULT load_compute(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const w_shader* pShaderBinding,
				_In_ const uint32_t& pSpecializationData,
				_In_ const std::string& pPipelineCacheName,
				_In_ const std::vector<w_push_constant_range> pPushConstantRanges)
			{
				const char* _trace_info = (this->_name + "::load_compute").c_str();

				this->_gDevice = pGDevice;

				auto _pipeline_cache = w_pipeline::get_pipeline_cache(pPipelineCacheName);

				//store compute shader descriptor set
				auto _shader_des_set = pShaderBinding->get_compute_descriptor_set().handle;
				this->_compute_shader_descriptor_set = _shader_des_set ? _shader_des_set : nullptr;

				std::vector<VkDescriptorSetLayout> _descriptor_set_layouts;
				auto _shader_descriptor_set_layout = pShaderBinding->get_compute_descriptor_set_layout().handle;
				if (_shader_descriptor_set_layout)
				{
					_descriptor_set_layouts.push_back(_shader_descriptor_set_layout);
				}
				auto _push_const_size = static_cast<uint32_t>(pPushConstantRanges.size());

				VkPipelineLayoutCreateInfo _pipeline_layout_create_info = {};
				_pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				_pipeline_layout_create_info.setLayoutCount = static_cast<uint32_t>(_descriptor_set_layouts.size());
				_pipeline_layout_create_info.pSetLayouts = _descriptor_set_layouts.size() ? _descriptor_set_layouts.data() : nullptr;
				_pipeline_layout_create_info.pushConstantRangeCount = _push_const_size;
				_pipeline_layout_create_info.pPushConstantRanges = _push_const_size ? (VkPushConstantRange*)pPushConstantRanges.data() : nullptr;

				auto _hr = vkCreatePipelineLayout(
					pGDevice->vk_device,
					&_pipeline_layout_create_info,
					nullptr,
					&this->_pipeline_layout);

				if (_hr)
				{
					V(W_FAILED,
						w_log_type::W_ERROR,
						"creating compute pipeline layout for graphics device: {}. trace info: {}",
						this->_gDevice->get_info(),
						_trace_info);
					return W_FAILED;
				}

				// Create pipeline		
				VkComputePipelineCreateInfo _compute_pipeline_create_info = {};
				_compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
				_compute_pipeline_create_info.layout = this->_pipeline_layout;
				_compute_pipeline_create_info.flags = 0;
				_compute_pipeline_create_info.stage = pShaderBinding->get_compute_shader_stage();

				// Use specialization constants to pass max. level of detail (determined by no. of meshes)
				VkSpecializationMapEntry specializationEntry{};
				specializationEntry.constantID = 0;
				specializationEntry.offset = 0;
				specializationEntry.size = sizeof(uint32_t);

				uint32_t _specialization_data = pSpecializationData;

				VkSpecializationInfo _specialization_info;
				_specialization_info.mapEntryCount = 1;
				_specialization_info.pMapEntries = &specializationEntry;
				_specialization_info.dataSize = sizeof(_specialization_data);
				_specialization_info.pData = &_specialization_data;

				_compute_pipeline_create_info.stage.pSpecializationInfo = &_specialization_info;

				_hr = vkCreateComputePipelines(
					pGDevice->vk_device,
					_pipeline_cache,
					1,
					&_compute_pipeline_create_info,
					nullptr,
					&this->_pipeline);

				if (_hr)
				{
					V(W_FAILED,
						w_log_type::W_ERROR,
						"creating compute pipeline for graphics device: {}. trace info: {}",
						this->_gDevice->get_info(),
						_trace_info);
					return W_FAILED;
				}

				return W_PASSED;
			}

			void bind(_In_ const w_command_buffer& pCommandBuffer, 
				_In_ const w_pipeline_bind_point& pPipelineBindPoint)
			{
				auto _cmd = pCommandBuffer.handle;
				auto _bind_point = (VkPipelineBindPoint)pPipelineBindPoint;

				if (_bind_point == VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS)
				{
					//set descriptor set of shader
					if (this->_shader_descriptor_set)
					{
						vkCmdBindDescriptorSets(_cmd,
							_bind_point,
							this->_pipeline_layout,
							0,
							1,
							&this->_shader_descriptor_set,
							0,
							nullptr);
					}
				}
				else
				{
					//set descriptor set of compute shader
					if (this->_compute_shader_descriptor_set)
					{
						vkCmdBindDescriptorSets(_cmd,
							_bind_point,
							this->_pipeline_layout,
							0,
							1,
							&this->_compute_shader_descriptor_set,
							0,
							nullptr);
					}
				}
				vkCmdBindPipeline(_cmd, _bind_point, this->_pipeline);
				_cmd = nullptr;
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

				this->_shader_descriptor_set = nullptr;
                this->_gDevice = nullptr;
				_name.clear();

                return 0;
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

			void set_push_constant_buffer(
				_In_ const w_command_buffer&			pCommandBuffer,
				_In_ const w_shader_stage_flag_bits&	pStageFlags,
				_In_ const uint32_t&					pOffset,
				_In_ const uint32_t&					pSize,
				_In_ const void*						pValues)
			{
				vkCmdPushConstants(
					pCommandBuffer.handle,
					this->_pipeline_layout,
					(VkShaderStageFlags)pStageFlags,
					0,
					pSize,
					pValues);
			}

#pragma endregion

            static std::map<std::string, VkPipelineCache> pipeline_caches;

        private:

            const VkPipelineLayoutCreateInfo _generate_pipeline_layout_create_info(
                _In_ const w_vertex_binding_attributes& pVertexBindingAttributes,
                _In_ const w_primitive_topology pPrimitiveTopology,
                _In_ const VkDescriptorSetLayout* const pDescriptorSetLayoutBinding,
                _In_ const std::vector<w_dynamic_state>& pDynamicStates,
                _In_ const std::vector<w_push_constant_range>& pPushConstantRanges,
                _Out_ VkPipelineVertexInputStateCreateInfo** pVertexInputStateCreateInfo,
                _Out_ VkPipelineInputAssemblyStateCreateInfo** pInputAssemblyStateCreateInfo,
                _Out_ VkPipelineDynamicStateCreateInfo** pDynamicStateCreateInfo)
            {
                using namespace wolf::content_pipeline;

                auto _vertex_binding_descriptions = new std::vector<VkVertexInputBindingDescription>();
                auto _vertex_attribute_descriptions = new std::vector<VkVertexInputAttributeDescription>();

                uint32_t _location_index = 0;
                for (auto& _binding : pVertexBindingAttributes.binding_attributes)
                {
                    uint32_t _stride = 0;
                    for (auto& _iter : _binding.second)
                    {
						switch (_iter)
						{
						case w_vertex_attribute::W_FLOAT:
						case w_vertex_attribute::W_TEXTURE_INDEX:
						case w_vertex_attribute::W_SCALE:
							_stride += 4;//floats
							break;
						case w_vertex_attribute::W_VEC2:
						case w_vertex_attribute::W_UV:
							_stride += 8;//floats
							break;
						case w_vertex_attribute::W_VEC3:
						case w_vertex_attribute::W_POS:
						case w_vertex_attribute::W_ROT:
						case w_vertex_attribute::W_NORM:
						case w_vertex_attribute::W_TANGENT:
						case w_vertex_attribute::W_BINORMAL:
							_stride += 12;//floats
							break;
						case w_vertex_attribute::W_VEC4:
						case w_vertex_attribute::W_COLOR:
						case w_vertex_attribute::W_BLEND_WEIGHT:
						case w_vertex_attribute::W_BLEND_INDICES:
							_stride += 16;//floats
							break;
						}
                    }

                    _vertex_binding_descriptions->push_back(
                    {
                            _binding.first,                                                     // Binding
                            _stride,                                                            // Stride
                            (VkVertexInputRate)_binding.first                                   // InputRate => 0 = VK_VERTEX_INPUT_RATE_VERTEX , 1 = VK_VERTEX_INPUT_RATE_INSTANCE                         
                    });

                    uint32_t _offset = 0;
                    for (auto& _attr : _binding.second)
                    {
                        switch (_attr)
                        {
                        case w_vertex_attribute::W_FLOAT:
						case w_vertex_attribute::W_TEXTURE_INDEX:
						case w_vertex_attribute::W_SCALE:
                            _vertex_attribute_descriptions->push_back(
                            {
                                _location_index,                                               // Location
                                _vertex_binding_descriptions->at(_binding.first).binding,      // Binding
                                VK_FORMAT_R32_SFLOAT,                                          // Format
                                _offset                                                        // Offset
                            });
                            _offset += 4;//floats
                            break;
                        case w_vertex_attribute::W_VEC2:
						case w_vertex_attribute::W_UV:
                            _vertex_attribute_descriptions->push_back(
                            {
                                _location_index,                                               // Location
                                _vertex_binding_descriptions->at(_binding.first).binding,      // Binding
                                VK_FORMAT_R32G32_SFLOAT,                                       // Format
                                _offset                                                        // Offset
                            });
                            _offset += 8;//floats
                            break;
                        case w_vertex_attribute::W_VEC3:
						case w_vertex_attribute::W_POS:
						case w_vertex_attribute::W_ROT:
						case w_vertex_attribute::W_NORM:
						case w_vertex_attribute::W_TANGENT:
						case w_vertex_attribute::W_BINORMAL:
                            _vertex_attribute_descriptions->push_back(
                            {
                                _location_index,                                               // Location
                                _vertex_binding_descriptions->at(_binding.first).binding,      // Binding
                                VK_FORMAT_R32G32B32_SFLOAT,                                    // Format
                                _offset                                                        // Offset
                            });
                            _offset += 12;//floats
                            break;
                        case w_vertex_attribute::W_VEC4:
						case w_vertex_attribute::W_COLOR:
						case w_vertex_attribute::W_BLEND_WEIGHT:
						case w_vertex_attribute::W_BLEND_INDICES:
                            _vertex_attribute_descriptions->push_back(
                            {
                                _location_index,                                               // Location
                                _vertex_binding_descriptions->at(_binding.first).binding,      // Binding
                                VK_FORMAT_R32G32B32A32_SFLOAT,                                 // Format
                                _offset                                                        // Offset
                            });
                            _offset += 16;//floats
                            break;
                        }

                        _location_index++;
                    }
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
                _vas_ptr->topology = (VkPrimitiveTopology)pPrimitiveTopology;
                _vas_ptr->primitiveRestartEnable = VK_FALSE;
                *pInputAssemblyStateCreateInfo = _vas_ptr;

                auto _dys_ptr = new VkPipelineDynamicStateCreateInfo();
                _dys_ptr->sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                _dys_ptr->pNext = nullptr;
                _dys_ptr->flags = 0;

				auto _dynamic_state_count = static_cast<uint32_t>(pDynamicStates.size());
				if (_dynamic_state_count)
				{
					_dys_ptr->dynamicStateCount = _dynamic_state_count;
					_dys_ptr->pDynamicStates = (VkDynamicState*)pDynamicStates.data();
				}
				else
				{
					_dys_ptr->dynamicStateCount = 0;
					_dys_ptr->pDynamicStates = nullptr;
				}
				*pDynamicStateCreateInfo = _dys_ptr;

                uint32_t  _push_constant_range_count = pPushConstantRanges.size();
                VkPipelineLayoutCreateInfo _pipeline_layout_create_info =
                {
                    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,                                      // Type
                    nullptr,                                                                            // Next
                    0,                                                                                  // Flags
                    static_cast<uint32_t>(pDescriptorSetLayoutBinding == nullptr ? 0 : 1),              // SetLayoutCount
                    pDescriptorSetLayoutBinding == nullptr ? nullptr : pDescriptorSetLayoutBinding,     // SetLayouts
                    _push_constant_range_count,                                                         // PushConstantRangeCount
                    _push_constant_range_count ? pPushConstantRanges.data() : nullptr                   // PushConstantRanges
                };

                return _pipeline_layout_create_info;
            }


            std::string                                     _name;
            std::shared_ptr<w_graphics_device>              _gDevice;
            VkPipeline                                      _pipeline;
            VkPipelineLayout                                _pipeline_layout;
			VkDescriptorSet									_shader_descriptor_set;
			VkDescriptorSet									_compute_shader_descriptor_set;
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

W_RESULT w_pipeline::load(
	_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const w_vertex_binding_attributes& pVertexBindingAttributes,
	_In_ const w_primitive_topology pPrimitiveTopology,
	_In_ const w_render_pass* pRenderPassBinding,
	_In_ const w_shader* pShaderBinding,
	_In_ const std::vector<w_viewport>& pViewPorts,
	_In_ const std::vector<w_viewport_scissor>& pViewPortScissors,
	_In_ const std::string& pPipelineCacheName,
	_In_ const std::vector<w_dynamic_state>& pDynamicStates,
	_In_ const std::vector<w_push_constant_range>& pPushConstantRanges,
	_In_ const uint32_t& pTessellationPatchControlPoints,
	_In_ const w_pipeline_rasterization_state_create_info& pPipelineRasterizationStateCreateInfo,
	_In_ const w_pipeline_multisample_state_create_info& pPipelineMultiSampleStateCreateInfo,
	_In_ const w_pipeline_color_blend_attachment_state& pBlendState,
	_In_ const w_color& pBlendColors)
{
	if (!this->_pimp) return W_FAILED;

	return this->_pimp->load(
		pGDevice,
		pVertexBindingAttributes,
		pPrimitiveTopology,
		pRenderPassBinding,
		pShaderBinding,
		pViewPorts,
		pViewPortScissors,
		pPipelineCacheName,
		pDynamicStates,
		pPushConstantRanges,
		pTessellationPatchControlPoints,
		pPipelineRasterizationStateCreateInfo,
		pPipelineMultiSampleStateCreateInfo,
		pBlendState,
		pBlendColors);
}

//load pipeline for compute stage
W_EXP W_RESULT w_pipeline::load_compute(
	_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const w_shader* pShaderBinding,
	_In_ const uint32_t& pSpecializationData,
	_In_ const std::string& pPipelineCacheName,
	_In_ const std::vector<w_push_constant_range> pPushConstantRanges)
{
	if (!this->_pimp) return W_FAILED;

	return this->_pimp->load_compute(
		pGDevice,
		pShaderBinding,
		pSpecializationData,
		pPipelineCacheName,
		pPushConstantRanges);
}

W_RESULT w_pipeline::bind(_In_ const w_command_buffer& pCommandBuffer, 
	_In_ const w_pipeline_bind_point& pPipelineBindPoint)
{
    if (!this->_pimp) return W_FAILED;
    this->_pimp->bind(pCommandBuffer, pPipelineBindPoint);
	return W_PASSED;
}

ULONG w_pipeline::release()
{
	if (_super::get_is_released()) return 1;
    
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

void w_pipeline::set_push_constant_buffer(
	_In_ const w_command_buffer&			pCommandBuffer,
	_In_ const w_shader_stage_flag_bits&	pStageFlags,
	_In_ const uint32_t&					pOffset,
	_In_ const uint32_t&					pSize,
	_In_ const void*						pValues)
{
	if (!this->_pimp) return;
	return this->_pimp->set_push_constant_buffer(pCommandBuffer, pStageFlags, pOffset, pSize, pValues);
}

#pragma endregion

VkPipelineLayout w_pipeline::create_pipeline_layout(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const VkPipelineLayoutCreateInfo* const pPipelineLayoutCreateInfo)
{
	//create pipeline layout
	VkPipelineLayout _pipeline_layout = 0;
	auto _hr = vkCreatePipelineLayout(pGDevice->vk_device,
		pPipelineLayoutCreateInfo == nullptr ?
		&(w_graphics_device::defaults_states::pipelines::layout_create_info) : pPipelineLayoutCreateInfo,
		nullptr,
		&_pipeline_layout);
	if (_hr)
	{
		V(W_FAILED, 
			w_log_type::W_ERROR,
			"creating pipeline layout for graphics device: {}. trace info: {}",
			pGDevice->get_info(), 
			"w_pipeline::create_pipeline_layout");
		return 0;
	}
	return _pipeline_layout;
}

W_RESULT w_pipeline::create_pipeline_cache(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_z_ const std::string& pPipelineCacheName)
{
	//pipeline cache already exists
	auto _old_pipline_cache = get_pipeline_cache(pPipelineCacheName);
	if (_old_pipline_cache) return W_PASSED;
	

    VkPipelineCache _pipeline_cache;
    VkPipelineCacheCreateInfo _pipeline_cache_create_info = {};
    _pipeline_cache_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    
    auto _hr = vkCreatePipelineCache(pGDevice->vk_device, &_pipeline_cache_create_info, nullptr, &_pipeline_cache);
	if (_hr)
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			"error on creating pipeline cache with graphics device: {}. trace info: {}",
			pGDevice->get_info(),
			"w_pipeline::create_pipeline_cache");

		return W_FAILED;
	}

    w_pipeline_pimp::pipeline_caches[pPipelineCacheName] = _pipeline_cache;
    
    return W_PASSED;
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
