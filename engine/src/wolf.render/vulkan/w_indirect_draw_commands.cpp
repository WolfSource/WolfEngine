#include "w_render_pch.h"
#include "w_indirect_draw_commands.h"

using namespace wolf::render::vulkan;

W_RESULT w_indirect_draws_command_buffer::load(
	_In_ const std::shared_ptr<w_graphics_device>& pGDevice,
	_In_ const uint32_t& pDrawCount)
{
	const std::string _trace_info = "w_indirect_draws_command_buffer::load";

	if (!this->drawing_commands.size())
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			"empty indirect drawing commands. trace info: {}",
			_trace_info);
		return W_FAILED;
	}
	if (pDrawCount > this->drawing_commands.size())
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			"draw count is greater than indirect drawing commands. trace info: {}",
			_trace_info);
		return W_FAILED;
	}

	w_buffer _staging_buffer;
	defer _(nullptr, [&](...)
	{
		_staging_buffer.release();
	});

	uint32_t _size = (uint32_t)(pDrawCount * sizeof(w_draw_indexed_indirect_command));
	//if (_staging_buffer.allocate_as_staging(pGDevice, _size) == W_FAILED)
	//{
	//	V(W_FAILED, "loading staging buffer of indirect_draw_commands", _trace_info, 3);
	//	return W_FAILED;
	//}

	if (_staging_buffer.allocate(
		pGDevice,
		_size,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		w_memory_usage_flag::MEMORY_USAGE_CPU_ONLY,
		false) == W_FAILED)
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			"loading staging buffer of indirect_draw_commands. trace info: {}",
			_trace_info);
		return W_FAILED;
	}

	if (_staging_buffer.bind() == W_FAILED)
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			"binding to staging buffer of indirect_draw_commands. trace info: {}",
			_trace_info);
		return W_FAILED;
	}

	if (_staging_buffer.set_data(this->drawing_commands.data()) == W_FAILED)
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			"setting data for staging buffer of indirect_draw_commands. trace info: {}",
			_trace_info);
		return W_FAILED;
	}

	//load memory for indirect buffer as seperated memory
	if (this->buffer.allocate(
		pGDevice,
		_size,
		VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		w_memory_usage_flag::MEMORY_USAGE_GPU_ONLY,
		false) == W_FAILED)
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			"loading staging buffer of indirect_commands_buffer. trace info: {}",
			_trace_info);
		return W_FAILED;
	}

	//bind indircet buffer
	if (this->buffer.bind() == W_FAILED)
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			"binding to staging buffer of indirect_commands_buffer. trace info: {}",
			_trace_info);
		return W_FAILED;
	}

	if (_staging_buffer.copy_to(this->buffer) == W_FAILED)
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			"copy staging buffer to device buffer of indirect_commands_buffer. trace info: {}",
			_trace_info);
		return W_FAILED;
	}

	return W_PASSED;
}
