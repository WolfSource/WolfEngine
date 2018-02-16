/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_command_buffer
	Description		 : Command buffers
	Comment          : 
*/
#ifndef __W_COMMAND_BUFFER_H__
#define __W_COMMAND_BUFFER_H__

#include <w_graphics_headers.h>
#include <w_render_export.h>

namespace wolf
{
	namespace graphics
	{
		class w_command_buffer_pimp;
		class w_command_buffer : public system::w_object
		{
		public:
			W_EXP w_command_buffer();
			W_EXP ~w_command_buffer();

			/*
				pGDevice = graphices device,
				pCount = count of command buffers to be created,
				pLevel = primary or secondary level of thread execution
				pCreateCommandPool = create seperated command pool,
				pCommandPoolQueue = if pCreateCommandPool set true, then use this w_queue_index for creating command pool
			*/
			W_EXP W_RESULT load(
				_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
				_In_ const size_t& pCount,
				_In_ const w_command_buffer_level& pLevel = w_command_buffer_level::PRIMARY,
				_In_ const bool& pCreateCommandPool = false,
				_In_ const w_queue* pCommandPoolQueue = nullptr);

			//begin command buffer
			W_EXP W_RESULT begin(_In_ const size_t& pCommandBufferIndex,
				_In_ const w_command_buffer_usage_flags pFlags = w_command_buffer_usage_flag_bits::SIMULTANEOUS_USE_BIT);

			//end command buffer
			W_EXP W_RESULT end(_In_ const size_t& pCommandBufferIndex);

			//Flushing the command buffer will also submit it to the queue and uses a fence to ensure that command has been executed before returning
			W_EXP W_RESULT flush(_In_ const size_t& pCommandBufferIndex);

			//Flushing all command buffers will also submit these commands buffers to the queue and uses a fence to ensure that all commands have been executed before returning
			W_EXP W_RESULT flush_all();

			//release all resources
			W_EXP ULONG release() override;

#pragma region Getters

			W_EXP const VkCommandBuffer* get_commands() const;
			W_EXP const VkCommandBuffer get_command_at(_In_ const size_t& pIndex) const;
			W_EXP const VkCommandBuffer get_active_command() const;
			W_EXP const size_t get_commands_size() const;

#pragma endregion

#pragma region Setters

			W_EXP void set_active_command(_In_ const uint32_t& pindex);

#pragma endregion


#ifdef __PYTHON__
			W_EXP bool py_load(
				_In_ boost::shared_ptr<wolf::graphics::w_graphics_device>& pGDevice,
				_In_ const size_t& pCount,
				_In_ const w_command_buffer_level& pLevel);
			W_EXP bool py_begin(_In_ const size_t& pCommandBufferIndex, _In_ const w_command_buffer_usage_flags pFlags);
			W_EXP bool py_end(_In_ const size_t& pCommandBufferIndex);
			W_EXP bool py_flush(_In_ const size_t& pCommandBufferIndex);
			W_EXP bool py_flush_all();
#endif

		private:
			typedef system::w_object            _super;
			w_command_buffer_pimp*              _pimp;
		};

	}
}

#include "python_exporter/py_command_buffer.h"

#endif
