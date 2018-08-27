#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::render::vulkan;

scene::scene(_In_z_ const std::wstring& pContentPath, _In_ const wolf::system::w_logger_config& pLogConfig) :
    w_game(pContentPath, pLogConfig)
{
	w_graphics_device_manager_configs _config;
	_config.debug_gpu = false;
	w_game::set_graphics_device_manager_configs(_config);

	w_game::set_fixed_time_step(false);
}

scene::~scene()
{
	//release all resources
	release();
}

void scene::initialize(_In_ std::map<int, w_present_info> pOutputWindowsInfo)
{
	// TODO: Add your pre-initialization logic here

	w_game::initialize(pOutputWindowsInfo);
}

void scene::load()
{
    defer(nullptr, [&](...)
    {
        w_game::load();
    });

    const std::string _trace_info = this->name + "::load";

	for (size_t i = 0; i < this->graphics_devices.size(); ++i)
	{
		auto _gDevice = this->graphics_devices[i];
		auto _output_window = &(_gDevice->output_presentation_window);

		w_point_t _screen_size;
		_screen_size.x = _output_window->width;
		_screen_size.y = _output_window->height;

		//initialize viewport
		this->_viewport[i].y = 0;
		this->_viewport[i].width = static_cast<float>(_screen_size.x);
		this->_viewport[i].height = static_cast<float>(_screen_size.y);
		this->_viewport[i].minDepth = 0;
		this->_viewport[i].maxDepth = 1;

		//initialize scissor of viewport
		this->_viewport_scissor[i].offset.x = 0;
		this->_viewport_scissor[i].offset.y = 0;
		this->_viewport_scissor[i].extent.width = _screen_size.x;
		this->_viewport_scissor[i].extent.height = _screen_size.y;

		//define color and depth as an attachments buffers for render pass
		std::vector<std::vector<w_image_view>> _render_pass_attachments;
		for (size_t i = 0; i < _output_window->swap_chain_image_views.size(); ++i)
		{
			_render_pass_attachments.push_back
			(
				//COLOR									   , DEPTH
				{ _output_window->swap_chain_image_views[i], _output_window->depth_buffer_image_view }
			);
		}

		//create render pass
		w_point _offset;
		_offset.x = this->_viewport[i].x;
		_offset.y = this->_viewport[i].y;

		w_point_t _size;
		_size.x = this->_viewport[i].width;
		_size.y = this->_viewport[i].height;

		auto _hr = this->_draw_render_pass[i].load(
			_gDevice,
			_offset,
			_size,
			_render_pass_attachments);
		if (_hr == W_FAILED)
		{
			release();
			V(W_FAILED,
				w_log_type::W_ERROR,
				true,
				"creating render pass. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
		}

		//create semaphore
		_hr = this->_draw_semaphore[i].initialize(_gDevice);
		if (_hr == W_FAILED)
		{
			release();
			V(W_FAILED,
				w_log_type::W_ERROR,
				true,
				"creating draw semaphore. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
		}

		//Fence for syncing
		_hr = this->_draw_fence[i].initialize(_gDevice);
		if (_hr == W_FAILED)
		{
			release();
			V(W_FAILED,
				w_log_type::W_ERROR,
				true,
				"creating draw fence. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
		}

		//create two primary command buffers for clearing screen
		auto _swap_chain_image_size = _output_window->swap_chain_image_views.size();
		_hr = this->_draw_command_buffers[i].load(_gDevice, _swap_chain_image_size);
		if (_hr == W_FAILED)
		{
			release();
			V(W_FAILED,
				w_log_type::W_ERROR,
				true,
				"creating draw command buffers. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
		}

		_build_draw_command_buffers(i);
	}
}

W_RESULT scene::_build_draw_command_buffers(_In_ const size_t& pGraphicsDeviceIndex)
{
	const std::string _trace_info = this->name + "::build_draw_command_buffers";
	W_RESULT _hr = W_PASSED;

	auto _size = this->_draw_command_buffers[pGraphicsDeviceIndex].get_commands_size();
	for (uint32_t i = 0; i < _size; ++i)
	{
		auto _cmd = this->_draw_command_buffers[pGraphicsDeviceIndex].get_command_at(i);
		this->_draw_command_buffers[pGraphicsDeviceIndex].begin(i);
		{
			this->_draw_render_pass[pGraphicsDeviceIndex].begin(
				i,
				_cmd,
				pGraphicsDeviceIndex == 0 ? w_color::CORNFLOWER_BLUE() : w_color::PURPLE(),
				1.0f,
				0.0f);
			{
			}
			this->_draw_render_pass[pGraphicsDeviceIndex].end(_cmd);
		}
		this->_draw_command_buffers[pGraphicsDeviceIndex].end(i);
	}
	return _hr;
}

void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
	if (w_game::exiting) return;
	const std::string _trace_info = this->name + "::update";
   
	w_game::update(pGameTime);
}

W_RESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
	if (w_game::exiting) return W_PASSED;

	const std::string _trace_info = this->name + "::render";

	for (size_t i = 0; i < this->graphics_devices.size(); ++i)
	{
		auto _gDevice = this->graphics_devices[i];
		auto _output_window = &(_gDevice->output_presentation_window);
		auto _frame_index = _output_window->swap_chain_image_index;

		auto _draw_cmd = this->_draw_command_buffers[i].get_command_at(_frame_index);

		const std::vector<w_pipeline_stage_flag_bits> _wait_dst_stage_mask =
		{
			w_pipeline_stage_flag_bits::COLOR_ATTACHMENT_OUTPUT_BIT,
		};

		//reset draw fence
		this->_draw_fence[i].reset();
		if (_gDevice->submit(
			{ &_draw_cmd },//command buffers
			_gDevice->vk_graphics_queue, //graphics queue
			_wait_dst_stage_mask, //destination masks
			{ _output_window->swap_chain_image_is_available_semaphore }, //wait semaphores
			{ _output_window->rendering_done_semaphore }, //signal semaphores
			&this->_draw_fence[i],
			false) == W_FAILED)
		{
			release();
			V(W_FAILED,
				w_log_type::W_ERROR,
				true,
				"submiting queue for drawing. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
		}
		this->_draw_fence[i].wait();
	}

	return w_game::render(pGameTime);
}

void scene::on_window_resized(_In_ const uint32_t& pIndex, _In_ const w_point& pNewSizeOfWindow)
{
	w_game::on_window_resized(pIndex, pNewSizeOfWindow);
}

void scene::on_device_lost()
{
	w_game::on_device_lost();
}

ULONG scene::release()
{
    if (this->get_is_released()) return 1;

    //release draw's objects
	for (size_t i = 0; i < this->graphics_devices.size(); ++i)
	{
		if (this->graphics_devices[i] == nullptr) continue;
		
		this->_draw_fence[i].release();
		this->_draw_semaphore[i].release();

		this->_draw_command_buffers[i].release();
		this->_draw_render_pass[i].release();
	}

	return w_game::release();
}
