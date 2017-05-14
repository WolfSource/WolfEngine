#include "w_render_pch.h"
#include "w_mesh.h"
#include "w_buffer.h"
#include "w_pipeline.h"
#include "w_command_buffers.h"
#include "w_uniform.h"
#include <w_vertex_declaration.h>
#include <w_model.h>

using namespace wolf::graphics;

namespace wolf
{
    namespace graphics
    {
        class w_mesh_pimp
        {
        public:
            w_mesh_pimp() :
                _name("w_mesh"),
                _gDevice(nullptr),
                _vertices_count(0),
                _indices_count(0)
            {
            }
            
            HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const void* const pVerticesData,
                _In_ const UINT pVerticesCount,
                _In_ const UINT pVerticesSize,
                _In_ const UINT* const pIndicesData,
                _In_ const UINT pIndicesCount,
                _In_ const w_render_pass* pRenderPass,
                _In_ const std::string& pPipelineCacheName,
                _In_ const w_shader_type pShaderType,
                _In_ const bool pZUp,
                _In_ bool pStaging)
            {
                this->_gDevice = pGDevice;
                this->_staging = pStaging;
                this->_vertices_count = pVerticesCount;
                this->_indices_count = pIndicesCount;

                HRESULT _hr;
                if (this->_staging)
                {
                    _hr = _create_staging_buffers(pVerticesData,
                        pVerticesSize,
                        pIndicesData,
                        static_cast<UINT>(pIndicesCount * sizeof(UINT)),
                        this->_vertex_buffer,
                        this->_index_buffer);
                    if (_hr == S_FALSE)
                    {
                        return S_FALSE;
                    }
                }
                else
                {
                    //vertex buffer is necessary
                    if (pVerticesSize == 0 || pVerticesData == nullptr ||
                        _create_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            pVerticesData,
                            pVerticesSize,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            this->_vertex_buffer) == S_FALSE)
                    {
                        return S_FALSE;
                    }

                    //index buffer is not necessary
                    if (pIndicesCount && pIndicesData != nullptr &&
                        _create_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                            pIndicesData,
                            static_cast<UINT>(pIndicesCount * sizeof(UINT)),
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            this->_index_buffer) == S_FALSE)
                    {
                        return S_FALSE;
                    }
                }

                _hr = _load_shader(pShaderType, pZUp);
                if (_hr == S_FALSE)
                {
                    return S_FALSE;
                }

                _hr = _load_texture();
                if (_hr == S_FALSE)
                {
                    return S_FALSE;
                }

                return _load_pipeline(pShaderType, pRenderPass, pPipelineCacheName);
            }
            
            HRESULT update()
            {
                return this->_wvp_uniform.update();
            }

            void render(_In_ const VkCommandBuffer& pCommandBuffer, _In_ const VkBuffer& pInstanceHandle,
                _In_ uint32_t& pInstancesCount)
            {
                auto _pipeline_handle = this->_pipeline.get_handle();
                auto _pipeline_layout_handle = this->_pipeline.get_layout_handle();

                vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_handle);
                vkCmdBindDescriptorSets(pCommandBuffer,
                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                    _pipeline_layout_handle,
                    0,
                    1,
                    &this->_descriptor_set,
                    0,
                    nullptr);

                VkDeviceSize _offset = 0;

                auto _vertex_buffer_handle = this->_vertex_buffer.get_handle();
                vkCmdBindVertexBuffers(pCommandBuffer, 0, 1, &_vertex_buffer_handle, &_offset);

                if (pInstanceHandle)
                {
                    vkCmdBindVertexBuffers(pCommandBuffer, 1, 1, &pInstanceHandle, &_offset);
                }

                auto _index_buffer_handle = this->_index_buffer.get_handle();
                vkCmdBindIndexBuffer(pCommandBuffer, _index_buffer_handle, 0, VK_INDEX_TYPE_UINT32);

                vkCmdDrawIndexed(pCommandBuffer, this->_indices_count, pInstancesCount + 1, 0, 0, 0);

                _vertex_buffer_handle = nullptr;
                _index_buffer_handle = nullptr;
            }

