#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::graphics;

scene::scene(_In_z_ const std::wstring& pContentPath, _In_z_ const std::wstring& pLogPath, _In_z_ const std::wstring& pAppName) :
	w_game(pContentPath, pLogPath, pAppName)
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

void scene::initialize(_In_ std::map<int, w_window_info> pOutputWindowsInfo)
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
		V(W_FAILED, "creating render pass", _trace_info, 3, true);
	}

	//create semaphore create info
	_hr = this->_draw_semaphore.initialize(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "creating draw semaphore", _trace_info, 3, true);
	}

	//Fence for render sync
	_hr = this->_draw_fence.initialize(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "creating draw fence", _trace_info, 3, true);
	}

	auto _swap_chain_image_size = _output_window->swap_chain_image_views.size();
	//create two primary command buffers for clearing screen
	_hr = this->_draw_command_buffers.load(_gDevice, _swap_chain_image_size);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "creating draw command buffers", _trace_info, 3, true);
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	//create two command buffers for render target
	_hr = this->_rt_command_buffer.load(_gDevice, _swap_chain_image_size);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "loading quad command buffers", _trace_info, 3, true);
	}

	_hr = this->_rt.load(
		_gDevice, 
		this->_viewport, 
		this->_viewport_scissor, 
		_render_pass_attachments[0],//color and depth
		_swap_chain_image_size);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "loading render target", _trace_info, 3, true);
	}

	//create semaphore for syncing
	_hr = this->_rt_semaphore.initialize(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "creating quad semaphore", _trace_info, 3, true);
	}

	//create fence for syncing
	_hr = this->_rt_fence.initialize(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "creating quad fence", _trace_info, 3, true);
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef WIN32
	auto _content_path_dir = wolf::system::io::get_current_directoryW() + L"/../../../../samples/02_basics/13_render_to_target/src/content/";
#elif defined(__APPLE__)
	auto _content_path_dir = wolf::system::io::get_current_directoryW() + L"/../../../../../samples/02_basics/13_render_to_target/src/content/";
#endif // WIN32

	//loading vertex shaders
	_hr = this->_shader.load(_gDevice,
		_content_path_dir + L"shaders/shader.vert.spv",
		w_shader_stage_flag_bits::VERTEX_SHADER);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "loading vertex shader", _trace_info, 3, true);
	}

	//loading fragment shader
	_hr = this->_shader.load(_gDevice,
		_content_path_dir + L"shaders/shader.frag.spv",
		w_shader_stage_flag_bits::FRAGMENT_SHADER);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "loading fragment shader", _trace_info, 3, true);
	}

	//load vertex shader uniform
	_hr = this->_u0.load(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "loading vertex shader uniform", _trace_info, 3, true);
	}

	//update uniform's data
	this->_u0.data.wvp = glm::mat4(1);
	_hr = this->_u0.update();
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "updating uniform", _trace_info, 3, true);
	}

	//just we need vertex position color
	this->_mesh.set_vertex_binding_attributes(w_vertex_declaration::VERTEX_POSITION_UV);

	std::vector<w_shader_binding_param> _shader_params;

	w_shader_binding_param _shader_param;
	_shader_param.index = 0;
	_shader_param.type = w_shader_binding_type::SAMPLER2D;
	_shader_param.stage = w_shader_stage_flag_bits::FRAGMENT_SHADER;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	_shader_param.image_info = this->_rt.get_attachment_descriptor_info(0);//attachment 0 is color
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	_shader_params.push_back(_shader_param);

	_shader_param.index = 1;
	_shader_param.type = w_shader_binding_type::UNIFORM;
	_shader_param.stage = w_shader_stage_flag_bits::VERTEX_SHADER;
	_shader_param.buffer_info = this->_u0.get_descriptor_info();
	_shader_params.push_back(_shader_param);

	_hr = this->_shader.set_shader_binding_params(_shader_params);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "setting shader binding param", _trace_info, 3, true);
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

	_hr = this->_mesh.load(_gDevice,
		_vertex_data.data(),
		static_cast<uint32_t>(_vertex_data.size() * sizeof(float)),
		static_cast<uint32_t>(_vertex_data.size()),
		_index_data.data(),
		static_cast<uint32_t>(_index_data.size()));
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "loading mesh", _trace_info, 3, true);
	}

	_build_draw_command_buffers();
}

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
					V(W_FAILED, "drawing mesh", _trace_info, 3, false);
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
	auto _time = pGameTime.get_total_seconds();
	auto _clear_color = w_color();
	_clear_color.r = static_cast<uint8_t>((float)(std::abs(std::sin(_time)) * 255.0f));
	_clear_color.g = static_cast<uint8_t>((float)(std::abs(std::cos(_time)) * 255.0f));
	_clear_color.b = 155;
	_clear_color.a = 255;

	this->_rt.record_command_buffer(&this->_rt_command_buffer, nullptr, _clear_color, 1.0f, 0);
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

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	auto _rt_cmd = this->_rt_command_buffer.get_command_at(_frame_index);

	//reset render target fence
	this->_rt_fence.reset();

	if (_gDevice->submit(
		{ &_rt_cmd },//command buffers
		_gDevice->vk_graphics_queue, //graphics queue
		_wait_dst_stage_mask, //destination masks
		{ _output_window->swap_chain_image_is_available_semaphore }, //wait semaphores
		{ this->_rt_semaphore }, //signal semaphores
		&this->_rt_fence,
		false) == W_FAILED)
	{
		V(W_FAILED, "submiting queue for drawing to render target", _trace_info, 3, true);
	}

	// Wait for fence to signal that command buffer done
	this->_rt_fence.wait();

	auto _draw_cmd = this->_draw_command_buffers.get_command_at(_frame_index);

	//reset draw fence
	this->_draw_fence.reset();
	if (_gDevice->submit(
		{ &_draw_cmd },
		_gDevice->vk_graphics_queue,
		_wait_dst_stage_mask,
		{ this->_rt_semaphore },
		{ _output_window->rendering_done_semaphore },
		&this->_draw_fence,
		false) == W_FAILED)
	{
		V(W_FAILED, "submiting queue for drawing gui", _trace_info, 3, true);
	}
	// Wait for fence to signal main command buffers done their job
	this->_draw_fence.wait();

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	
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

	this->_draw_fence.release();
	this->_draw_semaphore.release();

	this->_draw_command_buffers.release();
	this->_draw_render_pass.release();

	this->_shader.release();
	this->_pipeline.release();

	this->_mesh.release();
	this->_u0.release();

    //++++++++++++++++++++++++++++++++++++++++++++++++++++
    //The following codes have been added for this project
    //++++++++++++++++++++++++++++++++++++++++++++++++++++
	this->_rt_fence.release();
	this->_rt_semaphore.release();
	this->_rt_command_buffer.release();
	this->_rt.release();
    //++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++++++

	return w_game::release();
}
