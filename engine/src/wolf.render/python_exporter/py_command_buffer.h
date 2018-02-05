/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : py_command_buffer.h
    Description      : The python exporter for w_command_buffer class
    Comment          :
 */

#ifdef __PYTHON__

#ifndef __PY_COMMAND_BUFFER_H__
#define __PY_COMMAND_BUFFER_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pywolf
{
	using namespace boost::python;
	using namespace wolf::graphics;

	class py_command_buffer : public w_command_buffer
	{
	public:
		py_command_buffer() : _graphics_device_index(-1)
		{
		}

		bool py_load(
			_In_ const uint32_t& pGDeviceIndex,
			_In_ const size_t& pCount,
			_In_ const w_command_buffer_level& pLevel)
		{
			if (pGDeviceIndex >= pywolf::py_graphics_devices.size()) return false;
			this->_graphics_device_index = pGDeviceIndex;
			return _super::load(
				pywolf::py_graphics_devices[pGDeviceIndex],
				pCount,
				pLevel) == S_OK;
		}

		bool py_begin(_In_ const size_t& pCommandBufferIndex,
			_In_ const w_command_buffer_usage_flags pFlags) {
			return _super::begin(pCommandBufferIndex, pFlags) == S_OK;
		}
		bool py_begin_all(_In_ w_command_buffer_usage_flags pFlags) { return _super::begin_all(pFlags) == S_OK; }
		bool py_end(_In_ const size_t& pCommandBufferIndex) { return _super::end(pCommandBufferIndex) == S_OK; }
		bool py_end_all() { return _super::end_all() == S_OK; }
		bool py_flush(_In_ const size_t& pCommandBufferIndex) { return _super::flush(pCommandBufferIndex) == S_OK; }
		bool py_flush_all() { return _super::flush_all() == S_OK; }

#pragma region Getters

#pragma endregion

#pragma region Setters

#pragma endregion

	private:
		typedef     w_command_buffer _super;
		short       _graphics_device_index;
	};

	static void py_command_buffer_export()
	{
		//export w_command_buffer_level enum
		enum_<w_command_buffer_level>("w_command_buffer_level")
			.value("W_COMMAND_BUFFER_LEVEL_PRIMARY", w_command_buffer_level::W_COMMAND_BUFFER_LEVEL_PRIMARY)
			.value("W_COMMAND_BUFFER_LEVEL_SECONDARY", w_command_buffer_level::W_COMMAND_BUFFER_LEVEL_SECONDARY)
			.export_values()
			;

		//export w_command_buffer_usage_flag_bits enum
		enum_<w_command_buffer_usage_flag_bits>("w_command_buffer_usage_flag_bits")
			.value("W_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT", w_command_buffer_usage_flag_bits::W_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)
			.value("W_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT", w_command_buffer_usage_flag_bits::W_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT)
			.value("VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT", w_command_buffer_usage_flag_bits::W_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT)
			.export_values()
			;

		//export w_command_buffer class
		class_<py_command_buffer, boost::noncopyable>("w_command_buffer")
			.def("load", &py_command_buffer::py_load, "load command buffer(s)")
			.def("begin", &py_command_buffer::py_begin, "begin command buffer")
			.def("end", &py_command_buffer::py_end, "end command buffer")
			.def("begin_all", &py_command_buffer::py_begin_all, "begin all command buffers")
			.def("end_all", &py_command_buffer::py_end_all, "end all command buffers")
			.def("flush", &py_command_buffer::py_flush, "flush command buffer")
			.def("flush_all", &py_command_buffer::py_flush_all, "flush all command buffers")
			.def("release", &py_command_buffer::release, "release all resources")
			.def("get_enable", &py_command_buffer::get_enable, "get whether command buffer is enable")
			.def("get_commands_size", &py_command_buffer::get_commands_size, "get command buffer size")
			.def("set_enable", &py_command_buffer::set_enable, "set enable")
			;
	}
}

#endif//__PY_COMMAND_BUFFER_H__

#endif//__PYTHON__
