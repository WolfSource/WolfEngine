#include "w_render_pch.h"
#include "w_mesh.h"
#include "w_buffer.h"
#include "w_pipeline.h"
#include "w_command_buffers.h"
#include "w_uniform.h"
#include <w_vertex_declaration.h>
#include <w_cpipeline_model.h>

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
                _shader(nullptr),
                _copy_command_buffer(nullptr),
                _vertices_count(0),
                _indices_count(0),
                _vertex_declaration(w_mesh::w_vertex_declaration::VERTEX_POSITION_UV)
            {
        
            }
            
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
            HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const void* const pVerticesData,
                _In_ const UINT  pVerticesSize,
                _In_ const UINT pVerticesCount,
                _In_ const UINT* const pIndicesData,
                _In_ const UINT pIndicesCount,
                _In_ w_shader* pShader,
                _In_ const w_render_pass* pRenderPass,
                _In_ const std::string& pPipelineCacheName,
                _In_ const bool pZUp,
                _In_ const bool pUseDynamicBuffer)
            {
                this->_gDevice = pGDevice;
                this->_vertices_count = pVerticesCount;
                this->_indices_count = pIndicesCount;
                this->_shader = pShader;
                this->_dynamic_buffer = pUseDynamicBuffer;

                if (pVerticesCount == 0 || pVerticesData == nullptr)
                {
                    return S_FALSE;
                }

                if (this->_shader == nullptr)
                {
                    return S_FALSE;
                }

                bool _there_is_no_index_buffer = false;
                UINT _indices_size = pIndicesCount * sizeof(UINT);
                if (pIndicesCount == 0 || pIndicesData == nullptr)
                {
                    _there_is_no_index_buffer = true;
                }

                //create a buffers hosted into the DRAM named staging buffers
                if (_create_buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                    pVerticesData,
                    pVerticesSize,
                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                    this->_stagings_buffers.vertices) == S_FALSE)
                {
                    return S_FALSE;
                }

                if (!_there_is_no_index_buffer)
                {
                    if (_create_buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        pIndicesData,
                        _indices_size,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        this->_stagings_buffers.indices) == S_FALSE)
                    {
                        return S_FALSE;
                    }
                }

                // create VRAM buffers
                if (_create_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                    nullptr,
                    pVerticesSize,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    this->_vertex_buffer) == S_FALSE)
                {
                    return S_FALSE;
                }

                if (!_there_is_no_index_buffer)
                {
                    if (_create_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                        nullptr,
                        _indices_size,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                        this->_index_buffer) == S_FALSE)
                    {
                        return S_FALSE;
                    }
                }

                if (_copy_DRAM_to_VRAM(pVerticesSize, _indices_size) == S_FALSE)
                {
                    return S_FALSE;
                }

                if (!pUseDynamicBuffer)
                {
                    //release staging buffers
                    this->_stagings_buffers.vertices.release();
                    if (!_there_is_no_index_buffer)
                    {
                        this->_stagings_buffers.indices.release();
                    }
                }
    
                auto _hr = _load_texture();
                if (_hr == S_FALSE)
                {
                    return S_FALSE;
                }

                return _load_pipeline(pRenderPass, pPipelineCacheName);
            }

            HRESULT update_dynamic_buffer(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const void* const pVerticesData,
                _In_ const UINT pVerticesSize,
                _In_ const UINT pVerticesCount,
                _In_ const UINT* const pIndicesData,
                _In_ const UINT pIndicesCount)
            {
                if (!this->_dynamic_buffer)
                {
                    V(S_FALSE, "updating none dynamic buffer.", this->_name, 2);
                    return S_FALSE;
                }
                if (pVerticesCount != this->_vertices_count ||
                    pIndicesCount != this->_indices_count)
                {
                    V(S_FALSE, "Size of vertex or index buffer does not match.", this->_name, 2);
                    return S_FALSE;
                }

                auto _hr = this->_stagings_buffers.vertices.set_data(pVerticesData);
                V(_hr, "updating staging vertex buffer", this->_name, 3);

                _hr = this->_stagings_buffers.vertices.bind();
                V(_hr, "binding to staging vertex buffer", this->_name, 3);

                if (pIndicesCount && pIndicesData)
                {
                    _hr = this->_stagings_buffers.indices.set_data(pIndicesData);
                    V(_hr, "updating staging index buffer", this->_name, 3);

                    _hr = this->_stagings_buffers.vertices.bind();
                    V(_hr, "binding staging index buffer", this->_name, 3);
                }

                return _copy_DRAM_to_VRAM(pVerticesSize, this->_indices_count * sizeof(UINT));
            }

            void render(_In_ const VkCommandBuffer& pCommandBuffer, _In_ const VkBuffer& pInstanceHandle,
                _In_ uint32_t& pInstancesCount)
            {
                auto _pipeline_handle = this->_pipeline.get_handle();
                auto _pipeline_layout_handle = this->_pipeline.get_layout_handle();
                auto _descriptor_set = this->_shader->get_descriptor_set();
                
                vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_handle);
                
                if (_descriptor_set)
                {
                    vkCmdBindDescriptorSets(pCommandBuffer,
                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                        _pipeline_layout_handle,
                        0,
                        1,
                        &_descriptor_set,
                        0,
                        nullptr);
                }

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
   
            const UINT get_vertices_count() const
            {
                return this->_vertices_count;
            }
            
            const UINT get_indices_count() const
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

            const w_mesh::w_vertex_declaration get_vertex_declaration_struct()
            {
                return this->_vertex_declaration;
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

            void set_vertex_declaration_struct(_In_ const w_mesh::w_vertex_declaration& pValue)
            {
                this->_vertex_declaration = pValue;
            }

#pragma endregion

            void release()
            {
                //release vertex and index buffers

                this->_vertex_buffer.release();
                if (this->_indices_count)
                {
                    this->_index_buffer.release();
                }
                this->_pipeline.release();
                
                if (this->_dynamic_buffer)
                {
                    this->_stagings_buffers.vertices.release();
                    if (this->_indices_count)
                    {
                        this->_stagings_buffers.indices.release();
                    }
                    SAFE_DELETE(this->_copy_command_buffer);
                }

                this->_shader = nullptr;
                this->_texture = nullptr;
                this->_gDevice = nullptr;
            }

        private:
            
            HRESULT _copy_DRAM_to_VRAM(
                _In_ const UINT pVerticesSize,
                _In_ const UINT pIndicesSize)
            {
                //create one command buffer 
                if (!this->_copy_command_buffer)
                {
                    this->_copy_command_buffer = new w_command_buffers();
                    _copy_command_buffer->load(this->_gDevice, 1);
                }

                this->_copy_command_buffer->begin(0);
                {
                    auto _copy_cmd = _copy_command_buffer->get_command_at(0);

                    // Vertex buffer
                    VkBufferCopy _copy_region = {};
                    _copy_region.size = pVerticesSize;
                    vkCmdCopyBuffer(
                        _copy_cmd,
                        _stagings_buffers.vertices.get_handle(),
                        this->_vertex_buffer.get_handle(),
                        1,
                        &_copy_region);

                    if (pIndicesSize)
                    {
                        // Index buffer
                        _copy_region.size = pIndicesSize;
                        vkCmdCopyBuffer(
                            _copy_cmd,
                            _stagings_buffers.indices.get_handle(),
                            this->_index_buffer.get_handle(),
                            1,
                            &_copy_region);
                    }
                }

                this->_copy_command_buffer->flush(0);
                if (!this->_dynamic_buffer)
                {
                    SAFE_DELETE(this->_copy_command_buffer);
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
            

            HRESULT _load_texture()
            {
                this->_texture = w_texture::default_texture;
                return S_OK;
            }

            HRESULT _load_pipeline(_In_ const w_render_pass* pRenderPass,
                _In_ const std::string& pPipelineCacheName)
            {
                HRESULT _hr = S_OK;

                using namespace wolf::content_pipeline;

                std::vector<VkVertexInputBindingDescription> _vertex_binding_descriptions;
                std::vector<VkVertexInputAttributeDescription> _vertex_attribute_descriptions;

                switch (this->_vertex_declaration)
                {
                default:
                case w_mesh::w_vertex_declaration::VERTEX_POSITION:
                {

                }
                break;
                case w_mesh::w_vertex_declaration::VERTEX_POSITION_UV:
                {
                    //create pipeline for basic shader
                    _vertex_binding_descriptions.push_back(
                    {
                        0,                                                                                        // Binding
                        sizeof(vertex_declaration_structs::vertex_position_uv),                                   // Stride
                        VK_VERTEX_INPUT_RATE_VERTEX                                                               // InputRate
                    });
                    _vertex_attribute_descriptions.push_back(
                    {
                        0,                                                                                        // Location
                        _vertex_binding_descriptions[0].binding,                                                  // Binding
                        VK_FORMAT_R32G32B32A32_SFLOAT,                                                            // Format
                        offsetof(vertex_declaration_structs::vertex_position_uv, position)                        // Offset
                    });
                    _vertex_attribute_descriptions.push_back(
                    { 
                        1,                                                                                        // Location
                        _vertex_binding_descriptions[0].binding,                                                  // Binding
                        VK_FORMAT_R32G32_SFLOAT,                                                                  // Format
                        offsetof(vertex_declaration_structs::vertex_position_uv, uv)                              // Offset
                    });
                }
                break;
                case  w_mesh::w_vertex_declaration::VERTEX_POSITION_UV_INSTANCE_VEC7_INT:
                {
                    //create pipeline for instance shader
                    _vertex_binding_descriptions.push_back(
                    {
                        0,                                                                                        // Binding
                        sizeof(vertex_declaration_structs::vertex_position_uv),                                   // Stride
                        VK_VERTEX_INPUT_RATE_VERTEX                                                               // InputRate
                    });
                    _vertex_binding_descriptions.push_back(
                    {
                        1,                                                                                        // Binding
                        sizeof(wolf::content_pipeline::w_cpipeline_model::w_instance_info),                       // Stride
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
                    
                    /*
                        Per instance attributes: 
                        vec3        i_instance_pos;
                        vec3        i_instance_rot;
                        float       i_instance_scale;
                        int         i_instance_uv_index;
                    */
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
                case w_mesh::w_vertex_declaration::VERTEX_POSITION_UV_COLOR:
                {
                    //create pipeline for basic shader
                    _vertex_binding_descriptions.push_back(
                    {
                        0,                                                                                              // Binding
                        sizeof(vertex_declaration_structs::vertex_position_uv_color),                                   // Stride
                        VK_VERTEX_INPUT_RATE_VERTEX                                                                     // InputRate
                    });
                    _vertex_attribute_descriptions.push_back(
                    {
                        0,                                                                       // Location
                        _vertex_binding_descriptions[0].binding,                                 // Binding
                        VK_FORMAT_R32G32B32_SFLOAT,                                              // Format
                        offsetof(vertex_declaration_structs::vertex_position_uv_color, position) // Offset
                    });
                    _vertex_attribute_descriptions.push_back(
                    {
                        1,                                                                       // Location
                        _vertex_binding_descriptions[0].binding,                                 // Binding
                        VK_FORMAT_R32G32_SFLOAT,                                           // Format
                        offsetof(vertex_declaration_structs::vertex_position_uv_color, uv)    // Offset
                    });
                    _vertex_attribute_descriptions.push_back(
                    {
                        2,                                                                       // Location
                        _vertex_binding_descriptions[0].binding,                                 // Binding
                        VK_FORMAT_R32G32B32A32_SFLOAT,                                                 // Format
                        offsetof(vertex_declaration_structs::vertex_position_uv_color, color)       // Offset
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
                    _vertex_binding_descriptions.size() ? 
                    &_vertex_binding_descriptions[0] : nullptr,                   // VertexBindingDescriptions
                    static_cast<uint32_t>(_vertex_attribute_descriptions.size()), // VertexAttributeDescriptionCount
                    _vertex_attribute_descriptions.size() ? 
                    &_vertex_attribute_descriptions[0] : nullptr                  // VertexAttributeDescriptions
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
                    VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,                          // Type
                    nullptr,                                                                // Next
                    0,                                                                      // Flags
                    static_cast<uint32_t>(_descriptor_set_layout == nullptr ? 0 : 1),       // SetLayoutCount
                    _descriptor_set_layout == nullptr ? nullptr : &_descriptor_set_layout,  // SetLayouts
                    0,                                                                      // PushConstantRangeCount
                    nullptr                                                                 // PushConstantRanges
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
                    &_dynamic_state_create_info,
                    true);
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
            w_pipeline                                          _pipeline;
            w_texture*                                          _texture;
            w_shader*                                           _shader;
            w_mesh::w_vertex_declaration                        _vertex_declaration;
            bool                                                _dynamic_buffer;
            w_command_buffers*                                  _copy_command_buffer;
            struct
            {
                w_buffer vertices;
                w_buffer indices;
            } _stagings_buffers;
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
                    _In_  const UINT  pVerticesSize,
                     _In_ const UINT pVerticesCount,
                     _In_ const UINT* const pIndicesData,
                     _In_ const UINT pIndicesCount,
                     _In_ w_shader* pShader,
                     _In_ const w_render_pass* pRenderPass,
                     _In_ const std::string& pPipelineCacheName,
                     _In_ const bool pZUp,
                     _In_ const bool pUseDynamicBuffer)
{
    if (!this->_pimp) return S_FALSE;
    
    return this->_pimp->load(
        pGDevice,
        pVerticesData,
        pVerticesSize,
        pVerticesCount,
        pIndicesData,
        pIndicesCount,
        pShader,
        pRenderPass,
        pPipelineCacheName,
        pZUp,
        pUseDynamicBuffer);
}

HRESULT w_mesh::update_dynamic_buffer(
    _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const void* const pVerticesData,
    _In_ const UINT pVerticesSize,
    _In_ const UINT pVerticesCount,
    _In_ const UINT* const pIndicesData,
    _In_ const UINT pIndicesCount)
{
    return this->_pimp ? this->_pimp->update_dynamic_buffer(
        pGDevice,
        pVerticesData,
        pVerticesSize,
        pVerticesCount,
        pIndicesData,
        pIndicesCount) : S_FALSE;
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

const UINT w_mesh::get_vertices_count() const
{
    return this->_pimp ? this->_pimp->get_vertices_count() : 0;
}

const UINT w_mesh::get_indices_count() const
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

const w_mesh::w_vertex_declaration w_mesh::get_vertex_declaration_struct()
{
    return this->_pimp ? w_vertex_declaration::VERTEX_POSITION : this->_pimp->get_vertex_declaration_struct();
}

#pragma endregion

#pragma region Setters

void w_mesh::set_shader(_In_ w_shader* pShader)
{
    if (!this->_pimp) return;
    this->_pimp->set_shader(pShader);
}

void w_mesh::set_texture(_In_ w_texture* pTexture)
{
    if (!this->_pimp) return;
    this->_pimp->set_texture(pTexture);
}

void w_mesh::set_vertex_declaration_struct(_In_ const w_mesh::w_vertex_declaration& pValue)
{
    if (!this->_pimp) return;
    this->_pimp->set_vertex_declaration_struct(pValue);
}

#pragma endregion
