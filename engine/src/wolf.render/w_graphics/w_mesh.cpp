#include "w_render_pch.h"
#include "w_mesh.h"
#include "w_buffer.h"
#include "w_command_buffers.h"
#include "w_uniform.h"

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
                _copy_command_buffer(nullptr),
                _vertices_count(0),
                _indices_count(0)
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
                _In_ const uint32_t  pVerticesSize,
                _In_ const uint32_t pVerticesCount,
                _In_ const uint32_t* const pIndicesData,
                _In_ const uint32_t pIndicesCount,
                _In_ const bool pUseDynamicBuffer)
            {
                this->_gDevice = pGDevice;
                this->_vertices_count = pVerticesCount;
                this->_indices_count = pIndicesCount;
                this->_dynamic_buffer = pUseDynamicBuffer;

                if (pVerticesCount == 0 || pVerticesData == nullptr)
                {
                    return S_FALSE;
                }

                bool _there_is_no_index_buffer = false;
                uint32_t _indices_size = pIndicesCount * sizeof(uint32_t);
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

                if (!this->_texture)
                {
                    this->_texture = w_texture::default_texture;
                }

                return S_OK;
            }

            HRESULT update_dynamic_buffer(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const void* const pVerticesData,
                _In_ const uint32_t pVerticesSize,
                _In_ const uint32_t pVerticesCount,
                _In_ const uint32_t* const pIndicesData,
                _In_ const uint32_t pIndicesCount)
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

            HRESULT draw(_In_ const VkCommandBuffer& pCommandBuffer, 
                _In_ const VkBuffer& pInstanceHandle,
                _In_ const uint32_t& pInstancesCount,
                _In_ const bool& pIndrectDraw)
            {
                VkDeviceSize _offsets[1] = { 0 };

                auto _vertex_buffer_handle = this->_vertex_buffer.get_handle();
				if (!_vertex_buffer_handle) return S_FALSE;
				
                vkCmdBindVertexBuffers(pCommandBuffer, 0, 1, &_vertex_buffer_handle, _offsets);

                if (pInstanceHandle)
                {
                    vkCmdBindVertexBuffers(pCommandBuffer, 1, 1, &pInstanceHandle, _offsets);
                }

				bool _draw_indexed = false;
                auto _index_buffer_handle = this->_index_buffer.get_handle();
				if (_index_buffer_handle)
				{
					_draw_indexed = true;
					vkCmdBindIndexBuffer(pCommandBuffer, _index_buffer_handle, 0, VK_INDEX_TYPE_UINT32);
				}

                if (!pIndrectDraw)
                {
					if (_draw_indexed)
					{
						vkCmdDrawIndexed(pCommandBuffer, this->_indices_count, pInstancesCount + 1, 0, 0, 0);
					}
					else
					{
						vkCmdDraw(pCommandBuffer, this->_vertices_count, pInstancesCount + 1, 0, 0);
					}
                }

                _vertex_buffer_handle = nullptr;
                _index_buffer_handle = nullptr;

				return S_OK;
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

            w_texture* get_texture() const
            {
                return this->_texture;
            }

            const w_vertex_binding_attributes get_vertex_binding_attributes() const
            {
                return this->_vertex_binding_attributes;
            }

#pragma endregion

#pragma region Setters

            void set_texture(_In_ w_texture* pTexture)
            {
                this->_texture = pTexture;
            }

            void set_vertex_binding_attributes(_In_ const w_vertex_declaration& pVertexBindingAttributes)
            {
                if (pVertexBindingAttributes == w_vertex_declaration::NOT_DEFINED) return;

                std::vector<w_vertex_attribute> _attr;

                switch (pVertexBindingAttributes)
                {
                case w_vertex_declaration::USER_DEFINED:
                    break;
                case w_vertex_declaration::VERTEX_POSITION:
                    _attr.push_back(w_vertex_attribute::Vec3);
                    break;
                case w_vertex_declaration::VERTEX_POSITION_COLOR:
                    _attr.push_back(w_vertex_attribute::Vec3);
                    _attr.push_back(w_vertex_attribute::Vec4);
                    break;
                case w_vertex_declaration::VERTEX_POSITION_NORMAL_COLOR:
                    _attr.push_back(w_vertex_attribute::Vec3);
                    _attr.push_back(w_vertex_attribute::Vec3);
                    _attr.push_back(w_vertex_attribute::Vec4);
                    break;
                case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV:
                    _attr.push_back(w_vertex_attribute::Vec3);
                    _attr.push_back(w_vertex_attribute::Vec3);
                    _attr.push_back(w_vertex_attribute::Vec2);
                    break;
                case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL:
                    _attr.push_back(w_vertex_attribute::Vec3);
                    _attr.push_back(w_vertex_attribute::Vec3);
                    _attr.push_back(w_vertex_attribute::Vec2);
                    _attr.push_back(w_vertex_attribute::Vec3);
                    _attr.push_back(w_vertex_attribute::Vec3);
                    break;
                case w_vertex_declaration::VERTEX_POSITION_NORMAL_UV_TANGENT_BINORMAL_WEIGHT_INDICES:
                    /*_v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);
                    _v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);
                    _v.attributes.push_back(w_mesh::w_vertex_attribute::Vec2);
                    _v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);
                    _v.attributes.push_back(w_mesh::w_vertex_attribute::Vec3);*/
                    break;
                case w_vertex_declaration::VERTEX_POSITION_UV:
                    _attr.push_back(w_vertex_attribute::Vec3);
                    _attr.push_back(w_vertex_attribute::Vec2);
                    break;
                case w_vertex_declaration::VERTEX_POSITION_UV_COLOR:
                    _attr.push_back(w_vertex_attribute::Vec3);
                    _attr.push_back(w_vertex_attribute::Vec2);
                    _attr.push_back(w_vertex_attribute::Vec4);
                    break;
                }

                //clear binding attributes
                for (auto& _iter : this->_vertex_binding_attributes.binding_attributes)
                {
                    _iter.second.clear();
                }
                this->_vertex_binding_attributes.binding_attributes.clear();

                //create new binding attributes
                this->_vertex_binding_attributes.declaration = pVertexBindingAttributes;
                this->_vertex_binding_attributes.binding_attributes[0] = _attr;
            }

            void set_vertex_binding_attributes(_In_ const w_vertex_binding_attributes& pVertexBindingAttributes)
            {
                this->_vertex_binding_attributes = pVertexBindingAttributes;
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
                if (this->_dynamic_buffer)
                {
                    this->_stagings_buffers.vertices.release();
                    if (this->_indices_count)
                    {
                        this->_stagings_buffers.indices.release();
                    }
                    SAFE_DELETE(this->_copy_command_buffer);
                }

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
                                   _In_ uint32_t pBufferSize,
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
            
            std::string                                         _name;
            std::shared_ptr<w_graphics_device>                  _gDevice;
            w_buffer                                            _vertex_buffer;
            w_buffer                                            _index_buffer;
            uint32_t                                            _indices_count;
            uint32_t                                            _vertices_count;
            w_texture*                                          _texture;
            w_vertex_binding_attributes                         _vertex_binding_attributes;
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
                    _In_  const uint32_t  pVerticesSize,
                     _In_ const uint32_t pVerticesCount,
                     _In_ const uint32_t* const pIndicesData,
                     _In_ const uint32_t pIndicesCount,
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
        pUseDynamicBuffer);
}

