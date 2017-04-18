#include "w_render_pch.h"
#include "w_mesh.h"
#include <atomic>
#include "w_buffer.h"
#include "w_command_buffers.h"

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
                _gDevice(nullptr)
            {
            }
            
            //Initialize mesh
            W_EXP HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                               _In_ const void* const pVerticesData,
                               _In_ const UINT pVerticesSize,
                               _In_ const unsigned short* const pIndicesData,
                               _In_ const UINT pIndicesSize,
                               _In_ bool pStaging)
            {
                this->_gDevice = pGDevice;
                this->_staging = pStaging;
                
                if(this->_staging)
                {
                    return _create_staging_buffers(pVerticesData,
                                                   pVerticesSize,
                                                   pIndicesData,
                                                   pIndicesSize,
                                                   this->_vertex_buffer,
                                                   this->_index_buffer);
                }
                else
                {
                    //vertex buffer is necessary
                    if(pVerticesSize == 0 || pVerticesData == nullptr ||
                       _create_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       pVerticesData,
                       pVerticesSize,
                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                       this->_vertex_buffer))
                    {
                        return S_FALSE;
                    }
                
                    //index buffer is not necessary
                    if(pIndicesSize && pIndicesData!= nullptr &&
                       _create_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                  pIndicesData,
                                  pIndicesSize,
                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                  this->_index_buffer))
                    {
                        return S_FALSE;
                    }
                    
                    return S_OK;
                }
            }
            
            VkBuffer get_vertex_buffer_handle() const
            {
                return this->_vertex_buffer.get_handle();
            }
            
            VkBuffer get_index_buffer_handle() const
            {
                return this->_index_buffer.get_handle();
            }
            
            ULONG release()
            {
                //release vertex and index buffers
                
                this->_vertex_buffer.release();
                this->_index_buffer.release();
            
                this->_gDevice = nullptr;
                
                return 1;
            }
            
            static  std::atomic<uint64_t>                     staging_command_buffers_current_index;
            
        private:
            
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
                                            _In_ const unsigned short* const pIndicesData,
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
                               pVerticesData,
                               pVerticesSize,
                               VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                               _stagings_buffers.vertices) == S_FALSE)
                    {
                        return S_FALSE;
                    }
                }
            
                
                // create VRAM buffers
                if(_create_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                  pVerticesData,
                                  pVerticesSize,
                                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                  pVertexBuffer) == S_FALSE)
                {
                    return S_FALSE;
                }
                
                if(!_there_is_no_index_buffer)
                {
                    if(_create_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                      pVerticesData,
                                      pVerticesSize,
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
                
                    VkBufferCopy _copy_region = {};
                
                    // Vertex buffer
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
                Create host visible buffers for rendering, This will result in poor rendering performance.
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
                
                //we can not access to VRAM
                if (!(pMemoryFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
                {
                    void* _data = nullptr;
                    auto _hr = vkMapMemory(this->_gDevice->vk_device,
                                      pBuffer.get_memory(),
                                      0,
                                      pBuffer.get_size(),
                                      0,
                                      &_data);
                    if(_hr)
                    {
                        _data = nullptr;
                        V(S_FALSE, "mapping data to to vertex buffer's memory for graphics device: " +
                            _gDevice->device_name + " ID:" + std::to_string(_gDevice->device_id),
                            this->_name, 3, false, true);
                    
                        return S_FALSE;
                    }
                
                    memcpy(_data, pBufferData, (size_t)pBuffer.get_size());
                
                    VkMappedMemoryRange _flush_range =
                    {
                        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,            // Type
                        nullptr,                                          // Next
                        pBuffer.get_memory(),                             // Memory
                        0,                                                // Offset
                        VK_WHOLE_SIZE                                     // Size
                    };
                
                    vkFlushMappedMemoryRanges(this->_gDevice->vk_device, 1, &_flush_range );
                
                    //unmap memory
                    vkUnmapMemory(this->_gDevice->vk_device, pBuffer.get_memory());
                    
                    _data = nullptr;
                }
                
                //bind to it
                auto _hr = pBuffer.bind();
                if(_hr)
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
            bool                                                _staging;
        };
    }
}

std::atomic<uint64_t> w_mesh_pimp::staging_command_buffers_current_index;
static std::once_flag _initialize_flag;

w_mesh::w_mesh() : _pimp(new w_mesh_pimp())
{
	_super::set_class_name("w_mesh");
    
    //call once
    std::call_once(_initialize_flag, []()
                   {
                       w_mesh_pimp::staging_command_buffers_current_index = 0;
                   });
}

w_mesh::~w_mesh()
{
	release();
}

HRESULT w_mesh::load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                     _In_ const void* const pVerticesData,
                     _In_ const UINT pVerticesSize,
                     _In_ const unsigned short* const pIndicesData,
                     _In_ const UINT pIndicesSize,
                     _In_ bool pStaging)
{
    return this->_pimp ? this->_pimp->load(pGDevice,
                                           pVerticesData,
                                           pVerticesSize,
                                           pIndicesData,
                                           pIndicesSize,
                                           pStaging) : S_FALSE;
}

ULONG w_mesh::release()
{
    if (this->get_is_released()) return 0;
    
    SAFE_RELEASE(this->_pimp);
   
    return _super::release();
}

VkBuffer w_mesh::get_vertex_buffer_handle() const
{
    return this->_pimp ? this->_pimp->get_vertex_buffer_handle() : VK_NULL_HANDLE;
}

VkBuffer w_mesh::get_index_buffer_handle() const
{
    return this->_pimp ? this->_pimp->get_index_buffer_handle() : VK_NULL_HANDLE;
}