#pragma region Getters

            VkBuffer get_vertex_buffer_handle() const
            {
                return this->_vertex_buffer.get_handle();
            }
            
            VkBuffer get_index_buffer_handle() const
            {
                return this->_index_buffer.get_handle();
            }
   
            UINT get_vertices_count() const
            {
                return this->_vertices_count;
            }
            
            UINT get_indices_count() const
            {
                return this->_indices_count;
            }
            
            w_shader* get_shader() const
            {
                return this->_shader;
            }

            w_texture* get_texture() const
            {
                return this->_texture;
            }

            glm::mat4 get_world () const
            {
                return this->_wvp_uniform.data.world;
            }

            glm::mat4 get_view_projection() const
            {
                return this->_wvp_uniform.data.view_projection;
            }

#pragma endregion

#pragma region Setters

            void set_shader(_In_ w_shader* pShader)
            {
                this->_shader = pShader;
            }

            void set_texture(_In_ w_texture* pTexture)
            {
                this->_texture = pTexture;
            }

            void set_world(_In_ const glm::mat4 pWorld)
            {
                this->_wvp_uniform.data.world = pWorld;
            }

            void set_view_projection(_In_ const glm::mat4 pViewProjection)
            {
                this->_wvp_uniform.data.view_projection = pViewProjection;
            }

#pragma endregion

            ULONG release()
            {
                //release vertex and index buffers
                
                this->_vertex_buffer.release();
                this->_index_buffer.release();
                this->_pipeline.release();

                this->_shader = nullptr;
                this->_texture = nullptr;
                this->_gDevice = nullptr;
                
                return 1;
            }
        private:

#pragma pack(push,1)
            struct world_view_projection_unifrom
            {
                glm::mat4 view_projection = glm::mat4(1);//Identity matrix
                glm::mat4 world = glm::mat4(1);//Identity matrix
            };