HRESULT w_mesh::update_dynamic_buffer(
    _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const void* const pVerticesData,
    _In_ const uint32_t pVerticesSize,
    _In_ const uint32_t pVerticesCount,
    _In_ const uint32_t* const pIndicesData,
    _In_ const uint32_t pIndicesCount)
{
    return this->_pimp ? this->_pimp->update_dynamic_buffer(
        pGDevice,
        pVerticesData,
        pVerticesSize,
        pVerticesCount,
        pIndicesData,
        pIndicesCount) : S_FALSE;
}

HRESULT w_mesh::draw(_In_ const VkCommandBuffer& pCommandBuffer,
	_In_ const VkBuffer& pInstanceHandle,
	_In_ const uint32_t& pInstancesCount,
	_In_ const bool& pIndirectDraw)
{
	return this->_pimp ? this->_pimp->draw(pCommandBuffer, pInstanceHandle, pInstancesCount, pIndirectDraw) : S_FALSE;
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

const uint32_t w_mesh::get_vertices_count() const
{
    return this->_pimp ? this->_pimp->get_vertices_count() : 0;
}

const uint32_t w_mesh::get_indices_count() const
{
    return this->_pimp ? this->_pimp->get_indices_count() : 0;
}

w_texture* w_mesh::get_texture() const
{
    return this->_pimp ? this->_pimp->get_texture() : nullptr;
}

const w_vertex_binding_attributes w_mesh::get_vertex_binding_attributes() const
{
    if (!this->_pimp) return w_vertex_binding_attributes();
    return this->_pimp->get_vertex_binding_attributes();
}

#pragma endregion

#pragma region Setters

void w_mesh::set_texture(_In_ w_texture* pTexture)
{
    if (!this->_pimp) return;
    this->_pimp->set_texture(pTexture);
}

void w_mesh::set_vertex_binding_attributes(_In_ const w_vertex_declaration& pVertexBindingAttributes)
{
    if (!this->_pimp) return;
    this->_pimp->set_vertex_binding_attributes(pVertexBindingAttributes);
}

void w_mesh::set_vertex_binding_attributes(_In_ const w_vertex_binding_attributes& pVertexBindingAttributes)
{
    if (!this->_pimp) return;
    this->_pimp->set_vertex_binding_attributes(pVertexBindingAttributes);
}

#pragma endregion
