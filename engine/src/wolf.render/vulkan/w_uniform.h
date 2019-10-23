/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (https://PooyaEimandar.github.io) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/WolfEngine/Wolf.Engine/issues
	Website			 : https://WolfEngine.App
	Name			 : w_shader_buffer.h
	Description		 : HLSL/GLSL shader buffer(in HLSL refers to constant buffers and in GLSL refers to Uniforms)
	Comment          :
*/

#ifndef __W_SHADER_BUFFER_H__
#define __W_SHADER_BUFFER_H__

#include "w_graphics_device_manager.h"
#include <vulkan/w_command_buffers.h>
#include "w_buffer.h"

namespace wolf
{
	namespace render
	{
		namespace vulkan
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
				W_RESULT load(_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
					_In_ const bool& pHostVisible = false)
				{
					const std::string _trace_info = this->name + "::update";

					this->_host_visible = pHostVisible;

					//store the shared graphics device pointer
					auto _buffer_size = static_cast<uint32_t>(sizeof(T));

					this->_gDevice = pGDevice;

					//create uniform buffer
					W_RESULT _hr = W_PASSED;

					if (this->_host_visible)
					{
						_hr = this->_buffer.allocate(pGDevice,
							_buffer_size,
							VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
							w_memory_usage_flag::MEMORY_USAGE_CPU_ONLY);

						if (_hr == W_FAILED)
						{
							V(_hr,
								w_log_type::W_ERROR,
								"loading host visible buffer. graphics device: {}. trace info: {}",
								_gDevice->get_info(),
								_trace_info);
							return _hr;
						}
					}
					else
					{
						_hr = this->_buffer.allocate(_gDevice,
							_buffer_size,
							VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
							w_memory_usage_flag::MEMORY_USAGE_GPU_ONLY);

						if (_hr == W_FAILED)
						{
							V(_hr,
								w_log_type::W_ERROR,
								"loading device buffer. graphics device: {}. trace info: {}",
								_gDevice->get_info(),
								_trace_info);
							return _hr;
						}

						//create staging buffer
						_hr = this->_staging_buffer.allocate(_gDevice,
							_buffer_size,
							VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
							w_memory_usage_flag::MEMORY_USAGE_CPU_ONLY);
						if (_hr == W_FAILED)
						{
							V(_hr,
								w_log_type::W_ERROR,
								"loading staging buffer. graphics device: {}. trace info: {}",
								_gDevice->get_info(),
								_trace_info);
							return _hr;
						}

						//_hr = this->_staging_buffer.bind();
						//if (_hr == W_FAILED)
						//{
						//    V(_hr, "binding device buffer " +
						//        _gDevice->get_info(),
						//        _trace,
						//        3);
						//    return _hr;
						//}
					}

					//_hr = this->_buffer.bind();
					//if (_hr == W_FAILED)
					//{
					//    V(_hr, "binding buffer " +
					//        _gDevice->get_info(),
					//        _trace,
					//        3);
					//    return _hr;
					//}

					return _hr;
				}

				W_RESULT update()
				{
					const std::string _trace_info = this->name + "update";

					W_RESULT _hr = W_PASSED;

					if (this->_host_visible)
					{
						_hr = this->_buffer.set_data(&this->data);
						V(_hr,
							w_log_type::W_ERROR,
							"setting to host visible buffer. graphics device : {}.trace info : {}",
							_gDevice->get_info(),
							_trace_info);
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
							_hr = W_FAILED;
							V(_hr,
								w_log_type::W_ERROR,
								"begining command buffer . graphics device : {}.trace info : {}",
								_gDevice->get_info(),
								_trace_info);
						}
						this->_staging_buffer.unmap();

						if (_hr == W_FAILED) return _hr;

						_hr = this->_staging_buffer.copy_to(this->_buffer);
						V(_hr,
							w_log_type::W_ERROR,
							"copy staging buffer to device buffer . graphics device : {}.trace info : {}",
							_gDevice->get_info(),
							_trace_info);
					}

					return _hr;
				}

				const w_descriptor_buffer_info get_descriptor_info() const
				{
					return this->_buffer.get_descriptor_info();
				}

				//Release resources
				ULONG release()
				{
					if (this->get_is_released()) return 1;

					this->_buffer.release();
					this->_staging_buffer.release();
					this->_gDevice = nullptr;

					return _super::release();
				}

				uint32_t get_size()
				{
					return this->_buffer.get_size();
				}

				w_buffer_handle get_handle()
				{
					return this->_buffer.get_buffer_handle();
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
}

#include "python_exporter/py_uniform.h"

#endif
