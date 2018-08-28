/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_indirect_draw_commands
	Description		 : indirect draw command buffers
	Comment          : 
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_INDIRECT_DRAW_COMMANDS_H__
#define __W_INDIRECT_DRAW_COMMANDS_H__

#include <w_graphics_device_manager.h>
#include "w_buffer.h"

namespace wolf
{
	namespace render
	{
		namespace vulkan
		{
			struct w_indirect_draws_command_buffer
			{
				wolf::render::vulkan::w_buffer								buffer;
				std::vector<w_draw_indexed_indirect_command>				drawing_commands;

				W_VK_EXP W_RESULT load(
					_In_ const std::shared_ptr<w_graphics_device>& pGDevice, 
					_In_ const w_command_buffer& pCommandBuffer,
					_In_ const uint32_t& pDrawCount);
			};
		}
	}
}

#endif
