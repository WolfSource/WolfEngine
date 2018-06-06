#include "pch.h"
#include "scene.h"
#include <w_content_manager.h>
#include <glm_extension.h>

using namespace std;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++
#define MAX_SEARCH_LENGHT 256
static char sSearch[MAX_SEARCH_LENGHT];
static ImTextureID sTexID = (void*)("#i");
static float sWindowWidth = 0;
static float sWindowHeight = 0;
static float sUXAnimationSpeed = 1.2f;
typedef enum collapse_states
{
	openned = 0,
	openning,
	collapsing,
	collapsed
};
collapse_states sLeftWidgetCollapseState = collapse_states::openned;
static ImVec2 sLeftWidgetControllerSize;

namespace Colors
{
	static ImVec4 White = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	static ImVec4 Black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	static ImVec4 Transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	static ImVec4 LightBlue = ImVec4(0.0f, 0.4f, 1.0f, 0.9f);
	static ImVec4 Orange = ImVec4(1.0f, 0.564f, 0.313f, 1.0f);
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++

static uint32_t sFPS = 0;
static float sElapsedTimeInSec = 0;
static float sTotalTimeTimeInSec = 0;

static std::once_flag _once_flag;

scene::scene(_In_z_ const std::wstring& pContentPath, _In_z_ const std::wstring& pLogPath, _In_z_ const std::wstring& pAppName) :
	w_game(pContentPath, pLogPath, pAppName),
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	_current_selected_model(nullptr),
	_show_all_instances_colors(false),
	_rebuild_command_buffer(true)
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
{
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

void scene::initialize(_In_ std::map<int, w_window_info> pOutputWindowsInfo)
{
	//Add your pre-initialization logic here
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

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef WIN32
	shared::scene_content_path = wolf::system::io::get_current_directoryW() + L"/../../../../samples/03_advances/06_scene/src/content/";
#elif defined(__APPLE__)
	shared::scene_content_path = wolf::system::io::get_current_directoryW() + L"/../../../../../samples/03_advances/06_scene/src/content/";
#endif // WIN32
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	w_point_t _preferred_backbuffer_size;
	_preferred_backbuffer_size.x = _output_window->width;
	_preferred_backbuffer_size.y = _output_window->height;

	sWindowWidth = _preferred_backbuffer_size.x;
	sWindowHeight = _preferred_backbuffer_size.y;

	//initialize viewport
	this->_viewport.y = 0;
	this->_viewport.width = static_cast<float>(_preferred_backbuffer_size.x);
	this->_viewport.height = static_cast<float>(_preferred_backbuffer_size.y);
	this->_viewport.minDepth = 0;
	this->_viewport.maxDepth = 1;

	//initialize scissor of viewport
	this->_viewport_scissor.offset.x = 0;
	this->_viewport_scissor.offset.y = 0;
	this->_viewport_scissor.extent.width = _preferred_backbuffer_size.x;
	this->_viewport_scissor.extent.height = _preferred_backbuffer_size.y;

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

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//load gui icon
	auto _gui_icons = new (std::nothrow) w_texture();
	if (!_gui_icons)
	{
		_gui_icons = nullptr;
	}
	else
	{
		_hr = _gui_icons->initialize(_gDevice);
		if (_hr == W_FAILED)
		{
			release();
			V(W_FAILED,
				w_log_type::W_ERROR,
				true,
				"initializing icon. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
		}

		_hr = _gui_icons->load_texture_2D_from_file(wolf::content_path +
			L"textures/icons.png", true);
		if (_hr == W_FAILED)
		{
			release();
			V(W_FAILED,
				w_log_type::W_ERROR,
				true,
				"loading icon. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
		}
	}

	//load imgui
	w_imgui::load(
		_gDevice,
		_output_window,
		this->_viewport,
		this->_viewport_scissor,
		_gui_icons);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

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

	//set vertex binding attributes
	std::map<uint32_t, std::vector<w_vertex_attribute>> _basic_vertex_declaration;
	_basic_vertex_declaration[0] = { W_POS, W_NORM, W_UV }; //position ,normal and uv per each vertex

	std::map<uint32_t, std::vector<w_vertex_attribute>> _instance_vertex_declaration;
	_instance_vertex_declaration[0] = { W_POS, W_NORM, W_UV }; //position ,normal and uv per each vertex
	_instance_vertex_declaration[1] = { W_POS, W_ROT, W_SCALE }; // position, rotation, scale per each instance
	
	w_vertex_binding_attributes _basic_vertex_binding_attributes(_basic_vertex_declaration);
	w_vertex_binding_attributes _instance_vertex_binding_attributes(_instance_vertex_declaration);

	auto _instanced_vertex_shader_path = shared::scene_content_path + L"shaders/instance.vert.spv";
	auto _basic_vertex_shader_path = shared::scene_content_path + L"shaders/basic.vert.spv";
	auto _fragment_shader_path = shared::scene_content_path + L"shaders/shader.frag.spv";
	
	//create pipeline cache for model
	std::string _model_pipeline_cache_name = "model_pipeline_cache";
	if (w_pipeline::create_pipeline_cache(_gDevice, _model_pipeline_cache_name) == W_FAILED)
	{
		logger.error("could not create pipeline cache for models");
		_model_pipeline_cache_name.clear();
	}

	//load collada scene
	auto _scene = w_content_manager::load<w_cpipeline_scene>(wolf::content_path + L"models/sponza/sponza.wscene");
	if (_scene)
	{
		//get first camera
		_scene->get_first_camera(this->_first_camera);
		float _near_plan = 0.1f, far_plan = 10000;

		this->_first_camera.set_near_plan(_near_plan);
		this->_first_camera.set_far_plan(far_plan);
		this->_first_camera.set_aspect_ratio(this->_viewport.width / this->_viewport.height);
		this->_first_camera.set_rotation_speed(1.0f);
		this->_first_camera.set_movement_speed(3000.0f);

		this->_first_camera.update_view();
		this->_first_camera.update_projection();
		this->_first_camera.update_frustum();

		//get all models
		std::vector<w_cpipeline_model*> _cmodels;
		_scene->get_all_models(_cmodels);

		std::wstring _vertex_shader_path;
		for (auto _m : _cmodels)
		{
			model_mesh* _model = nullptr;
			if (_m->get_instances_count())
			{
				_vertex_shader_path = _instanced_vertex_shader_path;
				_model = new (std::nothrow) model_mesh(_m, _instance_vertex_binding_attributes);
			}
			else
			{
				_vertex_shader_path = _basic_vertex_shader_path;
				_model = new (std::nothrow) model_mesh(_m, _basic_vertex_binding_attributes);
			}

			if (!_model)
			{
				V(W_FAILED,
					w_log_type::W_WARNING,
					"allocating memory for model: {}. graphics device: {} . trace info: {}", _m->get_name(), _gDevice->get_info(), _trace_info);

				continue;
			}

			_hr = _model->load(
				_gDevice, 
				_model_pipeline_cache_name, 
				_vertex_shader_path,
				_fragment_shader_path, 
				this->_draw_render_pass);
			if (_hr == W_FAILED)
			{
				V(W_FAILED,
					w_log_type::W_WARNING,
					"loading model: {}. graphics device: {} . trace info: {}", _m->get_name(), _gDevice->get_info(), _trace_info);
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
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				//The following codes have been added for this project
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				//draw all models
				for (auto _model : this->_models)
				{
					_model->draw(_cmd, false);
				}
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

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	bool _gui_procceded = false;
	w_imgui::new_frame(sElapsedTimeInSec, [this, &_gui_procceded]()
	{
		_gui_procceded = _update_gui();
	});

	if (!_gui_procceded)
	{
		w_point_t _screen_size;
		_screen_size.x = this->_viewport.width;
		_screen_size.y = this->_viewport.height;
		this->_force_update_camera = this->_first_camera.update(pGameTime, _screen_size);
	}
	
	//first time update all models
	std::call_once(_once_flag, [this]()
	{
		this->_force_update_camera = true;
	});
	if (this->_force_update_camera)
	{
		this->_force_update_camera = false;
		//update view and projection of all models
		for (auto _model : this->_models)
		{
			_model->set_view_projection(this->_first_camera.get_view(), this->_first_camera.get_projection());
		}
	}

	if (this->_rebuild_command_buffer)
	{
		_build_draw_command_buffers();
		this->_rebuild_command_buffer = false;
	}
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

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	for (auto _m : this->_models)
	{
		SAFE_RELEASE(_m);
	}
	this->_current_selected_model = nullptr;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	//release gui's resources
	w_imgui::release();

	return w_game::release();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//The following codes have been added for this project
//++++++++++++++++++++++++++++++++++++++++++++++++++++

#pragma region gui

void scene::_show_floating_debug_window()
{
	//Setting Style
	ImGuiStyle& _style = ImGui::GetStyle();
	_style.WindowPadding = ImVec2(5, 0);
	_style.ItemSpacing = ImVec2(0, 2);

	_style.Colors[ImGuiCol_Text] = Colors::White;
	_style.Colors[ImGuiCol_WindowBg] = Colors::LightBlue;
	
	ImGuiWindowFlags  _window_flags = 0;;
	ImGui::SetNextWindowSize(ImVec2(400, 350), ImGuiSetCond_FirstUseEver);

	char _str[30];
	w_sprintf(_str, 30, "Wolf.Engine v.%d.%d.%d.%d", WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
	bool _is_open = true;
	if (!ImGui::Begin(_str, &_is_open, _window_flags))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("Press \"Esc\" to exit\r\nRight click on name of mesh to focus\r\nFPS:%d\r\nFrameTime:%f\r\nTotalTime:%f\r\n",
		sFPS,
		sElapsedTimeInSec,
		sTotalTimeTimeInSec);

	if (ImGui::Checkbox("Show all", &this->_show_all))
	{
		for (auto _m : this->_models)
		{
			if (!_m) continue;
			_m->set_visible(this->_show_all);
		}
		this->_rebuild_command_buffer = true;
	}

	if (ImGui::Checkbox("Show all instances colors", &this->_show_all_instances_colors))
	{
		for (auto _m : this->_models)
		{
			if (!_m) continue;
			_m->set_enable_instances_colors(this->_show_all_instances_colors);
		}
	}

	if (this->_current_selected_model)
	{
		auto _checked = this->_current_selected_model->get_enable_instances_colors();
		if (ImGui::Checkbox("Show instances colors", &_checked))
		{
			this->_current_selected_model->set_enable_instances_colors(_checked);
		}
		_checked = this->_current_selected_model->get_visible();
		if (ImGui::Checkbox("Visible", &_checked))
		{
			this->_current_selected_model->set_visible(_checked);
			this->_rebuild_command_buffer = true;
		}
	}

	ImGui::End();

	return;
}

scene::widget_info scene::_show_left_widget_controller()
{
	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 40.0f, sWindowHeight / 10.0f);
	_w_i.pos = ImVec2(0, 0);

#pragma region Setting Style
	ImGuiStyle& _style = ImGui::GetStyle();
	_style.WindowPadding = ImVec2(0, 0);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 0);
	_style.ItemSpacing = ImVec2(0, 0);
	_style.WindowBorderSize = 0.0f;
	_style.FrameBorderSize = 0.0f;
	_style.ChildBorderSize = 0;
#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(_w_i.size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Left Controller", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		return _w_i;
	}

	ImGui::SetWindowPos(ImVec2(_w_i.pos.x, _w_i.pos.y + 16.0f));
	if (ImGui::ImageButton(sTexID, ImVec2(32.0f, 32.0f), ImVec2(0, 0.1f), ImVec2(0.133f, 0.233f)))
	{
		sLeftWidgetCollapseState = collapse_states::openning;
	}

	ImGui::End();

	return _w_i;
}

scene::widget_info scene::_show_search_widget(_In_ scene::widget_info* pRelatedWidgetInfo)
{
	float _y_offset = pRelatedWidgetInfo->pos.y + pRelatedWidgetInfo->size.y + 1;
	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 6.095238095238095f, sWindowHeight - _y_offset);
	_w_i.pos = ImVec2(0, _y_offset);

	if (sLeftWidgetCollapseState == collapse_states::collapsed)
	{
		return _w_i;
	}
	else if (sLeftWidgetCollapseState == collapse_states::openning)
	{
		sLeftWidgetControllerSize.y = _w_i.size.y;
	}
	else if (sLeftWidgetCollapseState == collapse_states::collapsing)
	{
		sLeftWidgetControllerSize.y = _w_i.size.y;
	}
	else
	{
		sLeftWidgetControllerSize = _w_i.size;
	}

#pragma region Setting Style
	ImGuiStyle& _style = ImGui::GetStyle();
	_style.WindowPadding = ImVec2(0, -1);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 5.0f);
	_style.ItemSpacing = ImVec2(0, 0.0f);
	_style.WindowBorderSize = 1.5f;
	_style.FrameBorderSize = 0.1f;
	_style.ChildBorderSize = 0;

	//set text color
	_style.Colors[ImGuiCol_Text] = Colors::White;
	_style.Colors[ImGuiCol_HeaderHovered] = Colors::Orange;
	_style.Colors[ImGuiCol_Border] = Colors::Transparent;
	_style.Colors[ImGuiCol_Button] = Colors::Transparent;
	_style.Colors[ImGuiCol_ButtonHovered] = Colors::Transparent;
	_style.Colors[ImGuiCol_ButtonActive] = Colors::Transparent;
	_style.Colors[ImGuiCol_ImageButtonHovered] = Colors::White;
	_style.Colors[ImGuiCol_ImageButtonActive] = Colors::Orange;
#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(sLeftWidgetControllerSize, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Search", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		return _w_i;
	}

	auto _text_box_width = _w_i.size.x - 5.0f;
	ImGui::SetWindowPos(ImVec2(_w_i.pos.x + 2, _w_i.pos.y + 5));

	auto _serach_color = Colors::White;
	_serach_color.w = 0.9f;
	ImGui::PushStyleColor(ImGuiCol_Header, _serach_color);
	ImGui::PushStyleColor(ImGuiCol_Text, _serach_color);
	ImGui::PushItemWidth(_text_box_width);
	if (ImGui::InputText("", sSearch, MAX_SEARCH_LENGHT, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		logger.write("Searching");
	}
	ImGui::PopItemWidth();
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGuiTreeNodeFlags _node_flags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_Selected |
		ImGuiTreeNodeFlags_Bullet |
		ImGuiTreeNodeFlags_NoTreePushOnOpen;

	auto _icon_color = Colors::Black;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
	bool _opened = true;
	if (ImGui::CollapsingHeaderEx(sTexID, ImVec2(0.3f, 0.0f), ImVec2(0.4f, 0.1f), _icon_color, "Models", &_opened))
	{
		//add all models and instances to tree view
		auto _size = this->_models.size();
		if (_size)
		{
			for (int i = 0; i < _size; ++i)
			{
				//create a tree node for model
				auto _model = this->_models.at(i);
				if (!_model) continue;

				auto _name = _model->get_model_name();

				if (ImGui::TreeNode(_name.c_str()))
				{
					ImGui::PushStyleColor(ImGuiCol_Header, Colors::Transparent);
					//The Ref
					ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "Ref model");
					auto _b_sphere = w_bounding_sphere::create_from_bounding_box(_model->get_global_bounding_box());
					if (ImGui::IsItemClicked(1))
					{
						//on double click, focus on object
						this->_first_camera.focus(_b_sphere);
						this->_force_update_camera = true;

						this->_current_selected_model = _model;
					}
					else if (ImGui::IsItemClicked(0))
					{
						//on left click, change debug window
						this->_current_selected_model = _model;
					}
					//create sub tree nodes for all instances of models
					for (auto& _ins : _model->get_instances())
					{
						ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, _ins.name.c_str());
                        if (ImGui::IsItemClicked(1))
						{
							//on double click, focus on object
							_b_sphere.center[0] = _ins.position[0];
							_b_sphere.center[1] = _ins.position[1];
							_b_sphere.center[2] = _ins.position[2];

							this->_first_camera.focus(_b_sphere);
							this->_force_update_camera = true;
						}
					}
					ImGui::PopStyleColor();
					ImGui::TreePop();
				}
			}

			//fake
			if (ImGui::TreeNode("		"))
			{
				ImGui::TreePop();
			}
		}
	}
	ImGui::PopStyleVar();
	ImGui::End();

	return _w_i;
}

scene::widget_info scene::_show_explorer()
{
	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 6.095238095238095f, sWindowHeight / 22.5f);
	_w_i.pos = ImVec2(0, 0);

	if (sLeftWidgetCollapseState == collapse_states::collapsed)
	{
		_show_left_widget_controller();
		return _w_i;
	}
	else if (sLeftWidgetCollapseState == collapse_states::openning)
	{
		sLeftWidgetControllerSize.x += sUXAnimationSpeed * (sElapsedTimeInSec * 1000.0f);
		sLeftWidgetControllerSize.y = _w_i.size.y;

		if (sLeftWidgetControllerSize.x > _w_i.size.x)
		{
			sLeftWidgetCollapseState = collapse_states::openned;
		}
	}
	else if (sLeftWidgetCollapseState == collapse_states::collapsing)
	{
		sLeftWidgetControllerSize.x -= sUXAnimationSpeed * (sElapsedTimeInSec * 1000.0f);
		sLeftWidgetControllerSize.y = _w_i.size.y;

		if (sLeftWidgetControllerSize.x < _w_i.size.x / 4)
		{
			sLeftWidgetCollapseState = collapse_states::collapsed;
		}
	}
	else
	{
		sLeftWidgetControllerSize = _w_i.size;
	}

#pragma region Setting Style
	ImGuiStyle& _style = ImGui::GetStyle();
	_style.WindowPadding = ImVec2(0, 0);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 0);
	_style.ItemSpacing = ImVec2(0, 0);
	_style.WindowBorderSize = 0.0f;
	_style.FrameBorderSize = 0.0f;
	_style.ChildBorderSize = 0;
	
#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(sLeftWidgetControllerSize, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Scene Explorer", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
        ImGui::End();
        return _w_i;
	}

	ImGui::SetWindowPos(ImVec2(10, _w_i.pos.y + (_w_i.size.y / 2) - 8));
	ImGui::Text("Scene Explorer");
	ImGui::SameLine(0.0f, 0.0f);

	ImVec2 _icon_size(_w_i.size.y - 2.0f, _w_i.size.y - 2.0f);
	ImGui::SetWindowPos(ImVec2(80.0f, _w_i.pos.y + 1));
	if (ImGui::ImageButton(sTexID, _icon_size, ImVec2(0, 0), ImVec2(0.1, 0.1), -1.0f, ImVec4(0, 0, 0, 0), Colors::Black))
	{
		sLeftWidgetCollapseState = collapse_states::collapsing;
	}
    
	ImGui::End();
	return _w_i;
}

bool scene::_update_gui()
{
	_show_floating_debug_window();
	auto _l_w_info = _show_explorer();
	_show_search_widget(&_l_w_info);

	return ImGui::IsMouseHoveringAnyWindow() ? true : false;
}

#pragma endregion

//++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++
