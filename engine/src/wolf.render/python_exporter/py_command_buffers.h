/*
    Project          : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
    Source           : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
    Website          : http://WolfSource.io
    Name             : py_command_buffer.h
    Description      : The python exporter for w_command_buffer class
    Comment          :
 */

#ifdef __PYTHON__

#ifndef __PY_COMMAND_BUFFERS_H__
#define __PY_COMMAND_BUFFERS_H__

#include <python_exporter/w_boost_python_helper.h>

namespace pyWolf
{
	using namespace boost::python;
	using namespace wolf::graphics;

	BOOST_PYTHON_MEMBER_FUNCTION_OVERLOADS(w_command_buffers_begin_overloads, w_command_buffers::begin, 1, 2)

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

		//export w_command_buffer struct
		class_<w_command_buffer>("w_command_buffer");

		//export w_command_buffers class
		class_<w_command_buffers, boost::noncopyable>("w_command_buffers")
			.def("load", &w_command_buffers::py_load, "load command buffers")
			.def("begin", &w_command_buffers::begin, w_command_buffers_begin_overloads())
			.def("end", &w_command_buffers::end, "end command buffer")
			.def("flush", &w_command_buffers::flush, "flush command buffer")
			.def("flush_all", &w_command_buffers::flush_all, "flush all command buffers")
			.def("release", &w_command_buffers::release, "release all resources")
			.def("get_commands", &w_command_buffers::py_get_commands, "get command buffers")
			.def("get_command_at", &w_command_buffers::get_command_at, "get command buffer at")
			.def("get_commands_size", &w_command_buffers::get_commands_size, "get command buffer size")
			;
	}
}

#endif//__PY_COMMAND_BUFFER_H__

#endif//__PYTHON__
