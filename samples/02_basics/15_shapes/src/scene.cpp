#include "pch.h"
#include "scene.h"
#include <glm_extension.h>

using namespace std;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::graphics;

static uint32_t sFPS = 0;
static float sElapsedTimeInSec = 0;
static float sTotalTimeTimeInSec = 0;

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
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"creating render pass. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}

    //create semaphore
    _hr = this->_draw_semaphore.initialize(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"creating draw semaphore. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}

    //Fence for syncing
    _hr = this->_draw_fence.initialize(_gDevice);
    if (_hr == W_FAILED)
    {
        release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"creating draw fence. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
    }

	//load imgui
	w_imgui::load(
		_gDevice,
		_output_window,
		this->_viewport,
		this->_viewport_scissor,
		nullptr);

    //create two primary command buffers for clearing screen
    auto _swap_chain_image_size = _output_window->swap_chain_image_views.size();
    _hr = this->_draw_command_buffers.load(_gDevice, _swap_chain_image_size);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"creating draw command buffers. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	//Add Line
	this->_shape_line = new (std::nothrow) w_shapes(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(3.0f, 3.0f, 3.0f),
		w_color::RED());
	if (!this->_shape_line)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"allocating memory for shape(line). graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}
	_hr = this->_shape_line->load(_gDevice, this->_draw_render_pass, this->_viewport, this->_viewport_scissor);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"loading shape(line). graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}

	//Add Triangle
	this->_shape_triangle = new (std::nothrow) w_shapes(
		glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 2.0f, 0.0f),
		w_color::GREEN());
	if (!this->_shape_triangle)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"allocating memory for shape(triangle). graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}
	_hr = this->_shape_triangle->load(_gDevice, this->_draw_render_pass, this->_viewport, this->_viewport_scissor);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"loading shape(triangle). graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}

	//Add Circle
	this->_shape_circle = new (std::nothrow) w_shapes(
		glm::vec3(0.0f, 0.0f, 0.0f),
		2.0f,
		w_color::ORANGE(),
		w_plane::XY, 
		30.0f);
	if (!this->_shape_circle)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"allocating memory for shape(circle). graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}
	_hr = this->_shape_circle->load(_gDevice, this->_draw_render_pass, this->_viewport, this->_viewport_scissor);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"loading shape(circle). graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}

	//Add Bounding Box
	w_bounding_box _bounding_box;

	_bounding_box.min[0] = -3.0f;
	_bounding_box.min[1] = -3.0f;
	_bounding_box.min[2] = -3.0f;

	_bounding_box.max[0] = 3.0f;
	_bounding_box.max[1] = 3.0f;
	_bounding_box.max[2] = 3.0f;

	this->_shape_box = new (std::nothrow) w_shapes(_bounding_box, w_color::YELLOW());
	if (!this->_shape_box)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"allocating memory for shape(box). graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}
	_hr = this->_shape_box->load(_gDevice, this->_draw_render_pass, this->_viewport, this->_viewport_scissor);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"loading shape(box). graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}

	//Add Bounding Sphere
	w_bounding_sphere _bounding_sphere;

	_bounding_sphere.center[0] = 0.0f;
	_bounding_sphere.center[1] = 0.0f;
	_bounding_sphere.center[2] = 0.0f;

	_bounding_sphere.radius = 3.0f;

	const uint32_t _sphere_resolution = 30;
	this->_shape_sphere = new (std::nothrow) w_shapes(_bounding_sphere, w_color::PURPLE(), _sphere_resolution);
	if (!this->_shape_sphere)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"allocating memory for shape(sphere). graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}
	_hr = this->_shape_sphere->load(_gDevice, this->_draw_render_pass, this->_viewport, this->_viewport_scissor);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"loading shape(sphere). graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

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
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				//The following codes have been added for this project
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				this->_shape_line->draw(_cmd);
				this->_shape_triangle->draw(_cmd);
				this->_shape_circle->draw(_cmd);
				this->_shape_box->draw(_cmd);
				this->_shape_sphere->draw(_cmd);
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
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

    sFPS = pGameTime.get_frames_per_second();
    sElapsedTimeInSec = pGameTime.get_elapsed_seconds();
    sTotalTimeTimeInSec = pGameTime.get_total_seconds();

    w_imgui::new_frame(sElapsedTimeInSec, [this]()
    {
        _update_gui();
    });
    
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	auto _angle = (float)pGameTime.get_total_seconds();
	auto _eye = glm::vec3((float)std::cos(_angle * 0.5f), 0.5f, (float)std::sin(_angle * 0.5f)) * 15.0f;
	auto _up = glm::vec3(0, -1, 0);
	auto _look_at = glm::vec3(0, 0, 0);

	auto _world = glm::mat4(1);//identity
	auto _view = glm::lookAtRH(_eye, _look_at, _up);
	auto _projection = glm::perspectiveRH(
		45.0f * glm::pi<float>() / 180.0f,
		this->_viewport.width / this->_viewport.height, 
		0.1f, 
		1000.0f);

	auto _wvp = _projection * _view * _world;

	this->_shape_line->update(_wvp);
	this->_shape_triangle->update(_wvp);
	this->_shape_circle->update(_wvp);
	this->_shape_box->update(_wvp);
	this->_shape_sphere->update(_wvp);
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

	w_imgui::render();

	auto _draw_cmd = this->_draw_command_buffers.get_command_at(_frame_index);
	auto _gui_cmd = w_imgui::get_command_buffer_at(_frame_index);

	const std::vector<w_pipeline_stage_flag_bits> _wait_dst_stage_mask =
	{
		w_pipeline_stage_flag_bits::COLOR_ATTACHMENT_OUTPUT_BIT,
	};

	//reset draw fence
	this->_draw_fence.reset();
	if (_gDevice->submit(
		{ &_draw_cmd, &_gui_cmd },//command buffers
		_gDevice->vk_graphics_queue, //graphics queue
		_wait_dst_stage_mask, //destination masks
		{ _output_window->swap_chain_image_is_available_semaphore }, //wait semaphores
		{ _output_window->rendering_done_semaphore }, //signal semaphores
		&this->_draw_fence,
		false) == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"submiting queue for drawing. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}
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

    //release draw's objects
	this->_draw_fence.release();
	this->_draw_semaphore.release();

	this->_draw_command_buffers.release();
	this->_draw_render_pass.release();

    //release gui's objects
    w_imgui::release();

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	
	SAFE_RELEASE(this->_shape_line);
	SAFE_RELEASE(this->_shape_triangle);
	SAFE_RELEASE(this->_shape_circle);
	SAFE_RELEASE(this->_shape_box);
	SAFE_RELEASE(this->_shape_sphere);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	return w_game::release();
}

