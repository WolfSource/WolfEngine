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

	static void py_command_buffer_export()
	{
		//export w_command_buffer_level enum
		enum_<w_command_buffer_level>("w_command_buffer_level")
			.value("PRIMARY", w_command_buffer_level::PRIMARY)
			.value("SECONDARY", w_command_buffer_level::SECONDARY)
			.export_values()
			;

		//export w_command_buffer_usage_flag_bits enum
		enum_<w_command_buffer_usage_flag_bits>("w_command_buffer_usage_flag_bits")
			.value("ONE_TIME_SUBMIT_BIT", w_command_buffer_usage_flag_bits::ONE_TIME_SUBMIT_BIT)
			.value("RENDER_PASS_CONTINUE_BIT", w_command_buffer_usage_flag_bits::RENDER_PASS_CONTINUE_BIT)
			.value("SIMULTANEOUS_USE_BIT", w_command_buffer_usage_flag_bits::SIMULTANEOUS_USE_BIT)
			.export_values()
			;

		//export w_command_buffer class
		class_<w_command_buffer, boost::noncopyable>("w_command_buffer")
			.def("load", &w_command_buffer::py_load, "load command buffer(s)")
			.def("begin", &w_command_buffer::py_begin, "begin command buffer")
			.def("end", &w_command_buffer::py_end, "end command buffer")
			.def("flush", &w_command_buffer::py_flush, "flush command buffer")
			.def("flush_all", &w_command_buffer::py_flush_all, "flush all command buffers")
			.def("release", &w_command_buffer::release, "release all resources")
			.def("get_commands_size", &w_command_buffer::get_commands_size, "get command buffer size")
			.def("set_active_command", &w_command_buffer::set_active_command, "set active command buffer")
			;
	}
}

#endif//__PY_COMMAND_BUFFER_H__

#endif//__PYTHON__