#pragma pack(pop)

            /*
                static data such as index buffer or vertex buffer should be stored on device memory
                for fastest access by GPU.
                So we are giong to do folloing steps:
                    * create buffer in DRAM
                    * copy user data to this buffer
                    * create buffer in VRAM
                    * copy from DRAM to VRAM
                    * delete DRAM buffer
                    * use VRAM buffer for rendering
            */
            HRESULT _create_staging_buffers(_In_ const void* const pVerticesData,
                                            _In_ const UINT pVerticesSize,
                                            _In_ const UINT* const pIndicesData,
                                            _In_ const UINT pIndicesSize,
                                            _Inout_ w_buffer& pVertexBuffer,
                                            _Inout_ w_buffer& pIndexBuffer)
            {
                if (pVerticesSize == 0 || pVerticesData == nullptr)
                {
                    return S_FALSE;
                }
                
                bool _there_is_no_index_buffer = false;
                if (pIndicesSize == 0 || pIndicesData == nullptr)
                {
                    _there_is_no_index_buffer = true;
                }
                
                //create a buffers hosted into the DRAM named staging buffers
                struct
                {
                    w_buffer vertices;
                    w_buffer indices;
                } _stagings_buffers;
                
                if(_create_buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               pVerticesData,
                               pVerticesSize,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                               _stagings_buffers.vertices) == S_FALSE)
                {
                    return S_FALSE;
                }
                
                if(!_there_is_no_index_buffer)
                {
                    if(_create_buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                               pIndicesData,
                               pIndicesSize,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                               _stagings_buffers.indices) == S_FALSE)
                    {
                        return S_FALSE;
                    }
                }
            
                
                // create VRAM buffers
                if(_create_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                  nullptr,
                                  pVerticesSize,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                  pVertexBuffer) == S_FALSE)
                {
                    return S_FALSE;
                }
                
                if(!_there_is_no_index_buffer)
                {
                    if(_create_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      nullptr,
                                      pIndicesSize,
                                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                      pIndexBuffer) == S_FALSE)
                    {
                        return S_FALSE;
                    }
                }
                
                
                //create one command buffer
                auto _copy_command_buffer = new w_command_buffers();
                _copy_command_buffer->load(this->_gDevice, 1);
                
                _copy_command_buffer->begin(0);
                {
                    auto _copy_cmd = _copy_command_buffer->get_command_at(0);
                
                    // Vertex buffer
                    VkBufferCopy _copy_region = {};
                    _copy_region.size = pVerticesSize;
                    vkCmdCopyBuffer(_copy_cmd,
                                    _stagings_buffers.vertices.get_handle(),
                                    pVertexBuffer.get_handle(),
                                    1,
                                    &_copy_region);
                
                    if (!_there_is_no_index_buffer)
                    {
                        // Index buffer
                        _copy_region.size = pIndicesSize;
                        vkCmdCopyBuffer(_copy_cmd,
                                        _stagings_buffers.indices.get_handle(),
                                        pIndexBuffer.get_handle(),
                                        1,
                                        &_copy_region);
                    }
                }
                _copy_command_buffer->flush(0);
                
                SAFE_DELETE(_copy_command_buffer);
                
                _stagings_buffers.vertices.release();
                if (!_there_is_no_index_buffer)
                {
                    _stagings_buffers.indices.release();
                }
                return S_OK;
            }
            
            /*
                Create buffers for rendering.
             */
            HRESULT _create_buffer(_In_ const VkBufferUsageFlags pBufferUsageFlag,
                                   _In_ const void* const pBufferData,
                                   _In_ UINT pBufferSize,
                                   _In_ const VkMemoryPropertyFlags pMemoryFlags,
                                   _Inout_ w_buffer& pBuffer)
            {
                if(pBuffer.load(this->_gDevice, pBufferSize, pBufferUsageFlag, pMemoryFlags))
                {
                    V(S_FALSE, "loading memory of buffer for graphics device: " +
                          _gDevice->device_name + " ID:" + std::to_string(_gDevice->device_id),
                          this->_name, 3, false, true);
                    
                    return S_FALSE;
                }
                
                
                HRESULT _hr;
                //we can not access to VRAM, but we can copy our data to DRAM
                if (!(pMemoryFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
                {
                    _hr = pBuffer.set_data(pBufferData);
                    if(_hr == S_FALSE)
                    {
                        V(S_FALSE, "setting data to vertex buffer's memory staging for graphics device: " +
                          _gDevice->device_name + " ID:" + std::to_string(_gDevice->device_id),
                          this->_name, 3, false, true);
                    
                        return S_FALSE;
                    }
                }
                
                //bind to it
                _hr = pBuffer.bind();
                if(_hr == S_FALSE)
                {
                    V(S_FALSE, "binding to vertex buffer's memory for graphics device: " +
                      _gDevice->device_name + " ID:" + std::to_string(_gDevice->device_id),
                      this->_name, 3, false, true);
                    
                    return S_FALSE;
                }
                
                return S_OK;
            }
            

            HRESULT _load_shader(_In_ w_shader_type pShaderType, _In_ bool pZUp)
            {
                HRESULT _hr;

                std::string _shader_name;
                std::wstring _vertex_shader_path;
                std::wstring _fragment_shader_path;

                switch (pShaderType)
                {
                default:
                case BASIC_SHADER:
                    _shader_name = "static_basic";
                    _vertex_shader_path = content_path + L"shaders/test/basic.vert.spv";
                    _fragment_shader_path = content_path + L"shaders/test/basic.frag.spv";
                    break;
                case BASIC_INSTANCE_SHADER:
                    _shader_name = "static_basic_instance";
                    if (pZUp)
                    {
                        _vertex_shader_path = content_path + L"shaders/test/static_instancing_z_up.vert.spv";
                    }
                    else
                    {
                        _vertex_shader_path = content_path + L"shaders/test/static_instancing_y_up.vert.spv";
                    }
                    _fragment_shader_path = content_path + L"shaders/test/basic.frag.spv";
                    break;
                }

                this->_shader = w_shader::get_shader_from_shared(_shader_name);
                if (!this->_shader)
                {
                    _hr = w_shader::load_to_shared_shaders(this->_gDevice,
                        _shader_name,
                        _vertex_shader_path,
                        _fragment_shader_path,
                        &this->_shader);
                    if (_hr == S_FALSE)
                    {
                        logger.error("Error on loading shader for mesh: " +
                            this->_name);
                        return S_FALSE;
                    }
                }

                //we need one image and one uniform for basic shaders
                const std::vector<VkDescriptorPoolSize> _descriptor_pool_sizes =
                {
                    {
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,          // Type
                        1                                                   // DescriptorCount
                    },
                    {
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                  // Type
                        1                                                   // DescriptorCount
                    }
                };

                _hr = this->_shader->create_descriptor_pool(_descriptor_pool_sizes);
                if (_hr == S_FALSE)
                {
                    logger.error("Error on creating shader descriptor pool for mesh: " +
                        this->_name);
                    return S_FALSE;
                }

                std::vector<VkDescriptorSetLayoutBinding> _layout_bindings =
                {
                    {
                        0,                                                  // Binding
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,          // DescriptorType
                        1,                                                  // DescriptorCount
                        VK_SHADER_STAGE_FRAGMENT_BIT,                       // StageFlags
                        nullptr                                             // ImmutableSamplers
                    },
                    {
                        1,                                                  // Binding
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,                  // DescriptorType
                        1,                                                  // DescriptorCount
                        VK_SHADER_STAGE_VERTEX_BIT,                         // StageFlags
                        nullptr                                             // ImmutableSamplers
                    }
                };
                _hr = this->_shader->create_description_set_layout_binding(_layout_bindings);
                if (_hr == S_FALSE)
                {
                    logger.error("Error on creating shader descriptor pool for mesh: " +
                        this->_name);
                    return S_FALSE;
                }

                //load uniform
                _hr = this->_wvp_uniform.load(this->_gDevice);
                if (_hr == S_FALSE)
                {
                    logger.error("Error on loading world view projection uniform: " +
                        this->_name);
                    return S_FALSE;
                }

                const VkDescriptorImageInfo _image_info = this->_texture == nullptr ?
                    w_texture::default_texture->get_descriptor_info() :
                    this->_texture->get_descriptor_info();

                const VkDescriptorBufferInfo _buffer_info = this->_wvp_uniform.get_descriptor_info();
                this->_descriptor_set = _shader->get_descriptor_set();
                const std::vector<VkWriteDescriptorSet> _write_descriptor_sets =
                {
                    {
                        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // Type
                        nullptr,                                        // Next
                        _descriptor_set,                                // DstSet
                        0,                                              // DstBinding
                        0,                                              // DstArrayElement
                        1,                                              // DescriptorCount
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,      // DescriptorType
                        &_image_info,                                   // ImageInfo
                        nullptr,
                    },
                    {
                        VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,         // Type
                        nullptr,                                        // Next
                        _descriptor_set,                                // DstSet
                        1,                                              // DstBinding
                        0,                                              // DstArrayElement
                        1,                                              // DescriptorCount
                        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,              // DescriptorType
                        nullptr,                                        // ImageInfo
                        &_buffer_info,                                  // BufferInfo
                        nullptr                                         // TexelBufferView
                    }
                };

                this->_shader->update_descriptor_sets(_write_descriptor_sets);

                return _hr;
            }

            HRESULT _load_texture()
            {
               // this->_texture = w_texture::default_texture;
                return S_OK;
            }

            HRESULT _load_pipeline(_In_ w_shader_type pShaderType, _In_ const w_render_pass* pRenderPass,
                _In_ const std::string& pPipelineCacheName)
            {
                HRESULT _hr = S_OK;

                std::vector<VkVertexInputBindingDescription> _vertex_binding_descriptions;
                std::vector<VkVertexInputAttributeDescription> _vertex_attribute_descriptions;

                switch (pShaderType)
                {
                default:
                case BASIC_SHADER:
                {
                    //create pipeline for basic shader
                    _vertex_binding_descriptions.push_back(
                    {
                        0,                                                                                        // Binding
                        sizeof(wolf::content_pipeline::vertex_declaration_structs::vertex_position_uv),           // Stride
                        VK_VERTEX_INPUT_RATE_VERTEX                                                               // InputRate
                    });
                    _vertex_attribute_descriptions.push_back(
                    {
                        0,                                                                                        // Location
                        _vertex_binding_descriptions[0].binding,                                                  // Binding
                        VK_FORMAT_R32G32B32A32_SFLOAT,                                                            // Format
                        offsetof(wolf::content_pipeline::vertex_declaration_structs::vertex_position_uv, position)// Offset
                    });
                    _vertex_attribute_descriptions.push_back(
                    { 
                        1,                                                                                        // Location
                        _vertex_binding_descriptions[0].binding,                                                  // Binding
                        VK_FORMAT_R32G32_SFLOAT,                                                                  // Format
                        offsetof(wolf::content_pipeline::vertex_declaration_structs::vertex_position_uv, uv)      // Offset
                    });
                }
                break;
                case BASIC_INSTANCE_SHADER:
                {
                    //create pipeline for instance shader
                    _vertex_binding_descriptions.push_back(
                    {
                        0,                                                                                        // Binding
                        sizeof(wolf::content_pipeline::vertex_declaration_structs::vertex_position_uv),           // Stride
                        VK_VERTEX_INPUT_RATE_VERTEX                                                               // InputRate
                    });
                    _vertex_binding_descriptions.push_back(
                    {
                        1,                                                                                        // Binding
                        sizeof(wolf::content_pipeline::w_model::w_instance_info),                                 // Stride
                        VK_VERTEX_INPUT_RATE_INSTANCE                                                             // InputRate
                    });

                    _vertex_attribute_descriptions.push_back(
                    {
                        0,                                                                                        // Location
                        _vertex_binding_descriptions[0].binding,                                                  // Binding
                        VK_FORMAT_R32G32B32_SFLOAT,                                                               // Format
                        0                                                                                         // Offset
                    });
                    _vertex_attribute_descriptions.push_back(
                    {
                        1,                                                                                        // Location
                        _vertex_binding_descriptions[0].binding,                                                  // Binding
                        VK_FORMAT_R32G32_SFLOAT,                                                                  // Format
                        sizeof(float) * 3                                                                         // Offset
                    });
                    
                    // Per instance attributes
                    _vertex_attribute_descriptions.push_back(
                    {
                        2,                                                                                        // Location
                        _vertex_binding_descriptions[1].binding,                                                  // Binding
                        VK_FORMAT_R32G32B32_SFLOAT,                                                               // Format
                        0                                                                                         // Offset
                    });                                                                                           
                    _vertex_attribute_descriptions.push_back(                                                     
                    {                                                                                             
                        3,                                                                                        // Location
                        _vertex_binding_descriptions[1].binding,                                                  // Binding
                        VK_FORMAT_R32G32B32_SFLOAT,                                                               // Format
                        sizeof(float) * 3                                                                         // Offset
                    });
                    _vertex_attribute_descriptions.push_back(
                    {
                        4,                                                                                        // Location
                        _vertex_binding_descriptions[1].binding,                                                  // Binding
                        VK_FORMAT_R32_SFLOAT,                                                                     // Format
                        sizeof(float) * 6                                                                         // Offset
                    });                                                                                           
                    _vertex_attribute_descriptions.push_back(                                                     
                    {                                                                                             
                        5,                                                                                        // Location
                        _vertex_binding_descriptions[1].binding,                                                  // Binding
                        VK_FORMAT_R32_SINT,                                                                       // Format
                        sizeof(float) * 7                                                                         // Offset
                    });
                }
                break;
                }

                VkPipelineVertexInputStateCreateInfo _vertex_input_state_create_info =
                {
                    VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,    // Type
                    nullptr,                                                      // Next
                    0,                                                            // Flags
                    static_cast<uint32_t>(_vertex_binding_descriptions.size()),   // VertexBindingDescriptionCount
                    &_vertex_binding_descriptions[0],                             // VertexBindingDescriptions
                    static_cast<uint32_t>(_vertex_attribute_descriptions.size()), // VertexAttributeDescriptionCount
                    &_vertex_attribute_descriptions[0]                            // VertexAttributeDescriptions
                };

                VkPipelineInputAssemblyStateCreateInfo _input_assembly_state_create_info =
                {
                    VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,    // Type
                    nullptr,                                                        // Next
                    0,                                                              // Flags
                    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                            // Topology
                    VK_FALSE                                                        // Enable restart primitive
                };

                std::vector<VkDynamicState> _dynamic_states =
                {
                    VK_DYNAMIC_STATE_VIEWPORT,
                    VK_DYNAMIC_STATE_SCISSOR,
                };

                VkPipelineDynamicStateCreateInfo _dynamic_state_create_info =
                {
                    VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,                   // Type
                    nullptr,                                                                // Next
                    0,                                                                      // Flags
                    static_cast<uint32_t>(_dynamic_states.size()),                          // DynamicStateCount
                    &_dynamic_states[0]                                                     // DynamicStates
                };

                auto _descriptor_set_layout = this->_shader->get_descriptor_set_layout_binding();
                VkPipelineLayoutCreateInfo _pipeline_layout_create_info =
                {
                    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  // Type
                    nullptr,                                        // Next
                    0,                                              // Flags
                    1,                                              // SetLayoutCount
                    &_descriptor_set_layout,                        // SetLayouts
                    0,                                              // PushConstantRangeCount
                    nullptr                                         // PushConstantRanges
                };

                _hr = this->_pipeline.load(_gDevice,
                    pPipelineCacheName,
                    pRenderPass->get_handle(),
                    this->_shader->get_shader_stages(),
                    { pRenderPass->get_viewport() }, //viewports
                    { pRenderPass->get_viewport_scissor() }, //viewports scissor
                    &_pipeline_layout_create_info,
                    &_vertex_input_state_create_info,
                    &_input_assembly_state_create_info,
                    nullptr,
                    nullptr,
                    nullptr,
                    &_dynamic_state_create_info);
                if (_hr)
                {
                    logger.error("Error creating pipeline for mesh");
                }

                return _hr;
            }

            std::string                                         _name;
            std::shared_ptr<w_graphics_device>                  _gDevice;
            w_buffer                                            _vertex_buffer;
            w_buffer                                            _index_buffer;
            UINT                                                _indices_count;
            UINT                                                _vertices_count;
            bool                                                _staging;
            w_pipeline                                          _pipeline;
            w_texture*                                          _texture;
            w_shader*                                           _shader;
            w_uniform<world_view_projection_unifrom>            _wvp_uniform;
            VkDescriptorSet                                     _descriptor_set;
        };
    }
}

