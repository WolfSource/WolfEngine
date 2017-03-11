/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_shader_buffer.h
	Description		 : Constant HLSL buffer
	Comment          :
*/

#ifndef __W_SHADER_BUFFER_H__
#define __W_SHADER_BUFFER_H__

#include "w_graphics_device_manager.h"
#include <w_object.h>

#if defined(__linux) || defined(__APPLE__) || defined(__ANDROID)
#include <w_std.h>
#endif

namespace wolf
{
	namespace graphics
	{
		template<typename T>
		class w_shader_buffer : public wolf::system::w_object
		{
		public:
                        w_shader_buffer() :
                            _buffer_memory(nullptr),
                            _buffer_memory_reqs(nullptr)
                        {
                            std::memset(this->_buffer_description_info, 
                                    0, 
                                    sizeof(this->_buffer_description_info));
                        }
			~w_shader_buffer()
                        {
				release();
			}

			//Load the shader buffer
			void load(_In_ std::shared_ptr<w_graphics_device>& pGDevice)
			{
                            //store the shared graphics device pointer
                            this->_gDevice = pGDevice;
                            
#if defined(__WIN32) || defined(__UNIVERSAL)
                            // ID3D11Device1* pDevice
				D3D11_BUFFER_DESC _bufferDesc;
				std::memset(&_bufferDesc, 0, sizeof(D3D11_BUFFER_DESC));

				_bufferDesc.ByteWidth = sizeof(T);
				_bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				_bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

				auto _hr = pDevice->CreateBuffer(&_bufferDesc, nullptr, &this->_buffer);
#else
                                VkBufferCreateInfo _buffer_info = {};
                                _buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                                _buffer_info.pNext = nullptr;
                                _buffer_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                                _buffer_info.size = sizeof(T);
                                _buffer_info.queueFamilyIndexCount = 0;
                                _buffer_info.pQueueFamilyIndices = nullptr;
                                _buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                                _buffer_info.flags = 0;
                                
                                auto _hr = vkCreateBuffer(pGDevice->vk_device, 
                                        &_buffer_info, 
                                        NULL, 
                                        &this->_buffer_description_info.buffer);
                                V(_hr, "creating shader buffer", "w_shader_buffer", 3, false, true);
                                
                                //get the memory requirements
                                vkGetBufferMemoryRequirements(pGDevice->vk_device, 
                                        this->_buffer_description_info.buffer, 
                                        &this->_buffer_memory_reqs);
                                
                                //allocate information
                                VkMemoryAllocateInfo _alloc_info = {};
                                _alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                                _alloc_info.pNext = nullptr;
                                _alloc_info.memoryTypeIndex = 0;
                                _alloc_info.allocationSize = this->_buffer_memory_reqs.size;
    
                                _hr = w_graphics_device_manager::memory_type_from_properties(
                                        this->_buffer_memory_reqs.memoryTypeBits,
                                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                        &_alloc_info.memoryTypeIndex);
                                V(_hr, "getting shader memory type from properties. No mappable or coherent memory", "w_shader_buffer", 3, false, true);

                                _hr = vkAllocateMemory(
                                        this->_gDevice->vk_device, 
                                        &_alloc_info, 
                                        nullptr, 
                                        &this->_buffer_memory);
                                V(_hr, "allocating shader memory", "w_shader_buffer", 3, false, true);
#endif                                
			}

			// Writes new data into the constant buffer.
			void set_data(T& pValue)
			{
                              //map data to the memory
                            
#if defined(__WIN32) || defined(__UNIVERSAL)
				D3D11_MAPPED_SUBRESOURCE _mappedResource;
				std::memset(&_mappedResource, 0, sizeof(D3D11_MAPPED_SUBRESOURCE));

				this->data = &pValue;
				auto _hr = pContext->Map(_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &_mappedResource);
				{
					V(hr);
					*(T*) _mappedResource.pData = pValue;
					pContext->Unmap(_buffer, 0);
				}
                                
                                pDevice->UpdateSubresource(this->_buffer.Get(), 0, nullptr, &this->data, 0, 0);
#else
                               
                               uint8_t* _data = nullptr;
                               auto _hr = vkMapMemory(this->_gDevice->vk_device,
                                       this->_buffer_memory, 
                                       0, 
                                       this->_buffer_memory_reqs.size, 
                                       0, 
                                       (void**)&_data);
                               V(_hr, "mapping shader memory", "w_shader_buffer", 3, false, true);
                               if(_hr != VK_SUCCESS) return;
                               
                               memcpy(_data, &this->data, sizeof(T));
                               vkUnmapMemory(this->_gDevice->vk_device,
                                      this->_buffer_memory);
                               
                               _hr = vkBindBufferMemory(this->_gDevice->vk_device, 
                                       this->_buffer_description_info.buffer, 
                                       this->_buffer_memory, 
                                       0);
                               V(_hr, "binding shader buffer", "w_shader_buffer", 3, false, true);
                              
                               //set the buffer description info
                               this->_buffer_description_info.offset = 0;
                               this->_buffer_description_info.range = sizeof(T);
#endif
			}

			//Release resources
			ULONG release()
			{
				if (this->get_is_released()) return 0;
				
#if defined(__WIN32) || defined(__UNIVERSAL)
                                //release directX buffer
				COMPTR_RELEASE(this->_buffer);
#else 
                                //release vulkan shader buffer and memory
                                
                                vkDestroyBuffer(this->_gDevice->vk_device, 
                                        this->_buffer_description_info.buffer, 
                                        nullptr);                               
                                this->_buffer_description_info.buffer = nullptr;
                                this->_buffer_description_info.offset = 0;
                                this->_buffer_description_info.range = 0;
                                
                                vkFreeMemory(this->_gDevice->vk_device, 
                                        this->_buffer_memory, 
                                        nullptr);
                                this->_buffer_memory = nullptr;                           
#endif
				return _super::release();
			}

#pragma region Getters
                        
#if  defined(__WIN32) || defined(__UNIVERSAL)
			// Get the underlying D3D constant buffer.
			ID3D11Buffer* get_buffer() const { return this->_buffer.Get(); }
#endif
                        
#pragma endregion

			T data;

		private:
                        typedef  wolf::system::w_object      _super;    
                        std::shared_ptr<w_graphics_device>   _gDevice;
                                
#if  defined(__WIN32) || defined(__UNIVERSAL)
			Microsoft::WRL::ComPtr<ID3D11Buffer> _buffer;
#else 
                        VkDeviceMemory                       _buffer_memory;
                        VkMemoryRequirements                 _buffer_memory_reqs;
                        VkDescriptorBufferInfo               _buffer_description_info;
#endif
		};
	}
}

#endif