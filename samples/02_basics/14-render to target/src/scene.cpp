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
	_config.debug_gpu = true;
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

	auto _gDevice = this->graphics_devices[0];
	auto _output_window = &(_gDevice->output_presentation_windows[0]);

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

	//initialize buffer attachments
	w_attachment_buffer_desc _color(w_texture_buffer_type::W_TEXTURE_COLOR_BUFFER);
	w_attachment_buffer_desc _depth(w_texture_buffer_type::W_TEXTURE_DEPTH_BUFFER);

	_depth.desc.format = _output_window->vk_depth_buffer_format;

	//define color and depth attachments for render pass
	std::vector<w_attachment_buffer_desc> _attachment_descriptions = { _color, _depth };

	//create render pass
	auto _hr = this->_draw_render_pass.load(_gDevice,
		_viewport,
		_viewport_scissor,
		_attachment_descriptions);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating render pass", _trace_info, 3, true);
	}

	//create frame buffers
	auto _render_pass_handle = this->_draw_render_pass.get_handle();
	_hr = this->_draw_frame_buffers.load(_gDevice,
		_render_pass_handle,
		_output_window);

	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating frame buffers", _trace_info, 3, true);
	}

	//create semaphore create info
	_hr = this->_draw_semaphore.initialize(_gDevice);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating draw semaphore", _trace_info, 3, true);
	}

	//Fence for render sync
	_hr = this->_draw_fence.initialize(_gDevice);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating draw fence", _trace_info, 3, true);
	}

	auto _swap_chain_image_size = _output_window->vk_swap_chain_image_views.size();
	//create two primary command buffers for clearing screen
	_hr = this->_draw_command_buffers.load(_gDevice, _swap_chain_image_size);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating draw command buffers", _trace_info, 3, true);
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	//create two command buffers for render target
	_hr = this->_rt_command_buffer.load(_gDevice, _swap_chain_image_size);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "loading quad command buffers", _trace_info, 3, true);
	}

	_hr = this->_rt.load(_gDevice, this->_viewport, this->_viewport_scissor, _attachment_descriptions, _swap_chain_image_size);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "loading render target", _trace_info, 3, true);
	}

	//create semaphore for syncing
	_hr = this->_rt_semaphore.initialize(_gDevice);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating quad semaphore", _trace_info, 3, true);
	}

	//create fence for syncing
	_hr = this->_rt_fence.initialize(_gDevice);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating quad fence", _trace_info, 3, true);
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

#ifdef WIN32
	auto _content_path_dir = wolf::system::io::get_current_directory() + L"/../../../../samples/02_basics/14-render to target/src/content/";
#elif defined(__APPLE__)
	auto _content_path_dir = wolf::system::io::get_current_directory() + L"/../../../../../samples/02_basics/14-render to target/src/content/";
#endif // WIN32

	//loading vertex shaders
	_hr = this->_shader.load(_gDevice,
		_content_path_dir + L"shaders/shader.vert.spv",
		w_shader_stage::VERTEX_SHADER);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "loading vertex shader", _trace_info, 3, true);
	}

	//loading fragment shader
	_hr = this->_shader.load(_gDevice,
		_content_path_dir + L"shaders/shader.frag.spv",
		w_shader_stage::FRAGMENT_SHADER);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "loading fragment shader", _trace_info, 3, true);
	}

	//load vertex shader uniform
	_hr = this->_u0.load(_gDevice);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "loading vertex shader uniform", _trace_info, 3, true);
	}

	//update uniform's data
	this->_u0.data.wvp = glm::mat4(1);
	_hr = this->_u0.update();
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "updating uniform", _trace_info, 3, true);
	}

	//just we need vertex position color
	this->_mesh.set_vertex_binding_attributes(w_vertex_declaration::VERTEX_POSITION_UV);

	std::vector<w_shader_binding_param> _shader_params;

	w_shader_binding_param _shader_param;
	_shader_param.index = 0;
	_shader_param.type = w_shader_binding_type::SAMPLER;
	_shader_param.stage = w_shader_stage::FRAGMENT_SHADER;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	_shader_param.image_info = this->_rt.get_attachment_descriptor_info(0);//attachment 0 is color
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	_shader_params.push_back(_shader_param);

	_shader_param.index = 1;
	_shader_param.type = w_shader_binding_type::UNIFORM;
	_shader_param.stage = w_shader_stage::VERTEX_SHADER;
	_shader_param.buffer_info = this->_u0.get_descriptor_info();
	_shader_params.push_back(_shader_param);

	_hr = this->_shader.set_shader_binding_params(_shader_params);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "setting shader binding param", _trace_info, 3, true);
	}

	//loading pipeline cache
	std::string _pipeline_cache_name = "pipeline_cache";
	if (w_pipeline::create_pipeline_cache(_gDevice, _pipeline_cache_name) == S_FALSE)
	{
		logger.error("could not create pipeline cache");
		_pipeline_cache_name.clear();
	}

	auto _descriptor_set_layout_binding = this->_shader.get_descriptor_set_layout();
	_hr = this->_pipeline.load(_gDevice,
		this->_mesh.get_vertex_binding_attributes(),
		VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		this->_draw_render_pass.get_handle(),
		this->_shader.get_shader_stages(),
		_descriptor_set_layout_binding ? &_descriptor_set_layout_binding : nullptr,
		{ this->_viewport },
		{ this->_viewport_scissor });

	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating pipeline", _trace_info, 3, true);
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

	std::vector<uint32_t> _index_data =
	{
		0,
		1,
		3,
		3,
		1,
		2
	};

	_hr = this->_mesh.load(_gDevice,
		_vertex_data.data(),
		static_cast<uint32_t>(_vertex_data.size() * sizeof(float)),
		static_cast<uint32_t>(_vertex_data.size()),
		_index_data.data(),
		static_cast<uint32_t>(_index_data.size()));
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "loading mesh", _trace_info, 3, true);
	}

	build_draw_command_buffers(_gDevice);
}

