#include "pch.h"
#include "scene.h"
#include <w_task.h>
#include <tbb/parallel_for.h>

using namespace std;
using namespace wolf;
using namespace wolf::framework;
using namespace wolf::system;
using namespace wolf::graphics;

scene::scene(_In_z_ const std::wstring& pContentPath, _In_ const wolf::system::w_logger_config& pLogConfig) :
	w_game(pContentPath, pLogConfig)
{
	w_graphics_device_manager_configs _config;
	_config.debug_gpu = false;
	w_game::set_graphics_device_manager_configs(_config);

	w_game::set_fixed_time_step(false);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	w_media_core::register_all(false);
	
	this->_max_buffering_frames = 3;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
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

	auto _gDevice = this->graphics_devices[0];
	auto _output_window = &(_gDevice->output_presentation_window);

	if (this->_media_core.open_media(content_path + L"media/Snow_Monkeys_in_Japan_5K_30sec.MKV") == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			L"opening media from following path {}media/Snow_Monkeys_in_Japan_5K_30sec.MKV",
			content_path);
	}
	this->_media_time.set_fixed_time_step(true);
	this->_media_time.set_target_elapsed_seconds(1.0f / this->_media_core.get_video_frame_rate());

	//allocate memory for buffering, we need 3 frames
	auto _size = 
		this->_max_buffering_frames * /*3 frames*/
		4 * /*RGBA*/
		this->_media_core.get_video_frame_width() * /*frame width*/
		this->_media_core.get_video_frame_height() * /*frame height*/
		sizeof(uint8_t);
	if (this->_media_memory.alloc(_size) == nullptr)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"allocating memory for media buffering. trace infor: {}", _trace_info);
	}

	w_point_t _screen_size;
	_screen_size.x = _output_window->width;
	_screen_size.y = _output_window->height;

	//initialize viewport
	this->_viewport.y = 0;
	this->_viewport.width = static_cast<float>(_screen_size.x);
	this->_viewport.height = static_cast<float>(_screen_size.y);
	this->_viewport.minDepth = 0;
	this->_viewport.maxDepth = 1;

	//initialize scissor of viewport
	this->_viewport_scissor.offset.x = 0;
	this->_viewport_scissor.offset.y = 0;
	this->_viewport_scissor.extent.width = _screen_size.x;
	this->_viewport_scissor.extent.height = _screen_size.y;

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
	auto _hr = this->_draw_render_pass.load(
		_gDevice,
		_viewport,
		_viewport_scissor,
		_render_pass_attachments);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"creating render pass. trace info: {}", _trace_info);
	}

	//create semaphore create info
	_hr = this->_draw_semaphore.initialize(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"creating draw semaphore. trace info: {}", _trace_info);
	}

	//Fence for render sync
	_hr = this->_draw_fence.initialize(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"creating draw fence. trace info: {}", _trace_info);
	}

	//create two primary command buffers for clearing screen
	auto _swap_chain_image_size = _output_window->swap_chain_image_views.size();
	_hr = this->_draw_command_buffers.load(_gDevice, _swap_chain_image_size);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"creating draw command buffers. trace info: {}", _trace_info);
	}

#ifdef WIN32
	auto _content_path_dir = wolf::system::io::get_current_directoryW() + L"/../../../../samples/02_basics/05_texture/src/content/";
#elif defined(__APPLE__)
	auto _content_path_dir = wolf::system::io::get_current_directoryW() + L"/../../../../../samples/02_basics/05_texture/src/content/";
