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
		py_command_buffer()
		{
		}

		

#pragma region Setters
		
#pragma endregion

	private:
		typedef w_command_buffer _super;

	};

	static void py_command_buffer_export()
	{
		//export w_game class
		class_<py_command_buffer, boost::noncopyable>("w_command_buffer")
			;
	}
}

#endif//__PY_COMMAND_BUFFER_H__

#endif//__PYTHON__
