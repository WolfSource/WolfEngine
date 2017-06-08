/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_shader_buffer.h
	Description		 : HLSL/GLSL shader buffer(in HLSL refers to constant buffers and in GLSL refers to Uniforms)
	Comment          :
*/

#ifndef __W_SHADER_BUFFER_H__
#define __W_SHADER_BUFFER_H__

#include "w_graphics_device_manager.h"
#include <w_graphics/w_command_buffers.h>
#include "w_buffer.h"

namespace wolf
{
	namespace graphics
	{
		template<typename T>
		class w_uniform : public wolf::system::w_object
		{
		public:
            w_uniform() :
                _host_visible(false)
            {
                _super::name = "w_uniform";
            }
            
			~w_uniform()
            {
				release();
			}

			/*
                Load the uniform buffer
                _In_ pGDevice : Graphics Device
                _In_ pHostVisible : True means host memory of uniform's buffer in DRAM, otherwise host memory in VRAM
            */
			HRESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
                _In_ const bool& pHostVisible = false)
			{
                const std::string _trace = this->name + "update";

                this->_host_visible = pHostVisible;

                //store the shared graphics device pointer
                auto _buffer_size = static_cast<UINT32>(sizeof(T));
                
                this->_gDevice = pGDevice;
                
                //create uniform buffer
                HRESULT _hr = S_OK;

                if (this->_host_visible)
                {
                    _hr = this->_buffer.load(pGDevice,
                        _buffer_size,
                        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

                    if (_hr == S_FALSE)
                    {
                        V(_hr, "loading host visible buffer " +
                            _gDevice->print_info(),
                            _trace,
                            3);
                        return _hr;
                    }
                }
                else
                {
                    _hr = this->_buffer.load(pGDevice,
                        _buffer_size,
                        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
                    
                    if (_hr == S_FALSE)
                    {
                        V(_hr, "loading device buffer " +
                            _gDevice->print_info(),
                            _trace,
                            3);
                        return _hr;
                    }

                    //create staging buffer
                    _hr = this->_staging_buffer.load(pGDevice,
                        _buffer_size,
                        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
                    if (_hr == S_FALSE)
                    {
                        V(_hr, "loading staging buffer " +
                            _gDevice->print_info(),
                            _trace,
                            3);
                        return _hr;
                    }

                    _hr = this->_staging_buffer.bind();
                    if (_hr == S_FALSE)
                    {
                        V(_hr, "binding device buffer " +
                            _gDevice->print_info(),
                            _trace,
                            3);
                        return _hr;
                    }
                }
                
                _hr = this->_buffer.bind();
                if (_hr == S_FALSE)
                {
                    V(_hr, "binding buffer " +
                        _gDevice->print_info(),
                        _trace,
                        3);
                    return _hr;
                }
                                                
                return _hr;
            }

            HRESULT update()
            {
                const std::string _trace = this->name + "update";

                HRESULT _hr = S_OK;

                if (this->_host_visible)
                {
                    _hr = this->_buffer.set_data(&this->data);
                    V(_hr, "setting to host visible buffer " +
                        _gDevice->print_info(),
                        _trace,
                        3);
                }
                else
                {
                    auto _size = this->_staging_buffer.get_size();
                    auto _mapped = this->_staging_buffer.map();
                    if (_mapped)
                    {
                        memcpy(_mapped, &this->data, _size);
                        this->_staging_buffer.flush();
                    }
                    else
                    {
                        _hr = S_FALSE;
                        V(_hr, "begining command buffer " +
                            _gDevice->print_info(),
                            _trace,
                            3);
                    }
                    this->_staging_buffer.unmap();

                    if (_hr == S_FALSE) return _hr;

                    _hr = this->_staging_buffer.copy_to(this->_buffer);
                    V(_hr, "copy staging buffer to device buffer " +
                        _gDevice->print_info(),
                        _trace,
                        3);
                }

                return _hr;
            }

            const VkDescriptorBufferInfo get_descriptor_info() const
            {
                return this->_buffer.get_descriptor_info();
            }
                
			//Release resources
			ULONG release()
			{
				if (this->get_is_released()) return 0;
				
                this->_buffer.release();
                this->_staging_buffer.release();
                this->_gDevice = nullptr;
                
				return _super::release();
			}
            
            UINT32 get_size()
            {
                return this->_buffer.get_size();
            }
            
            VkBuffer get_handle()
            {
                return this->_buffer.get_handle();
            }
            
            
            //constant buffer/uniform data
			T data;

		private:
            typedef  wolf::system::w_object      _super;
            std::shared_ptr<w_graphics_device>   _gDevice;
            w_buffer                             _buffer;
            w_buffer                             _staging_buffer;
            bool                                 _host_visible;
		};
	}
}

#endif