#endif // WIN32

	//loading vertex shaders
	_hr = this->_shader.load(_gDevice,
		_content_path_dir + L"shaders/shader.vert.spv",
		w_shader_stage_flag_bits::VERTEX_SHADER);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"loading vertex shader. trace info: {}", _trace_info);
	}

	//loading fragment shader
	_hr = this->_shader.load(_gDevice,
		_content_path_dir + L"shaders/shader.frag.spv",
		w_shader_stage_flag_bits::FRAGMENT_SHADER);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"loading fragment shader. trace info: {}", _trace_info);
	}

	//load texture as staging buffer
	_hr = this->_texture.initialize(_gDevice,
		this->_media_core.get_video_frame_width(),
		this->_media_core.get_video_frame_height(),
		false,
		true);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"loading staging texture. trace info: {}", _trace_info);
	}
	//load texture from memory
	_hr = this->_texture.load_texture_from_memory_color(w_color::CYAN());
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"initializing staging texture buffer. trace info: {}", _trace_info);
	}

	//just we need vertex position color
	this->_mesh.set_vertex_binding_attributes(w_vertex_declaration::VERTEX_POSITION_UV);
	w_shader_binding_param _shader_param;
	_shader_param.index = 0;
	_shader_param.type = w_shader_binding_type::SAMPLER2D;
	_shader_param.stage = w_shader_stage_flag_bits::FRAGMENT_SHADER;
	_shader_param.image_info = this->_texture.get_descriptor_info();

	_hr = this->_shader.set_shader_binding_params(
		{
			_shader_param
		});
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"setting shader binding param. trace info: {}", _trace_info);
	}

	//loading pipeline cache
	std::string _pipeline_cache_name = "pipeline_cache";
	if (w_pipeline::create_pipeline_cache(_gDevice, _pipeline_cache_name) == W_FAILED)
	{
		logger.error("could not create pipeline cache");
		_pipeline_cache_name.clear();
	}

	_hr = this->_pipeline.load(_gDevice,
		this->_mesh.get_vertex_binding_attributes(),
		w_primitive_topology::TRIANGLE_LIST,
		&this->_draw_render_pass,
		&this->_shader,
		{ this->_viewport },
		{ this->_viewport_scissor });
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "creating pipeline", _trace_info, 3, true);
	}

	std::vector<float> _vertex_data =
	{
		-0.7f, -0.7f,	0.0f,		//pos0
		 0.0f,  0.0f,               //uv0
		-0.7f,  0.7f,	0.0f,		//pos1
		 0.0f,  1.0f,               //uv1
		 0.7f,  0.7f,	0.0f,		//pos2
		 1.0f,  1.0f,           	//uv2
		 0.7f, -0.7f,	0.0f,		//pos3
		 1.0f,  0.0f,               //uv3
	};

	std::vector<uint32_t> _index_data = { 0, 1, 3, 3, 1, 2 };

	this->_mesh.set_texture(&this->_texture);
	_hr = this->_mesh.load(_gDevice,
		_vertex_data.data(),
		static_cast<uint32_t>(_vertex_data.size() * sizeof(float)),
		static_cast<uint32_t>(_vertex_data.size()),
		_index_data.data(),
		static_cast<uint32_t>(_index_data.size()));
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"loading mesh. trace info: {}", _trace_info);
	}

	_build_draw_command_buffers();

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	_buffer_video(this->_max_buffering_frames);
	//run media thread
	w_task::execute_async([this]()->W_RESULT
	{
		_run_video_buffering_thread();
		return W_PASSED;
	});
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++
void scene::_run_video_buffering_thread()
{
	while (true)
	{
		auto _hr = S_FALSE;
		//Lock this section and wait for signal to start buffering
		{
			tbb::interface5::unique_lock<tbb::mutex> _lk(this->_media_buffer_mutex);
			//wait for signal from main thread to start buffering another chuck
			this->_media_signal_slot.wait(_lk);

			if (w_game::exiting) break;

			_buffer_video(this->_max_buffering_frames - 1);
		}
	}

	this->_media_thread_exited.notify_all();
}

