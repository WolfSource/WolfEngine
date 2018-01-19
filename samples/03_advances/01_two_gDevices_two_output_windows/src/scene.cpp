#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::graphics;

scene::scene(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName) :
    w_game(pRunningDirectory, pAppName)
{
    auto _running_dir = pRunningDirectory;

#if defined(__WIN32) || defined(__UWP)
    content_path = _running_dir + L"../../../../content/";
#elif defined(__APPLE__)
    content_path = _running_dir + L"/../../../../../content/";
#elif defined(__linux)
    error
#elif defined(__ANDROID)
    error
#endif

#ifdef __WIN32
    w_graphics_device_manager_configs _config;
    _config.debug_gpu = false;
    w_game::set_graphics_device_manager_configs(_config);
#endif

    w_game::set_fixed_time_step(false);
}

scene::~scene()
{
	//release all resources
	release();
}

void scene::initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
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
		auto _output_window = &(_gDevice->output_presentation_windows[0]);

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

		//initialize attachment buffers
		w_attachment_buffer_desc _color(w_texture_buffer_type::W_TEXTURE_COLOR_BUFFER);
		w_attachment_buffer_desc _depth(w_texture_buffer_type::W_TEXTURE_DEPTH_BUFFER);

		//define color and depth buffers for render pass
		std::vector<w_attachment_buffer_desc> _attachment_descriptions = { _color, _depth };

		//create render pass
		auto _hr = this->_draw_render_pass[i].load(_gDevice,
			_viewport[i],
			_viewport_scissor[i],
			_attachment_descriptions);
		if (_hr == S_FALSE)
		{
			release();
			V(S_FALSE, "creating render pass", _trace_info, 3, true);
		}
		
		//create frame buffers
		auto _draw_render_pass_handle = this->_draw_render_pass[i].get_handle();
		_hr = this->_draw_frame_buffers[i].load(_gDevice,
			_draw_render_pass_handle,
			_output_window);
		if (_hr == S_FALSE)
		{
			release();
			V(S_FALSE, "creating frame buffers", _trace_info, 3, true);
		}

		//create semaphore
		_hr = this->_draw_semaphore[i].initialize(_gDevice);
		if (_hr == S_FALSE)
		{
			release();
			V(S_FALSE, "creating draw semaphore", _trace_info, 3, true);
		}

		//Fence for syncing
		_hr = this->_draw_fence[i].initialize(_gDevice);
		if (_hr == S_FALSE)
		{
			release();
			V(S_FALSE, "creating draw fence", _trace_info, 3, true);
		}

		//create two primary command buffers for clearing screen
		auto _swap_chain_image_size = _output_window->vk_swap_chain_image_views.size();
		_hr = this->_draw_command_buffers[i].load(_gDevice, _swap_chain_image_size);
		if (_hr == S_FALSE)
		{
			release();
			V(S_FALSE, "creating draw command buffers", _trace_info, 3, true);
		}

		_build_draw_command_buffers(i);
	}
}

HRESULT scene::_build_draw_command_buffers(_In_ const size_t& pGraphicsDeviceIndex)
{
	const std::string _trace_info = this->name + "::build_draw_command_buffers";
	HRESULT _hr = S_OK;

	auto _size = this->_draw_command_buffers[pGraphicsDeviceIndex].get_commands_size();
	for (uint32_t i = 0; i < _size; ++i)
	{
		this->_draw_command_buffers[pGraphicsDeviceIndex].begin(i);
		{
			auto _frame_buffer_handle = this->_draw_frame_buffers[pGraphicsDeviceIndex].get_frame_buffer_at(i);

			auto _cmd = this->_draw_command_buffers[pGraphicsDeviceIndex].get_command_at(i);
			this->_draw_render_pass[pGraphicsDeviceIndex].begin(_cmd,
				_frame_buffer_handle,
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

HRESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
	if (w_game::exiting) return S_OK;

	const std::string _trace_info = this->name + "::render";

	for (size_t i = 0; i < this->graphics_devices.size(); ++i)
	{
		auto _gDevice = this->graphics_devices[i];
		auto _output_window = &(_gDevice->output_presentation_windows[0]);
		auto _frame_index = _output_window->vk_swap_chain_image_index;

		//add wait semaphores
		std::vector<VkSemaphore> _wait_semaphors = { *(_output_window->vk_swap_chain_image_is_available_semaphore.get()) };
		auto _draw_command_buffer = this->_draw_command_buffers[i].get_command_at(_frame_index);

		const VkPipelineStageFlags _wait_dst_stage_mask[] =
		{
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		};

		//reset draw fence
		this->_draw_fence[i].reset();
		if (_gDevice->submit(
			{ _draw_command_buffer },
			_gDevice->vk_graphics_queue,
			&_wait_dst_stage_mask[0],
			_wait_semaphors,
			{ *_output_window->vk_rendering_done_semaphore.get() },
			&this->_draw_fence[i]) == S_FALSE)
		{
			V(S_FALSE, "submiting queue for drawing", _trace_info, 3, true);
		}
		this->_draw_fence[i].wait();

		//clear all wait semaphores
		_wait_semaphors.clear();
	}

	return w_game::render(pGameTime);
}

void scene::on_window_resized(_In_ uint32_t pIndex)
{
	w_game::on_window_resized(pIndex);
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
		this->_draw_frame_buffers[i].release();
	}

	return w_game::release();
}
