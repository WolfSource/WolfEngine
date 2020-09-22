/*
	Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website          : http://WolfSource.io
	Name             : py_uniform.h
	Description      : The python exporter for w_uniform template
	Comment          :
*/

#ifdef __PYTHON__

#ifndef __PY_UNIFORM_H__
#define __PY_UNIFORM_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pyWolf
{
	using namespace boost::python;
	using namespace wolf::graphics;

	class py_uniform
	{
	public:
		py_uniform() :
			_host_visible(false),
			_released(false),
			_name("py_uniform")
		{
		}

		~py_uniform()
		{
			release();
		}

		W_RESULT load(
			_In_ boost::shared_ptr<w_graphics_device>& pGDevice,
			_In_ const uint32_t& pBufferSize,
			_In_ const bool& pHostVisible = false)
		{
			if (!pGDevice.get()) return W_FAILED;

			const std::string _trace = this->_name + "update";
			if (!pBufferSize)
			{
				V(W_FAILED, "uniform buffer could not be zero" +
					_gDevice->get_info(),
					_trace,
					3);
				return W_FAILED;
			}

			this->_gDevice = boost_shared_ptr_to_std_shared_ptr<w_graphics_device>(pGDevice);
			this->_host_visible = pHostVisible;

			//create uniform buffer
			W_RESULT _hr = W_PASSED;

			if (this->_host_visible)
			{
				_hr = this->_buffer.load(this->_gDevice,
					pBufferSize,
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				if (_hr == W_FAILED)
				{
					V(_hr, "loading host visible buffer " +
						this->_gDevice->get_info(),
						_trace,
						3);
					return _hr;
				}
			}
			else
			{
				_hr = this->_buffer.load(this->_gDevice,
					pBufferSize,
					VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

				if (_hr == W_FAILED)
				{
					V(_hr, "loading device buffer " +
						this->_gDevice->get_info(),
						_trace,
						3);
					return _hr;
				}

				//create staging buffer
				_hr = this->_staging_buffer.load(this->_gDevice,
					pBufferSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
				if (_hr == W_FAILED)
				{
					V(_hr, "loading staging buffer " +
						this->_gDevice->get_info(),
						_trace,
						3);
					return _hr;
				}

				_hr = this->_staging_buffer.bind();
				if (_hr == W_FAILED)
				{
					V(_hr, "binding device buffer " +
						this->_gDevice->get_info(),
						_trace,
						3);
					return _hr;
				}
			}

			_hr = this->_buffer.bind();
			if (_hr == W_FAILED)
			{
				V(_hr, "binding buffer " +
					this->_gDevice->get_info(),
					_trace,
					3);
				return _hr;
			}

			return _hr;
		}

		W_RESULT update(_In_ boost::python::list pDataStructure)
		{
			const std::string _trace = this->_name + "update";

			W_RESULT _hr = W_PASSED;

			std::vector<float> _v;
			if (!boost_list_to_std_vector(pDataStructure, _v))
			{
				V(_hr, "lenght of extracted list is not valid" +
					_gDevice->get_info(),
					_trace,
					3);
				return W_FAILED;
			}
			
			if (this->_host_visible)
			{
				_hr = this->_buffer.set_data(_v.data());
				V(_hr, "setting to host visible buffer " +
					_gDevice->get_info(),
					_trace,
					3);
			}
			else
			{
				auto _size = this->_staging_buffer.get_size();
				auto _mapped = this->_staging_buffer.map();
				if (_mapped)
				{
					memcpy(_mapped, _v.data(), _size);
					this->_staging_buffer.flush();
				}
				else
				{
					_hr = W_FAILED;
					V(_hr, "begining command buffer " +
						_gDevice->get_info(),
						_trace,
						3);
				}
				this->_staging_buffer.unmap();

				if (_hr == W_FAILED) return _hr;

				_hr = this->_staging_buffer.copy_to(this->_buffer);
				V(_hr, "copy staging buffer to device buffer " +
					_gDevice->get_info(),
					_trace,
					3);
			}

			return _hr;
		}

		//Release resources
		ULONG release()
		{
			if (this->_released) return 1;

			this->_buffer.release();
			this->_staging_buffer.release();
			this->_gDevice = nullptr;
			this->_released = true;

			return 0;
		}

#pragma region Getters

		const w_descriptor_buffer_info get_descriptor_info() const
		{
			return this->_buffer.get_descriptor_info();
		}

		uint32_t get_size()
		{
			return this->_buffer.get_size();
		}

		w_buffer_handle get_handle()
		{
			return this->_buffer.get_buffer_handle();
		}
#pragma endregion

	private:
		std::shared_ptr<w_graphics_device>   _gDevice;
		std::string							 _name;
		w_buffer                             _buffer;
		w_buffer                             _staging_buffer;
		bool                                 _host_visible;
		bool								 _released;
	};

	BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(py_uniform_load_overloads, py_uniform::load, 2, 3)

	static void py_uniform_export()
	{
		//export py_uniform class
		class_<py_uniform, boost::noncopyable>("w_uniform")
			.def("load", &py_uniform::load, py_uniform_load_overloads())
			.def("update", &py_uniform::update, "update uniform")
			.def("release", &py_uniform::release, "release all resources of uniform")
			.def("get_descriptor_info", &py_uniform::get_descriptor_info, "get descriptor info of uniform")
			.def("get_size", &py_uniform::get_size, "get size of uniform")
			.def("get_handle", &py_uniform::get_handle, "get handle of uniform")
			;
	}
}

#endif//__PY_UNIFORM_H__

#endif//__PYTHON__