void scene::_buffer_video(_In_ const size_t& pNumberOfFrames)
{
	for (size_t i = 0; i < pNumberOfFrames; ++i)
	{
		if (this->_media_core.write_video_frame_to_buffer(this->_media_memory, 1, false) == W_FAILED)
		{
			logger.write("Error on media buffering");
		}
	}
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++

W_RESULT scene::_build_draw_command_buffers()
{
	const std::string _trace_info = this->name + "::build_draw_command_buffers";
	W_RESULT _hr = W_PASSED;

	auto _gDevice = this->get_graphics_device(0);
	auto _size = this->_draw_command_buffers.get_commands_size();
	
	for (uint32_t i = 0; i < _size; ++i)
	{
		auto _cmd = this->_draw_command_buffers.get_command_at(i);
		this->_draw_command_buffers.begin(i);
		{
			this->_draw_render_pass.begin(
				i,
				_cmd,
				w_color::CORNFLOWER_BLUE(),
				1.0f,
				0.0f);
			{
				this->_pipeline.bind(_cmd, w_pipeline_bind_point::GRAPHICS);
				_hr = this->_mesh.draw(_cmd, nullptr, 0, false);
				if (_hr == W_FAILED)
				{
					V(W_FAILED,
						w_log_type::W_ERROR,
						"drawing mesh. trace info: {}", _trace_info);
				}
			}
			this->_draw_render_pass.end(_cmd);
		}
		this->_draw_command_buffers.end(i);
	}
	return _hr;
}

void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
	if (w_game::exiting) return;
	const std::string _trace_info = this->name + "::update";

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	this->_media_time.tick([&]()
	{
		const size_t _size_in_bytes = 4 /*RGBA*/ * sizeof(uint8_t) * this->_texture.get_width() * this->_texture.get_height();
		auto _frames = (uint8_t*)this->_media_memory.get_start_ptr();
		auto _pixels = (uint8_t*)this->_texture.read_data_of_texture();
		if (_frames && _pixels)
		{
			std::memcpy(&_pixels[0], &_frames[this->_media_memory.read_index], _size_in_bytes);
			this->_texture.flush_staging_data();

			auto _delta = std::abs((int)this->_media_memory.write_index - (int)_size_in_bytes);
			if (this->_media_memory.read_index == _delta)
			{
				//notify for buffering
				this->_media_signal_slot.notify_one();
			}
			this->_media_memory.read_index = (this->_media_memory.read_index + _size_in_bytes) % this->_media_memory.get_size_in_bytes();
		}
	});
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	w_game::update(pGameTime);
}

W_RESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
	if (w_game::exiting) return W_PASSED;

	const std::string _trace_info = this->name + "::render";

	auto _gDevice = this->graphics_devices[0];
	auto _output_window = &(_gDevice->output_presentation_window);
	auto _frame_index = _output_window->swap_chain_image_index;

	const std::vector<w_pipeline_stage_flag_bits> _wait_dst_stage_mask =
	{
		w_pipeline_stage_flag_bits::COLOR_ATTACHMENT_OUTPUT_BIT,
	};

	//set active command buffer
	auto _cmd = this->_draw_command_buffers.get_command_at(_frame_index);
	//reset draw fence
	this->_draw_fence.reset();
	if (_gDevice->submit(
		{ &_cmd },//command buffers
		_gDevice->vk_graphics_queue, //graphics queue
		_wait_dst_stage_mask, //destination masks
		{ _output_window->swap_chain_image_is_available_semaphore }, //wait semaphores
		{ _output_window->rendering_done_semaphore }, //signal semaphores
		&this->_draw_fence,
		false) == W_FAILED)
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"submiting queue for drawing gui. trace info: {}", _trace_info);
	}
	// Wait for fence to signal that all command buffers are ready
	this->_draw_fence.wait();
	
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

	w_game::exiting = true;

	this->_draw_fence.release();
	this->_draw_semaphore.release();

	this->_draw_command_buffers.release();
	this->_draw_render_pass.release();

	this->_shader.release();
	this->_pipeline.release();

	this->_mesh.release();
    this->_texture.release();

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	{
		tbb::interface5::unique_lock<tbb::mutex> _lk(this->_media_buffer_mutex);

		//signal video thread
		this->_media_signal_slot.notify_all();
		//wait for exiting thread
		this->_media_thread_exited.wait(_lk);
		//realease all media resources
		this->_media_core.release();
		this->_media_memory.release();
		w_media_core::shut_down();
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	return w_game::release();
}