w_mesh::w_mesh() : _pimp(new w_mesh_pimp())
{
	_super::set_class_name("w_mesh");
}

w_mesh::~w_mesh()
{
	release();
}

HRESULT w_mesh::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                     _In_ const void* const pVerticesData,
                     _In_ const UINT pVerticesCount,
                     _In_ const UINT pVerticesSize,
                     _In_ const UINT* const pIndicesData,
                     _In_ const UINT pIndicesCount,
                     _In_ const w_render_pass* pRenderPass,
                     _In_ const std::string& pPipelineCacheName,
                     _In_ const w_shader_type pShaderType,
                     _In_ const bool pZUp,
                     _In_ const bool pStaging)
{
    if (!this->_pimp) return S_FALSE;
    
    return this->_pimp->load(pGDevice,
                             pVerticesData,
                             pVerticesCount,
                             pVerticesSize,
                             pIndicesData,
                             pIndicesCount,
                             pRenderPass,
                             pPipelineCacheName,
                             pShaderType,
                             pZUp,
                             pStaging);
}

HRESULT w_mesh::update()
{
    return this->_pimp ? this->_pimp->update() : S_FALSE;
}

void w_mesh::render(_In_ const VkCommandBuffer& pCommandBuffer,
    _In_ const VkBuffer& pInstanceHandle,
    _In_ uint32_t& pInstancesCount)
{
    if (this->_pimp)
    {
        this->_pimp->render(pCommandBuffer, pInstanceHandle, pInstancesCount);
    }
}