bool scene::_update_gui()
{
    //Setting Style
    ImGuiStyle& _style = ImGui::GetStyle();
    _style.Colors[ImGuiCol_Text].x = 1.0f;
    _style.Colors[ImGuiCol_Text].y = 1.0f;
    _style.Colors[ImGuiCol_Text].z = 1.0f;
    _style.Colors[ImGuiCol_Text].w = 1.0f;

    _style.Colors[ImGuiCol_WindowBg].x = 0.0f;
    _style.Colors[ImGuiCol_WindowBg].y = 0.4f;
    _style.Colors[ImGuiCol_WindowBg].z = 1.0f;
    _style.Colors[ImGuiCol_WindowBg].w = 0.9f;

    ImGuiWindowFlags  _window_flags = 0;;
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiSetCond_FirstUseEver);
    bool _is_open = true;
    if (!ImGui::Begin("Wolf.Engine", &_is_open, _window_flags))
    {
        ImGui::End();
        return false;
    }

    ImGui::Text("Press Esc to exit\r\nFPS:%d\r\nFrameTime:%f\r\nTotalTime:%f\r\nMouse Position:%d,%d\r\n",
        sFPS,
        sElapsedTimeInSec,
        sTotalTimeTimeInSec,
        wolf::inputs_manager.mouse.pos_x, wolf::inputs_manager.mouse.pos_y);

    ImGui::End();

    return true;
}
