#include "pch.h"
#include "scene.h"
#include <w_content_manager.h>
#include <glm_extension.h>

using namespace std;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

static uint32_t sFPS = 0;
static float sElapsedTimeInSec = 0;
static float sTotalTimeTimeInSec = 0;

scene::scene(_In_z_ const std::wstring& pContentPath, _In_z_ const std::wstring& pLogPath, _In_z_ const std::wstring& pAppName) :
	w_game(pContentPath, pLogPath, pAppName)
{
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

	//create semaphore
	_hr = this->_draw_semaphore.initialize(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "creating draw semaphore", _trace_info, 3, true);
	}

	//Fence for syncing
	_hr = this->_draw_fence.initialize(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "creating draw fence", _trace_info, 3, true);
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
		V(W_FAILED, "creating draw command buffers", _trace_info, 3, true);
	}

	//loading pipeline cache
	std::string _pipeline_cache_name = "pipeline_cache";
	if (w_pipeline::create_pipeline_cache(_gDevice, _pipeline_cache_name) == W_FAILED)
	{
		logger.error("could not create pipeline cache");
		_pipeline_cache_name.clear();
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	//load collada scene
#ifdef WIN32
	auto _content_path_dir = wolf::system::io::get_current_directory() + L"/../../../../samples/03_advances/06_scene/src/content/";
#elif defined(__APPLE__)
	auto _content_path_dir = wolf::system::io::get_current_directory() + L"/../../../../../samples/03_advances/06_scene/src/content/";
#endif // WIN32

	//set vertex binding attributes
	std::map<uint32_t, std::vector<w_vertex_attribute>> _vertex_declaration;
	_vertex_declaration[0] = { W_POS, W_NORM }; //position and normal per each vertex
	_vertex_declaration[1] = { W_POS, W_ROT, W_SCALE }; // position, rotation, scale per each instance
	
	w_vertex_binding_attributes _vertex_binding_attributes(_vertex_declaration);

	auto _instanced_vertex_shader_path = _content_path_dir + L"shaders/instance.vert.spv";
	auto _basic_vertex_shader_path = _content_path_dir + L"shaders/basic.vert.spv";
	auto _fragment_shader_path = _content_path_dir + L"shaders/shader.frag.spv";
	
	//create pipeline cache for model
	std::string _model_pipeline_cache_name = "model_pipeline_cache";
	if (w_pipeline::create_pipeline_cache(_gDevice, _model_pipeline_cache_name) == W_FAILED)
	{
		logger.error("could not create pipeline cache for models");
		_model_pipeline_cache_name.clear();
	}

	//load collada scene
	auto _scene = w_content_manager::load<w_cpipeline_scene>(_content_path_dir + L"models/sponza/sponza.wscene");
	if (_scene)
	{
		//get first camera
		_scene->get_first_camera(this->_first_camera);
		float _near_plan = 0.1f, far_plan = 5000;
		this->_first_camera.set_near_plan(_near_plan);
		this->_first_camera.set_far_plan(far_plan);
		this->_first_camera.set_aspect_ratio(this->_viewport.width / this->_viewport.height);

		this->_first_camera.update_view();
		this->_first_camera.update_projection();
		this->_first_camera.update_frustum();

		//get all models
		std::vector<w_cpipeline_model*> _cmodels;
		_scene->get_all_models(_cmodels);

		for (auto _m : _cmodels)
		{
			auto _model = new (std::nothrow) model(_m, _vertex_binding_attributes);
			if (!_model)
			{
				V(W_FAILED, "allocating memory for model: " + _m->get_name(), _trace_info, 2);
				continue;
			}

			_hr = _model->load(
				_gDevice, 
				_model_pipeline_cache_name, 
				_m->get_instances_count() ? _instanced_vertex_shader_path : _basic_vertex_shader_path,
				_fragment_shader_path, 
				this->_draw_render_pass);
			if (_hr == W_FAILED)
			{
				V(W_FAILED, "loading model: " + _m->get_name(), _trace_info, 2);
				continue;
			}

			//set view projection
			_model->set_view_projection(this->_first_camera.get_view(), this->_first_camera.get_projection());
			this->_models.push_back(_model);
		}
		_cmodels.clear();
		_scene->release();
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++


	_build_draw_command_buffers();
}

W_RESULT scene::_build_draw_command_buffers()
{
	const std::string _trace_info = this->name + "::build_draw_command_buffers";
	W_RESULT _hr = W_PASSED;

	auto _size = this->_draw_command_buffers.get_commands_size();
	for (uint32_t i = 0; i < _size; ++i)
	{
		this->_draw_command_buffers.begin(i);
		{
			auto _cmd = this->_draw_command_buffers.get_command_at(i);
			this->_draw_render_pass.begin(
				i,
				_cmd,
				w_color::CORNFLOWER_BLUE(),
				1.0f,
				0.0f);
			{
				//draw all models
				for (auto _model : this->_models)
				{
					_model->draw(_cmd, false);
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

	sFPS = pGameTime.get_frames_per_second();
	sElapsedTimeInSec = pGameTime.get_elapsed_seconds();
	sTotalTimeTimeInSec = pGameTime.get_total_seconds();

	w_imgui::new_frame(sElapsedTimeInSec, [this]()
	{
		_update_gui();
	});

	w_point_t _screen_size;
	_screen_size.x = this->_viewport.width;
	_screen_size.y = this->_viewport.height;
	if (this->_first_camera.update(pGameTime, _screen_size))
	{
		//update view and projection of all models
		for (auto _model : this->_models)
		{
			_model->set_view_projection(this->_first_camera.get_view(), this->_first_camera.get_projection());
		}
	}

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

	const VkPipelineStageFlags _wait_dst_stage_mask[] =
	{
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	};

	//reset draw fence
	this->_draw_fence.reset();
	if (_gDevice->submit(
		{ &_draw_cmd, &_gui_cmd },//command buffers
		_gDevice->vk_graphics_queue, //graphics queue
		&_wait_dst_stage_mask[0], //destination masks
		{ _output_window->swap_chain_image_is_available_semaphore }, //wait semaphores
		{ _output_window->rendering_done_semaphore }, //signal semaphores
		&this->_draw_fence) == W_FAILED)
	{
		V(W_FAILED, "submiting queue for drawing", _trace_info, 3, true);
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

	for (auto _m : this->_models)
	{
		SAFE_RELEASE(_m);
	}
	
	//release gui's objects
	w_imgui::release();

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