ULONG w_mesh::release()
{
    if (this->get_is_released()) return 0;
    
    SAFE_RELEASE(this->_pimp);
   
    return _super::release();
}

#pragma region Getters

VkBuffer w_mesh::get_vertex_buffer_handle() const
{
    return this->_pimp ? this->_pimp->get_vertex_buffer_handle() : VK_NULL_HANDLE;
}

VkBuffer w_mesh::get_index_buffer_handle() const
{
    return this->_pimp ? this->_pimp->get_index_buffer_handle() : VK_NULL_HANDLE;
}

UINT w_mesh::get_vertices_count() const
{
    return this->_pimp ? this->_pimp->get_vertices_count() : 0;
}

UINT w_mesh::get_indices_count() const
{
    return this->_pimp ? this->_pimp->get_indices_count() : 0;
}

w_shader* w_mesh::get_shader() const
{
    return this->_pimp ? this->_pimp->get_shader() : nullptr;
}

w_texture* w_mesh::get_texture() const
{
    return this->_pimp ? this->_pimp->get_texture() : nullptr;
}

const glm::mat4 w_mesh::get_world() const
{
    return this->_pimp ? this->_pimp->get_world() : glm::mat4(1);
}

const glm::mat4 w_mesh::get_view_projection() const
{
    return this->_pimp ? this->_pimp->get_view_projection() : glm::mat4(1);
}

#pragma endregion

#pragma region Setters

void w_mesh::set_shader(_In_ w_shader* pShader)
{
    this->_pimp->set_shader(pShader);
}

void w_mesh::set_texture(_In_ w_texture* pTexture)
{
    this->_pimp->set_texture(pTexture);
}

void w_mesh::set_world(_In_ const glm::mat4 pWorld)
{
    this->_pimp->set_world(pWorld);
}

void w_mesh::set_view_projection(_In_ const glm::mat4 pViewProjection)
{
    this->_pimp->set_view_projection(pViewProjection);
}

#pragma endregion