HRESULT scene::build_draw_command_buffers(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
	const std::string _trace_info = this->name + "::build_draw_command_buffers";
	HRESULT _hr = S_OK;

	auto _size = this->_draw_command_buffers.get_commands_size();
	for (uint32_t i = 0; i < _size; ++i)
	{
		this->_draw_command_buffers.begin(i);
		{
			auto _frame_buffer_handle = this->_draw_frame_buffers.get_frame_buffer_at(i);

			auto _cmd = this->_draw_command_buffers.get_command_at(i);
			this->_draw_render_pass.begin(_cmd,
				_frame_buffer_handle,
				w_color::CORNFLOWER_BLUE(),
				1.0f,
				0.0f);
			{
                auto _description_set = this->_shader.get_descriptor_set();
                this->_pipeline.bind(_cmd, &_description_set);
				_hr = this->_mesh.draw(_cmd, nullptr, 0, false);
				if (_hr == S_FALSE)
				{
					V(S_FALSE, "drawing mesh", _trace_info, 3, false);
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
	_clear_color.r = static_cast<uint8_t>((float)(std::sin(_time)) * 255.0f);
	_clear_color.g = static_cast<uint8_t>((float)(std::cos(_time)) * 255.0f);
	_clear_color.b = 155;
	_clear_color.a = 255;

	this->_rt.record_command_buffer(&this->_rt_command_buffer, nullptr, _clear_color, 1.0f, 0);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	w_game::update(pGameTime);
}

HRESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
	if (w_game::exiting) return S_OK;

	const std::string _trace_info = this->name + "::render";

	auto _gDevice = this->graphics_devices[0];
	auto _output_window = &(_gDevice->output_presentation_windows[0]);
	auto _frame_index = _output_window->vk_swap_chain_image_index;

	//add wait semaphores
	std::vector<VkSemaphore> _wait_semaphors = { *(_output_window->vk_swap_chain_image_is_available_semaphore.get()) };
	const VkPipelineStageFlags _wait_dst_stage_mask[] =
	{
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	};

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	auto _cmd = this->_rt_command_buffer.get_command_at(_frame_index);
	
	//reset render target fence
	this->_rt_fence.reset();
	
	//submit command buffer of render target
	if (_gDevice->submit(
		{ _cmd },
		_gDevice->vk_graphics_queue,
		&_wait_dst_stage_mask[0],
		_wait_semaphors,
		{ *this->_rt_semaphore.get() },
		this->_rt_fence) == S_FALSE)
	{
		V(S_FALSE, "submiting queue for drawing quad", _trace_info, 3, true);
	}
	// Wait for fence to signal that command buffer done
	this->_rt_fence.wait();

	 _cmd = this->_draw_command_buffers.get_command_at(_frame_index);
	//reset draw fence
	this->_draw_fence.reset();
	if (_gDevice->submit(
		{ _cmd },
		_gDevice->vk_graphics_queue,
		&_wait_dst_stage_mask[0],
		{ *this->_rt_semaphore.get() },
		{ *_output_window->vk_rendering_done_semaphore.get() },
		this->_draw_fence) == S_FALSE)
	{
		V(S_FALSE, "submiting queue for drawing gui", _trace_info, 3, true);
	}
	// Wait for fence to signal main command buffers done their job
	this->_draw_fence.wait();

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	//clear all wait semaphores
	_wait_semaphors.clear();

    auto _hr = w_game::render(pGameTime);

    uint8_t* _data = nullptr;
    auto _result = _gDevice->capture(0, &_data);

    return _hr;
}

void scene::on_window_resized(_In_ UINT pIndex)
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

	this->_draw_fence.release();
	this->_draw_semaphore.release();

	this->_draw_command_buffers.release();
	this->_draw_render_pass.release();
	this->_draw_frame_buffers.release();

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
