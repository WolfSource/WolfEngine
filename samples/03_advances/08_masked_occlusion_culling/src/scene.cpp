#include "pch.h"
#include "scene.h"
#include <w_content_manager.h>
#include <glm_extension.h>
#include <w_thread.h>
#include <w_task.h>
#include <tbb/parallel_for.h>

using namespace std;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::graphics;
using namespace wolf::content_pipeline;

#define MAX_SEARCH_LENGHT 256
static char sSearch[MAX_SEARCH_LENGHT];
static ImTextureID sIconTextureID = (void*)("#i");
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
static ImVec4 clear_color = ImVec4(0.184f, 0.184f, 0.184f, 1.00f);
static ImVec2 sLeftWidgetControllerSize;

namespace Colors
{
	static ImVec4 White = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	static ImVec4 WhiteGray = ImVec4(0.835f, 0.835f, 0.835f, 1.0f);
	static ImVec4 Gray = ImVec4(0.615f, 0.615f, 0.615f, 1.0f);
	static ImVec4 Black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	static ImVec4 Transparent = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	static ImVec4 Orange = ImVec4(1.0f, 0.564f, 0.313f, 1.0f);
	static ImVec4 LightBlue = ImVec4(0.0f, 0.631f, 0.949f, 1.0f);
	static ImVec4 Blue = ImVec4(0.0f, 0.490f, 0.949f, 1.0f);
	static ImVec4 BorderColor = ImVec4(0.4f, 0.392f, 0.388f, 1.0f);
};

static uint32_t sFPS = 0;
static float sElapsedTimeInSec = 0;
static float sTotalTimeTimeInSec = 0;

scene::scene(_In_z_ const std::wstring& pContentPath, _In_z_ const std::wstring& pLogPath, _In_z_ const std::wstring& pAppName) :
	w_game(pContentPath, pLogPath, pAppName),
	_current_selected_model(nullptr),
	_show_all_instances_colors(false),
	_rebuild_command_buffer(true),
	_show_all(true),
	_show_lods(false),
	_masked_occlusion_culling_debug_frame(nullptr),
	_show_moc_debug(false),
	_searching(false),
	_show_all_wireframe(false),
	_show_all_bounding_box(false),
	_visible_meshes(0),
	_has_camera_animation(false),
	_play_camera_anim(false),
	_current_camera_frame(0)
{
#if defined(__WIN32) && defined(DEBUG)
	w_graphics_device_manager_configs _config;
	_config.debug_gpu = false;
	w_game::set_graphics_device_manager_configs(_config);
#endif

	w_game::set_fixed_time_step(false);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	auto _number_of_threads = 1;// w_thread::get_number_of_hardware_thread_contexts();
	if (this->_masked_occlusion_culling.initialize(_number_of_threads, true) == W_FAILED)
	{
		V(W_FAILED, "initializing masked occlusion culling", "scene::scene", 3, true);
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	this->on_pixels_captured_signal += [&](_In_ const w_point_t pSize, _In_ uint8_t* pPixels)->void
	{
#pragma region Convert BGRA to RGBA
		//some gpu does not support RGBA, so we used BGRA as default format fo swap chain 
		for (size_t i = 0; i < pSize.x * pSize.y * 4; i += 4)
		{
			auto _r = pPixels[i];
			auto _b = pPixels[i + 2];

			pPixels[i] = _b;//R
			pPixels[i + 1] = pPixels[i + 1];//G
			pPixels[i + 2] = _r;//B
			pPixels[i + 3] = pPixels[i + 3];//A
		}
#pragma endregion

		std::string _path = "c:\\Wolf\\" + std::to_string(this->_current_camera_frame) + ".bmp";
		w_texture::save_bmp_to_file(_path.c_str(), pSize.x, pSize.y, pPixels, 4);

		this->_current_camera_frame = this->_current_camera_frame + 1;
		if (this->_current_camera_frame == this->_camera_anim_positions.size())
		{
			this->_play_camera_anim = false;
		}
	};
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

#ifdef WIN32
	shared::scene_content_path = wolf::system::io::get_current_directoryW() + L"/../../../../samples/03_advances/07_lod/src/content/";
#elif defined(__APPLE__)
	shared::scene_content_path = wolf::system::io::get_current_directoryW() + L"/../../../../../samples/03_advances/07_lod/src/content/";
#endif // WIN32

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

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	this->_masked_occlusion_culling_debug_frame = new (std::nothrow) w_texture();
	if (this->_masked_occlusion_culling_debug_frame)
	{
		if (this->_masked_occlusion_culling_debug_frame->initialize(
			_gDevice,
			_preferred_backbuffer_size.x,
			_preferred_backbuffer_size.y,
			false,
			true) == W_PASSED)
		{
			if (this->_masked_occlusion_culling_debug_frame->load_texture_from_memory_all_channels_same(255) == W_FAILED)
			{
				V(W_FAILED, "loading texture for masked occlusion culling debug frame", _trace_info, 2);
			}
		}
		else
		{
			V(W_FAILED, "initializing texture memory for masked occlusion culling debug frame", _trace_info, 2);
		}
	}
	else
	{
		V(W_FAILED, "allocating memory for texture of masked occlusion culling debug frame", _trace_info, 2);
	}
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

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

	//fence for syncing
	_hr = this->_draw_fence.initialize(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "creating draw fence", _trace_info, 3, true);
	}

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
			V(W_FAILED, "initializing icon", _trace_info, 3, true);
		}

		_hr = _gui_icons->load_texture_2D_from_file(wolf::content_path +
			L"textures/icons.png", true);
		if (_hr == W_FAILED)
		{
			release();
			V(W_FAILED, "loading icon", _trace_info, 3, true);
		}
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//load imgui
	w_imgui::load(
		_gDevice,
		_output_window,
		this->_viewport,
		this->_viewport_scissor,
		_gui_icons,
		&this->_masked_occlusion_culling_debug_frame,
		wolf::system::convert::wstring_to_string(wolf::content_path + L"fonts\\MyriadPro-Regular.ttf").c_str(),
		15.0f);
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	//create two primary command buffers for clearing screen
	auto _swap_chain_image_size = _output_window->swap_chain_image_views.size();
	_hr = this->_draw_command_buffers.load(_gDevice, _swap_chain_image_size);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED, "creating draw command buffers", _trace_info, 3, true);
	}

	//read all wscene(s) from content folder
	_load_scenes_from_folder(wolf::content_path + L"models/sponza/");

	//create coordinate system
	this->_shape_coordinate_axis = new (std::nothrow) w_shapes(w_color::LIME());
	if (this->_shape_coordinate_axis)
	{
		_hr = this->_shape_coordinate_axis->load(_gDevice, this->_draw_render_pass, this->_viewport, this->_viewport_scissor);
		if (_hr == W_FAILED)
		{
			V(W_FAILED, "loading shape coordinate axis", _trace_info, 3);
		}
	}
	else
	{
		V(W_FAILED, "allocating memory for shape coordinate axis", _trace_info, 3);
	}
}

W_RESULT scene::_load_scenes_from_folder(_In_z_ const std::wstring& pDirectoryPath)
{
	const std::string _trace_info = this->name + "::_load_scenes_from_folder";

	W_RESULT _hr = W_FAILED;
	auto _gDevice = this->graphics_devices[0];

	//loading pipeline cache
	std::string _model_pipeline_cache_name = "model_pipeline_cache";
	if (w_pipeline::create_pipeline_cache(_gDevice, _model_pipeline_cache_name) == W_FAILED)
	{
		logger.error("could not create model pipeline cache");
		_model_pipeline_cache_name.clear();
	}

	std::string _model_compute_pipeline_cache_name = "model_compute_pipeline_cache";
	if (w_pipeline::create_pipeline_cache(_gDevice, _model_compute_pipeline_cache_name) == W_FAILED)
	{
		logger.error("could not create model compute pipeline cache");
		_model_compute_pipeline_cache_name.clear();
	}

	//set vertex binding attributes
	std::map<uint32_t, std::vector<w_vertex_attribute>> _basic_vertex_declaration;
	_basic_vertex_declaration[0] = { W_POS, W_NORM, W_UV }; //position ,normal and uv per each vertex

	std::map<uint32_t, std::vector<w_vertex_attribute>> _instance_vertex_declaration;
	_instance_vertex_declaration[0] = { W_POS, W_NORM, W_UV }; //position ,normal and uv per each vertex
	_instance_vertex_declaration[1] = { W_POS, W_ROT }; // position, rotation per each instance

	w_vertex_binding_attributes _basic_vertex_binding_attributes(_basic_vertex_declaration);
	w_vertex_binding_attributes _instance_vertex_binding_attributes(_instance_vertex_declaration);

	const auto _instanced_vertex_shader_path = shared::scene_content_path + L"shaders/instance.vert.spv";
	const auto _basic_vertex_shader_path = shared::scene_content_path + L"shaders/basic.vert.spv";
	const auto _fragment_shader_path = shared::scene_content_path + L"shaders/shader.frag.spv";

	std::vector<std::wstring> _file_names;
	wolf::system::io::get_files_folders_in_directoryW(pDirectoryPath, _file_names);
	for (auto& _file_name : _file_names)
	{
		if (wolf::system::io::get_file_extentionW(_file_name) != L".wscene") continue;

		auto _scene = w_content_manager::load<w_cpipeline_scene>(_file_name);
		if (_scene)
		{
			//get first camera
			_scene->get_first_camera(this->_first_camera);
			float _near_plan = 0.1f, far_plan = 100000;

			this->_first_camera.set_near_plan(_near_plan);
			this->_first_camera.set_far_plan(far_plan);
			this->_first_camera.set_aspect_ratio(this->_viewport.width / this->_viewport.height);
			this->_first_camera.set_rotation_speed(1.0f);
			this->_first_camera.set_movement_speed(500.0f);

			this->_first_camera.update_view();
			this->_first_camera.update_projection();
			this->_first_camera.update_frustum();

			this->_masked_occlusion_culling.set_near_clip(_near_plan);
			this->_masked_occlusion_culling.set_resolution(this->_viewport.width, this->_viewport.height);
			//this->_masked_occlusion_culling.suspend_threads();

			//get all models
			std::vector<w_cpipeline_model*> _cmodels;
			_scene->get_all_models(_cmodels);

			std::wstring _vertex_shader_path;
			int index = 0;
			for (auto _m : _cmodels)
			{
				model* _model = nullptr;
				if (_m->get_instances_count())
				{
					_vertex_shader_path = _instanced_vertex_shader_path;
					_model = new (std::nothrow) model(_m, _instance_vertex_binding_attributes);
				}
				else
				{
					_vertex_shader_path = _basic_vertex_shader_path;
					_model = new (std::nothrow) model(_m, _basic_vertex_binding_attributes);
				}

				if (!_model)
				{
					V(W_FAILED, "allocating memory for model: " + _m->get_name(), _trace_info, 2);
					continue;
				}

				_model->set_is_sky(_m->get_name() == "SK001");
				_hr = _model->initialize();
				if (_hr == W_FAILED)
				{
					V(W_FAILED, "initializing model: " + _m->get_name(), _trace_info, 2);
					continue;
				}
				else
				{
					_hr = _model->load(
						_gDevice,
						_model_pipeline_cache_name,
						_model_compute_pipeline_cache_name,
						_vertex_shader_path,
						_fragment_shader_path,
						this->_draw_render_pass);
					if (_hr == W_FAILED)
					{
						V(W_FAILED, "loading model: " + _m->get_name(), _trace_info, 2);
						continue;
					}
				}

				this->_scene_models.push_back(_model);
			}
			_cmodels.clear();
			_scene->release();
		}
	}

	//check for wcam if exists
	auto _wcam = wolf::system::convert::wstring_to_string(pDirectoryPath) + "camera.wcam";
	if (wolf::system::io::get_is_file(_wcam.c_str()) == W_RESULT::W_PASSED)
	{
		//read camera animation path
		int _file_status = 1;
		auto _cam_anim_str = wolf::system::io::read_text_file(_wcam.c_str(), _file_status);
		if (_file_status == 1)
		{
			//read camera animation
			std::vector<std::string> _ns;
			wolf::system::convert::split_string(_cam_anim_str, "\n", _ns);

			if (_ns.size())
			{
				//the first line is camera positions
				std::vector<std::string> _poss;

				std::vector<float> _numbers;
				wolf::system::convert::split_string(_ns[0], "$", _poss);
				for (size_t i = 0; i < _poss.size(); ++i)
				{
					_numbers.clear();

					wolf::system::convert::split_string_then_convert_to<float>(_poss[i], ",", _numbers);
					this->_camera_anim_positions.push_back(glm::vec3(_numbers[0], _numbers[1], _numbers[2]));
				}

				//the second line is camera targets
				_poss.clear();
				wolf::system::convert::split_string(_ns[1], "$", _poss);
				for (size_t i = 0; i < _poss.size(); ++i)
				{
					_numbers.clear();

					wolf::system::convert::split_string_then_convert_to<float>(_poss[i], ",", _numbers);
					this->_camera_anim_targets.push_back(glm::vec3(_numbers[0], _numbers[1], _numbers[2]));
				}

				_poss.clear();
				_numbers.clear();
				_ns.clear();
			}

			this->_has_camera_animation = true;
		}
	}

	//sort models based on model names
	//if (this->_scene_models.size())
	//{
	//	std::sort(
	//		this->_scene_models.begin(),
	//		this->_scene_models.end(),
	//		[](_In_ model* p1, _In_ model* p2)
	//	{
	//		if (p1 && p2)
	//		{
	//			return p1->get_model_name().compare(p2->get_model_name()) <= 0;
	//		}
	//		return true;
	//	});
	//}


	return _hr;
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
				for (auto _model : this->_drawable_models)
				{
					_model->draw(_cmd, &this->_first_camera);
				}

				//draw coordinate system
				this->_shape_coordinate_axis->draw(_cmd);
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

		if (!this->_has_camera_animation || !this->_play_camera_anim)
		{
			this->_force_update_camera = this->_first_camera.update(pGameTime, _screen_size);
		}
	}

	if (this->_has_camera_animation && this->_play_camera_anim)
	{
		this->_camera_time.set_fixed_time_step(true);
		this->_camera_time.set_target_elapsed_seconds(1 / 30.0f);
		this->_camera_time.tick([this]()
		{
			this->_first_camera.set_translate(this->_camera_anim_positions[this->_current_camera_frame]);
			this->_first_camera.set_interest(this->_camera_anim_targets[this->_current_camera_frame]);
			this->_first_camera.update_view();
			this->_first_camera.update_frustum();

			this->_force_update_camera = true;
			this->_capture = true;
		});
	}

	if (this->_force_update_camera)
	{
		this->_force_update_camera = false;

		//clear all
		this->_visible_meshes = 0;
		this->_visible_models.clear();
		this->_drawable_models.clear();

		//test all bounding boxes of models with camera frustom
		tbb::parallel_for(
			tbb::blocked_range<size_t>(0, this->_scene_models.size()),
			[&](const tbb::blocked_range<size_t>& pRange)
		{
			for (size_t i = pRange.begin(); i < pRange.end(); ++i)
			{
				if (this->_scene_models[i] && this->_scene_models[i]->check_is_in_sight(&this->_first_camera))
				{
					this->_visible_models.push_back(this->_scene_models[i]);
				}
			}
		});

		//now check for masked occlusion culling
		bool _need_post_check = false;

		if (_visible_models.size())
		{
			//this->_masked_occlusion_culling.wake_threads();
			this->_masked_occlusion_culling.clear_buffer();

			auto _projection_view = this->_first_camera.get_projection_view();
			std::for_each(this->_visible_models.begin(), this->_visible_models.end(), [&](_In_ model* pModel)
			{
				if (pModel &&
					pModel->pre_update(_projection_view, this->_masked_occlusion_culling) == W_PASSED)
				{
					_need_post_check = true;
				}
			});

			//this->_masked_occlusion_culling.flush();
			if (_need_post_check)
			{
				//post update stage for all visible models to find drawable models
				std::for_each(this->_visible_models.begin(), this->_visible_models.end(), [&](_In_ model* pModel)
				{
					if (pModel &&
						pModel->post_update(this->_masked_occlusion_culling, this->_visible_meshes) == W_PASSED)
					{
						this->_drawable_models.push_back(pModel);
					}
				});

				if (_show_moc_debug)
				{
					auto _moc_debug_depth_frame = this->_masked_occlusion_culling.get_debug_frame(true);
					if (_moc_debug_depth_frame)
					{
						if (this->_masked_occlusion_culling_debug_frame->copy_data_to_texture_2D(_moc_debug_depth_frame) == W_FAILED)
						{
							V(W_FAILED, "copying data to texture of masked occlusion culling debug frame", _trace_info, 2);
						}
					}
				}
			}
			this->_rebuild_command_buffer = true;
			//this->_masked_occlusion_culling.suspend_threads();
		}

		//update shape coordinate
		auto _world = glm::mat4(1) * glm::scale(glm::vec3(20.0f));
		auto _wvp = this->_first_camera.get_projection_view() * _world;
		if (this->_shape_coordinate_axis->update(_wvp) == W_FAILED)
		{
			V(W_FAILED, "loading shape coordinate axis", _trace_info, 3);
		}
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

	//reset draw fence
	std::vector<wolf::graphics::w_semaphore> _wait_semaphors =
	{
		_output_window->swap_chain_image_is_available_semaphore
	};

	this->_draw_fence.reset();
	if (this->_rebuild_command_buffer)
	{
		//submit compute shader for all visible models
		std::for_each(this->_drawable_models.begin(), this->_drawable_models.end(),
			[&](_In_ model* pModel)
		{
			if (pModel)
			{
				if (pModel->submit_compute_shader() == W_PASSED)
				{
					auto _semaphore = pModel->get_compute_semaphore();
					if (_semaphore)
					{
						_wait_semaphors.push_back(*_semaphore);
					}
				}
			}
		});

		_build_draw_command_buffers();
		this->_rebuild_command_buffer = false;

		//If waitSemaphoreCount is not 0, pWaitDstStageMask must be a pointer to an array of waitSemaphoreCount valid combinations of VkPipelineStageFlagBits values
		this->_wait_dst_stage_mask.clear();
		this->_wait_dst_stage_mask.push_back(w_pipeline_stage_flag_bits::COLOR_ATTACHMENT_OUTPUT_BIT);
		for (size_t i = 1; i < _wait_semaphors.size(); ++i)
		{
			this->_wait_dst_stage_mask.push_back(w_pipeline_stage_flag_bits::COMPUTE_SHADER_BIT);
		}
	}

	std::vector<const w_command_buffer*> _commands_buffers = { &_draw_cmd };
	if (!this->_play_camera_anim)
	{
		_commands_buffers.push_back(&_gui_cmd);
	}

	if (_gDevice->submit(
		_commands_buffers,//command buffers
		_gDevice->vk_graphics_queue, //graphics queue
		this->_wait_dst_stage_mask, //destination masks
		_wait_semaphors, //wait semaphores
		{ _output_window->rendering_done_semaphore }, //signal semaphores
		&this->_draw_fence,
		false) == W_FAILED)
	{
		V(W_FAILED, "submiting queue for drawing", _trace_info, 3, true);
	}

	this->_draw_fence.wait();

	//if (_current_selected_model)
	//{
	//	if (_current_selected_model->get_instances_count())
	//	{
	//		auto _result = _current_selected_model->get_result_of_compute_shader();
	//		logger.write(std::to_string(_result.draw_count));
	//		logger.write(std::to_string(_result.lod_level[0]));
	//		logger.write(std::to_string(_result.lod_level[1]));
	//	}
	//}
	auto _hr = w_game::render(pGameTime);

	if (this->_play_camera_anim && this->_capture)
	{
		this->_capture = false;
		if (_gDevice->capture_presented_swap_chain_buffer(this->on_pixels_captured_signal) == W_FAILED)
		{
			V(W_FAILED, "capturing graphics device", _trace_info, 3);
			return W_FAILED;
		}
	}

	return _hr;
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
	for (auto _m : this->_scene_models)
	{
		SAFE_RELEASE(_m);
	}
	//this->_visible_models.clear();
	this->_scene_models.clear();
	this->_drawable_models.clear();
	this->_current_selected_model = nullptr;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	this->_masked_occlusion_culling.release();
	SAFE_DELETE(_masked_occlusion_culling_debug_frame);

	SAFE_RELEASE(this->_shape_coordinate_axis);

	//release gui's resources
	w_imgui::release();

	return w_game::release();
}

#pragma region gui

void scene::_show_floating_debug_window()
{
	//Setting Style
	ImGuiStyle& _style = ImGui::GetStyle();
	_style.WindowPadding = ImVec2(5, 0);
	_style.ItemSpacing = ImVec2(0, 2);

	_style.Colors[ImGuiCol_Text] = Colors::White;
	_style.Colors[ImGuiCol_WindowBg] = Colors::Gray;

	ImGuiWindowFlags  _window_flags = 0;;
	ImGui::SetNextWindowSize(ImVec2(550, 500), ImGuiSetCond_FirstUseEver);

	char _str[30];
#ifdef __WIN32
	sprintf_s(
#else
	sprintf(
#endif
		_str, "Wolf.Engine v.%d.%d.%d.%d", WOLF_MAJOR_VERSION, WOLF_MINOR_VERSION, WOLF_PATCH_VERSION, WOLF_DEBUG_VERSION);
	bool _is_open = true;
	if (!ImGui::Begin(_str, &_is_open, _window_flags))
	{
		ImGui::End();
		return;
	}

	auto _cam_position = this->_first_camera.get_translate();
	auto _cam_interest = this->_first_camera.get_interest();

	ImGui::Text("Press \"Esc\" to exit\r\nMovments:Q,Z,W,A,S,D and Mouse Left Button\r\nFPS:%d\r\nFrameTime:%f\r\nTotalTime:%f\r\nInFrustumVisibleModels:%d\r\nDrawingModels:%d\r\nVisibleMeshes:%d\r\nCamera Position:%.1f,%.1f,%.1f\r\nCamera LookAt:%.1f,%.1f,%.1f\r\n",
		sFPS,
		sElapsedTimeInSec,
		sTotalTimeTimeInSec,
		this->_visible_models.size(),
		this->_drawable_models.size(),
		this->_visible_meshes,
		_cam_position.x, _cam_position.y, _cam_position.z,
		_cam_interest.x, _cam_interest.y, _cam_interest.z);

	if (ImGui::Checkbox("Show all", &this->_show_all))
	{
		for (auto _m : this->_scene_models)
		{
			if (!_m) continue;
			_m->set_global_visiblity(this->_show_all);
		}
		this->_rebuild_command_buffer = true;
	}
	
	if (ImGui::Checkbox("Show only lod", &this->_show_lods))
	{
		for (auto _m : this->_scene_models)
		{
			if (!_m) continue;
			_m->set_show_only_lods(this->_show_lods);
		}
		this->_rebuild_command_buffer = true;
	}
	
	ImGui::Checkbox("Show Convex Hulls", &this->_show_moc_debug);

	if (ImGui::Checkbox("Show all instances colors", &this->_show_all_instances_colors))
	{
		for (auto _m : this->_scene_models)
		{
			if (!_m) continue;
			_m->set_enable_instances_colors(this->_show_all_instances_colors);
		}
	}
	if (ImGui::Checkbox("Show all in wireframe mode", &this->_show_all_wireframe))
	{
		for (auto _m : this->_scene_models)
		{
			if (!_m) continue;
			_m->set_showing_wireframe(this->_show_all_wireframe);
		}
		this->_rebuild_command_buffer = true;
	}
	if (ImGui::Checkbox("Show all bounding boxes", &this->_show_all_bounding_box))
	{
		for (auto _m : this->_scene_models)
		{
			if (!_m) continue;
			_m->set_show_bounding_box(this->_show_all_bounding_box);
		}
		this->_rebuild_command_buffer = true;
	}

	if (this->_current_selected_model)
	{
		auto _checked = this->_current_selected_model->get_enable_instances_colors();
		if (ImGui::Checkbox("Show instances colors", &_checked))
		{
			this->_current_selected_model->set_enable_instances_colors(_checked);
		}
		_checked = this->_current_selected_model->get_global_visiblity();
		if (ImGui::Checkbox("Visible", &_checked))
		{
			this->_current_selected_model->set_global_visiblity(_checked);
			this->_rebuild_command_buffer = true;
		}
		_checked = this->_current_selected_model->get_showing_wireframe();
		if (ImGui::Checkbox("Wireframe", &_checked))
		{
			this->_current_selected_model->set_showing_wireframe(_checked);
			this->_rebuild_command_buffer = true;
		}
	}

	auto _cam_rot_speed = this->_first_camera.get_rotation_speed();
	if (ImGui::SliderFloat("Camera Rotation Speed", &_cam_rot_speed, 0.1f, 5.0f))
	{
		this->_first_camera.set_rotation_speed(_cam_rot_speed);
	}

	auto _cam_mov_speed = this->_first_camera.get_movement_speed();
	if (ImGui::SliderFloat("Camera Movement Speed", &_cam_mov_speed, 100.0f, 10000.0f))
	{
		this->_first_camera.set_movement_speed(_cam_mov_speed);
	}

	if (this->_has_camera_animation && ImGui::Checkbox("Play Camera Animation", &this->_play_camera_anim))
	{
		this->_current_camera_frame = 0;
	}

	ImGui::End();

	return;
}

void scene::_show_floating_moc_debug_window()
{
	if (!this->_show_moc_debug) return;

	ImGuiWindowFlags  _window_flags = 0;;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;

	ImVec2 _image_size(640, 480);
	ImVec2 _window_size = ImVec2(_image_size.x - 5.0f, _image_size.y + 20.0f);

	ImGui::SetNextWindowSizeConstraints(_window_size, _window_size);
	ImGui::SetNextWindowContentWidth(_window_size.x);
	if (!ImGui::Begin("Live Camera 001", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImTextureID _tex_id = (void*)("#s");//this texture used as staging texture
	ImGui::Image(_tex_id, _image_size);
	ImGui::End();
}

//scene::widget_info scene::_show_left_widget_controller()
//{
//	widget_info _w_i;
//	_w_i.size = ImVec2(sWindowWidth / 40.0f, sWindowHeight / 10.0f);
//	_w_i.pos = ImVec2(0, 0);
//
//#pragma region Setting Style
//	ImGuiStyle& _style = ImGui::GetStyle();
//	_style.WindowPadding = ImVec2(0, 0);
//	_style.WindowRounding = 0;
//	_style.GrabRounding = 0;
//	_style.GrabMinSize = 0;
//	_style.FramePadding = ImVec2(0, 0);
//	_style.ItemSpacing = ImVec2(0, 0);
//	_style.WindowBorderSize = 0.0f;
//	_style.FrameBorderSize = 0.0f;
//	_style.ChildBorderSize = 0;
//#pragma endregion
//
//	ImGuiWindowFlags _window_flags = 0;
//	_window_flags |= ImGuiWindowFlags_NoTitleBar;
//	_window_flags |= ImGuiWindowFlags_NoResize;
//	_window_flags |= ImGuiWindowFlags_NoMove;
//	_window_flags |= ImGuiWindowFlags_NoScrollbar;
//	_window_flags |= ImGuiWindowFlags_NoCollapse;
//
//	ImGui::SetNextWindowSize(_w_i.size, ImGuiCond_Always);
//	ImGui::SetNextWindowPos(_w_i.pos);
//	if (!ImGui::Begin("Left Controller", 0, _window_flags))
//	{
//		// Early out if the window is collapsed, as an optimization.
//		return _w_i;
//	}
//
//	ImGui::SetWindowPos(ImVec2(_w_i.pos.x, _w_i.pos.y + 16.0f));
//	if (ImGui::ImageButton(sTexID, ImVec2(32.0f, 32.0f), ImVec2(0, 0.1f), ImVec2(0.133f, 0.233f)))
//	{
//		sLeftWidgetCollapseState = collapse_states::openning;
//	}
//
//	ImGui::End();
//
//	return _w_i;
//}
//
//scene::widget_info scene::_show_search_widget(_In_ scene::widget_info* pRelatedWidgetInfo)
//{
//	float _y_offset = pRelatedWidgetInfo->pos.y + pRelatedWidgetInfo->size.y + 1;
//	widget_info _w_i;
//	_w_i.size = ImVec2(sWindowWidth / 6.095238095238095f, sWindowHeight - _y_offset);
//	_w_i.pos = ImVec2(0, _y_offset);
//
//	if (sLeftWidgetCollapseState == collapse_states::collapsed)
//	{
//		return _w_i;
//	}
//	else if (sLeftWidgetCollapseState == collapse_states::openning)
//	{
//		sLeftWidgetControllerSize.y = _w_i.size.y;
//	}
//	else if (sLeftWidgetCollapseState == collapse_states::collapsing)
//	{
//		sLeftWidgetControllerSize.y = _w_i.size.y;
//	}
//	else
//	{
//		sLeftWidgetControllerSize = _w_i.size;
//	}
//
//#pragma region Setting Style
//	ImGuiStyle& _style = ImGui::GetStyle();
//	_style.WindowPadding = ImVec2(0, -1);
//	_style.WindowRounding = 0;
//	_style.GrabRounding = 0;
//	_style.GrabMinSize = 0;
//	_style.FramePadding = ImVec2(0, 5.0f);
//	_style.ItemSpacing = ImVec2(0, 0.0f);
//	_style.WindowBorderSize = 1.5f;
//	_style.FrameBorderSize = 0.1f;
//	_style.ChildBorderSize = 0;
//
//	//set text color
//	_style.Colors[ImGuiCol_Text] = Colors::White;
//	_style.Colors[ImGuiCol_HeaderHovered] = Colors::Orange;
//	_style.Colors[ImGuiCol_Border] = Colors::Transparent;
//	_style.Colors[ImGuiCol_Button] = Colors::Transparent;
//	_style.Colors[ImGuiCol_ButtonHovered] = Colors::Transparent;
//	_style.Colors[ImGuiCol_ButtonActive] = Colors::Transparent;
//	_style.Colors[ImGuiCol_ImageButtonHovered] = Colors::White;
//	_style.Colors[ImGuiCol_ImageButtonActive] = Colors::Orange;
//#pragma endregion
//
//	ImGuiWindowFlags _window_flags = 0;
//	_window_flags |= ImGuiWindowFlags_NoTitleBar;
//	_window_flags |= ImGuiWindowFlags_NoResize;
//	_window_flags |= ImGuiWindowFlags_NoMove;
//	_window_flags |= ImGuiWindowFlags_NoCollapse;
//
//	ImGui::SetNextWindowSize(sLeftWidgetControllerSize, ImGuiCond_Always);
//	ImGui::SetNextWindowPos(_w_i.pos);
//	if (!ImGui::Begin("Search", 0, _window_flags))
//	{
//		// Early out if the window is collapsed, as an optimization.
//		return _w_i;
//	}
//
//	auto _text_box_width = _w_i.size.x - 5.0f;
//	ImGui::SetWindowPos(ImVec2(_w_i.pos.x + 2, _w_i.pos.y + 5));
//
//	auto _serach_color = Colors::White;
//	_serach_color.w = 0.9f;
//	ImGui::PushStyleColor(ImGuiCol_Header, _serach_color);
//	ImGui::PushStyleColor(ImGuiCol_Text, _serach_color);
//	ImGui::PushItemWidth(_text_box_width);
//	if (ImGui::InputText("", sSearch, MAX_SEARCH_LENGHT, ImGuiInputTextFlags_EnterReturnsTrue))
//	{
//		this->_searched_models.clear();
//		if (!this->_searching)
//		{
//			if (sSearch[0] == '\0')
//			{
//				this->_searching = false;
//			}
//			else
//			{
//				this->_searching = true;
//				std::string _lower_search(sSearch);
//				std::transform(_lower_search.begin(), _lower_search.end(), _lower_search.begin(), ::tolower);
//				w_task::execute_async([this, _lower_search]()->W_RESULT
//				{
//					//start seraching areas
//					std::for_each(this->_scene_models.begin(), this->_scene_models.end(), [this, _lower_search](_In_ model* pModel)
//					{
//						if (pModel)
//						{
//							auto _name = pModel->get_model_name();
//							std::transform(_name.begin(), _name.end(), _name.begin(), ::tolower);
//							if (_name.find(_lower_search) != std::string::npos)
//							{
//								this->_searched_models.push_back(pModel);
//							}
//						}
//					});
//
//					return W_PASSED;
//
//				}, [this](W_RESULT pV)
//				{
//					//on callback
//					this->_searching = false;
//				});
//			}
//		}
//	}
//	ImGui::PopItemWidth();
//	ImGui::PopStyleColor();
//	ImGui::PopStyleColor();
//
//	ImGuiTreeNodeFlags _node_flags =
//		ImGuiTreeNodeFlags_OpenOnArrow |
//		ImGuiTreeNodeFlags_OpenOnDoubleClick |
//		ImGuiTreeNodeFlags_Selected |
//		ImGuiTreeNodeFlags_Bullet |
//		ImGuiTreeNodeFlags_NoTreePushOnOpen;
//
//	auto _icon_color = Colors::Black;
//	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 10));
//	bool _opened = true;
//	if (ImGui::CollapsingHeaderEx(sTexID, ImVec2(0.3f, 0.0f), ImVec2(0.4f, 0.1f), _icon_color, "Models", &_opened))
//	{
//		//add all models and instances to tree view
//		auto _size = this->_searched_models.size();
//		if (_size)
//		{
//			for (int i = 0; i < _size; ++i)
//			{
//				//create a tree node for model
//				auto _model = this->_searched_models.at(i);
//				if (!_model) continue;
//
//				auto _name = _model->get_model_name();
//
//				if (ImGui::TreeNode(_name.c_str()))
//				{
//					ImGui::PushStyleColor(ImGuiCol_Header, Colors::Transparent);
//					//The Ref
//					ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "Ref model");
//					auto _b_sphere = w_bounding_sphere::create_from_bounding_box(_model->get_global_bounding_box());
//					if (ImGui::IsItemClicked(1))
//					{
//						//on right click, focus on ref object
//
//						this->_current_selected_model = _model;
//						auto _position = this->_current_selected_model->get_position();
//						_b_sphere.center[0] = _position[0];
//						_b_sphere.center[1] = _position[1];
//						_b_sphere.center[2] = _position[2];
//
//						this->_first_camera.focus(_b_sphere);
//
//						this->_force_update_camera = true;
//
//					}
//					else if (ImGui::IsItemClicked(0))
//					{
//						//on left click, change debug window
//						this->_current_selected_model = _model;
//					}
//					//create sub tree nodes for all instances of models
//					for (auto& _ins : _model->get_instances())
//					{
//						ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, _ins.name.c_str());
//						if (ImGui::IsItemClicked(1))
//						{
//							//on right click, focus on object
//							_b_sphere.center[0] = _ins.position[0];
//							_b_sphere.center[1] = _ins.position[1];
//							_b_sphere.center[2] = _ins.position[2];
//
//							this->_first_camera.focus(_b_sphere);
//							this->_force_update_camera = true;
//						}
//					}
//					ImGui::PopStyleColor();
//					ImGui::TreePop();
//				}
//			}
//
//			//fake
//			if (ImGui::TreeNode("		"))
//			{
//				ImGui::TreePop();
//			}
//		}
//		else
//		{
//			_size = this->_scene_models.size();
//			if (_size)
//			{
//				for (int i = 0; i < _size; ++i)
//				{
//					//create a tree node for model
//					auto _model = this->_scene_models.at(i);
//					if (!_model) continue;
//
//					auto _name = _model->get_model_name();
//
//					if (ImGui::TreeNode(_name.c_str()))
//					{
//						ImGui::PushStyleColor(ImGuiCol_Header, Colors::Transparent);
//						//The Ref
//						ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "Ref model");
//						auto _b_sphere = w_bounding_sphere::create_from_bounding_box(_model->get_global_bounding_box());
//						if (ImGui::IsItemClicked(1))
//						{
//							//on right click, focus on ref object
//
//							this->_current_selected_model = _model;
//							auto _position = this->_current_selected_model->get_position();
//							_b_sphere.center[0] = _position[0];
//							_b_sphere.center[1] = _position[1];
//							_b_sphere.center[2] = _position[2];
//
//							this->_first_camera.focus(_b_sphere);
//
//							this->_force_update_camera = true;
//
//						}
//						else if (ImGui::IsItemClicked(0))
//						{
//							//on left click, change debug window
//							this->_current_selected_model = _model;
//						}
//						//create sub tree nodes for all instances of models
//						for (auto& _ins : _model->get_instances())
//						{
//							ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, _ins.name.c_str());
//							if (ImGui::IsItemClicked(1))
//							{
//								//on right click, focus on object
//								_b_sphere.center[0] = _ins.position[0];
//								_b_sphere.center[1] = _ins.position[1];
//								_b_sphere.center[2] = _ins.position[2];
//
//								this->_first_camera.focus(_b_sphere);
//								this->_force_update_camera = true;
//							}
//						}
//						ImGui::PopStyleColor();
//						ImGui::TreePop();
//					}
//				}
//
//				//fake
//				if (ImGui::TreeNode("		"))
//				{
//					ImGui::TreePop();
//				}
//			}
//		}
//	}
//	ImGui::PopStyleVar();
//	ImGui::End();
//
//	return _w_i;
//}

scene::widget_info scene::_show_menu(scene::widget_info* pRelatedWidgetInfo)
{
	ImGuiStyle& _style = ImGui::GetStyle();

	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth, sWindowHeight / 15.5f);
	_w_i.pos = ImVec2(0, 0);

	float _opacity = 0.5f;
	if (ImGui::IsMouseHoveringRect(_w_i.pos, ImVec2(_w_i.pos.x + _w_i.size.x, _w_i.pos.y + _w_i.size.y), false))
	{
		_opacity = 1.0f;
	}

#pragma region Setting Style
	_style.WindowPadding = ImVec2(0, 0);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 15);
	_style.ItemSpacing = ImVec2(15, 15);
	_style.WindowBorderSize = 0;
	_style.FrameBorderSize = 0;
	_style.ChildBorderSize = 0;

	//set text color
	_style.Colors[ImGuiCol_Text] = Colors::Black;
	_style.Colors[ImGuiCol_WindowBg] = Colors::WhiteGray;
	_style.Colors[ImGuiCol_WindowBg].w = _opacity;
	_style.Colors[ImGuiCol_HeaderHovered] = Colors::Orange;
	_style.Colors[ImGuiCol_Header] = Colors::Orange;
	_style.Colors[ImGuiCol_Header].w = 0.7f;
	_style.Colors[ImGuiCol_PopupBg] = Colors::WhiteGray;
	_style.Colors[ImGuiCol_PopupBg].w = 0.8f;
	_style.Colors[ImGuiCol_MenuBarBg].w = 0.0f;
	_style.Colors[ImGuiCol_Border].w = 0.0f;

#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;
	_window_flags |= ImGuiWindowFlags_MenuBar;

	ImGui::SetNextWindowSize(_w_i.size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Menu", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		goto end;
	}

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("	File_1													 ", NULL, nullptr);
			ImGui::MenuItem("	File_2													 ", NULL, nullptr);
			ImGui::MenuItem("	File_3													 ", NULL, nullptr);
			ImGui::MenuItem("	File_4													 ", NULL, nullptr);
			ImGui::MenuItem("	Exit													 ", NULL, nullptr);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			ImGui::MenuItem("	Edit_1													 ", NULL, nullptr);
			ImGui::MenuItem("	Edit_2													 ", NULL, nullptr);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Configuration"))
		{
			ImGui::MenuItem("	Config_1												 ", NULL, nullptr);
			ImGui::MenuItem("	Config_2												 ", NULL, nullptr);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("	Help_1													 ", NULL, nullptr);
			ImGui::MenuItem("	Help_2													 ", NULL, nullptr);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	//ImGui::SameLine();
	//ImGui::SetWindowPos(ImVec2(0, -30));
	//if (ImGui::ImageButton(sIconTextureID, ImVec2(32.0f, 32.0f), ImVec2(0, 0.1f), ImVec2(0.133f, 0.233f)))
	//{

	//}
end:
	ImGui::End();

	return _w_i;
}

scene::widget_info scene::_show_falcon_icon(scene::widget_info* pRelatedWidgetInfo)
{
	ImGuiStyle& _style = ImGui::GetStyle();

	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 20, sWindowHeight / 15.5f);
	_w_i.pos = ImVec2(sWindowWidth - _w_i.size.x - 25.0f, -10.0f);

#pragma region Setting Style
	_style.WindowPadding = ImVec2(0, 0);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 0);
	_style.ItemSpacing = ImVec2(0, 0);
	_style.WindowBorderSize = 0;
	_style.FrameBorderSize = 0;
	_style.ChildBorderSize = 0;

	//set text color
	_style.Colors[ImGuiCol_Text] = Colors::Black;
	_style.Colors[ImGuiCol_WindowBg] = Colors::Transparent;
	_style.Colors[ImGuiCol_Button] = Colors::Transparent;
	_style.Colors[ImGuiCol_ButtonActive] = Colors::Transparent;
	_style.Colors[ImGuiCol_ButtonHovered] = Colors::Transparent;
	_style.Colors[ImGuiCol_ImageButtonActive] = Colors::Blue;
	_style.Colors[ImGuiCol_ImageButtonHovered] = Colors::Orange;

#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;
	_window_flags |= ImGuiWindowFlags_MenuBar;

	ImGui::SetNextWindowSize(_w_i.size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Falcon", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		goto end;
	}

	if (ImGui::ImageButton(sIconTextureID, ImVec2(32.0f, 32.0f), ImVec2(0.133, 0.1f), ImVec2(0.266f, 0.233f), -1, Colors::Transparent, Colors::Black))
	{
		//show about
	}
	ImGui::SameLineEx(ImVec2(17.0f, 15.0f));
	ImGui::Text("alcon");
end:
	ImGui::End();

	return _w_i;
}

scene::widget_info scene::_show_explorer(scene::widget_info* pRelatedWidgetInfo)
{
	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 6.095238095238095f, sWindowHeight / 22.5f);
	_w_i.pos = ImVec2(0, pRelatedWidgetInfo->pos.y + pRelatedWidgetInfo->size.y + 1);

	if (sLeftWidgetCollapseState == collapse_states::collapsed)
	{
		_show_left_widget_controller(pRelatedWidgetInfo);
		return _w_i;
	}
	else if (sLeftWidgetCollapseState == collapse_states::openning)
	{
		sLeftWidgetControllerSize.x += sUXAnimationSpeed * sElapsedTimeInSec * 1000;
		sLeftWidgetControllerSize.y = _w_i.size.y;

		if (sLeftWidgetControllerSize.x > _w_i.size.x)
		{
			sLeftWidgetCollapseState = collapse_states::openned;
		}
	}
	else if (sLeftWidgetCollapseState == collapse_states::collapsing)
	{
		sLeftWidgetControllerSize.x -= sUXAnimationSpeed * sElapsedTimeInSec * 1000;
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

	//set text color
	_style.Colors[ImGuiCol_Text] = Colors::Black;
	_style.Colors[ImGuiCol_WindowBg] = Colors::Orange;
	_style.Colors[ImGuiCol_Button] = Colors::Orange;
	_style.Colors[ImGuiCol_ButtonHovered] = Colors::Orange;
	_style.Colors[ImGuiCol_ButtonActive] = Colors::Blue;
	_style.Colors[ImGuiCol_ImageButtonHovered] = Colors::LightBlue;
	_style.Colors[ImGuiCol_ImageButtonActive] = Colors::Orange;
#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(sLeftWidgetControllerSize, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Explorer", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		goto end;
	}

	ImGui::SetWindowPos(ImVec2(10, _w_i.pos.y + (_w_i.size.y / 2) - 8));
	ImGui::Text("Explorer");
	ImGui::SameLine(0.0f, 0.0f);

	ImVec2 _icon_size(_w_i.size.y - 2.0f, _w_i.size.y - 2.0f);
	ImGui::SetWindowPos(ImVec2(_w_i.pos.x + _w_i.size.x - 2.9f * _icon_size.x, _w_i.pos.y + 1));
	if (ImGui::ImageButton(sIconTextureID, _icon_size, ImVec2(0, 0), ImVec2(0.1, 0.1), -1.0f, ImVec4(0, 0, 0, 0), ImVec4(0.3f, 0.3f, 0.3f, 1)))
	{
		sLeftWidgetCollapseState = collapse_states::collapsing;
	}
end:
	ImGui::End();

	return _w_i;
}

scene::widget_info scene::_show_left_widget_controller(scene::widget_info* pRelatedWidgetInfo)
{
	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 40.0f, sWindowHeight / 10.0f);
	_w_i.pos = ImVec2(0, pRelatedWidgetInfo->pos.y + pRelatedWidgetInfo->size.y + 1);

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

	//set text color
	_style.Colors[ImGuiCol_WindowBg] = Colors::Orange;
	_style.Colors[ImGuiCol_ButtonActive] = Colors::Orange;
	_style.Colors[ImGuiCol_ImageButtonActive] = Colors::Blue;
	_style.Colors[ImGuiCol_ImageButtonHovered] = Colors::LightBlue;

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
		goto end;
	}

	ImGui::SetWindowPos(ImVec2(_w_i.pos.x, _w_i.pos.y + 16.0f));
	if (ImGui::ImageButton(sIconTextureID, ImVec2(32.0f, 32.0f), ImVec2(0, 0.1f), ImVec2(0.133f, 0.233f)))
	{
		sLeftWidgetCollapseState = collapse_states::openning;
	}
end:
	ImGui::End();

	return _w_i;
}

scene::widget_info scene::_show_search_widget(scene::widget_info* pRelatedWidgetInfo)
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
		//already did
		//sLeftWidgetControllerSize.x += 1.1f * sDeltaTime;
		sLeftWidgetControllerSize.y = _w_i.size.y;
	}
	else if (sLeftWidgetCollapseState == collapse_states::collapsing)
	{
		//already did
		//sLeftWidgetControllerSize.x -= sDeltaTime;
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
	_style.Colors[ImGuiCol_Text].x = 0.1f;
	_style.Colors[ImGuiCol_Text].y = 0.1f;
	_style.Colors[ImGuiCol_Text].z = 0.1f;
	_style.Colors[ImGuiCol_Text].w = 1.0f;

	_style.Colors[ImGuiCol_WindowBg] = Colors::Gray;
	_style.Colors[ImGuiCol_Header] = Colors::WhiteGray;
	_style.Colors[ImGuiCol_HeaderHovered] = Colors::Orange;
	_style.Colors[ImGuiCol_TextSelectedBg] = Colors::Orange;
	_style.Colors[ImGuiCol_Border] = Colors::BorderColor;
	_style.Colors[ImGuiCol_BorderShadow] = Colors::BorderColor;
	_style.Colors[ImGuiCol_BorderShadow].w = 0.5f;
	_style.Colors[ImGuiCol_Button] = Colors::Transparent;
	_style.Colors[ImGuiCol_ButtonHovered] = Colors::Orange;
	_style.Colors[ImGuiCol_ButtonActive] = Colors::Blue;
	_style.Colors[ImGuiCol_ImageButtonHovered] = Colors::LightBlue;
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
		goto end;
	}

	auto _text_box_width = _w_i.size.x / 1.5f;
	ImGui::SetWindowPos(ImVec2(_w_i.pos.x + 2, _w_i.pos.y + 5));

	auto _serach_color = Colors::Black;
	_serach_color.w = 0.9f;
	ImGui::PushStyleColor(ImGuiCol_Header, _serach_color);
	ImGui::PushItemWidth(_text_box_width);
	if (ImGui::InputText("", sSearch, MAX_SEARCH_LENGHT, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		printf("Searched entered");
	}
	ImGui::PopItemWidth();
	ImGui::PopStyleColor();

	ImGui::SameLineEx(ImVec2(_w_i.pos.x + _text_box_width, -3.0f), 7.0f);
	ImVec2 _icon_size(24.0f, 24.0f);
	ImGui::PushStyleColor(ImGuiCol_Border, Colors::Transparent);
	ImGui::PushStyleColor(ImGuiCol_BorderShadow, Colors::Transparent);
	if (ImGui::ImageButton(sIconTextureID, _icon_size, ImVec2(0.1f, 0.0f), ImVec2(0.2, 0.1), -1.0f, ImVec4(0, 0, 0, 0), Colors::Black))
	{
		printf("Searched called");
	}
	ImGui::SameLine(0.0f, 7.0f);
	if (ImGui::ImageButton(sIconTextureID, _icon_size, ImVec2(0.2f, 0.0f), ImVec2(0.3, 0.1), -1.0f, ImVec4(0, 0, 0, 0), Colors::Black))
	{
		printf("Searched called");
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();

	ImGuiTreeNodeFlags _node_flags =
		ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_Selected |
		ImGuiTreeNodeFlags_Leaf |
		ImGuiTreeNodeFlags_NoTreePushOnOpen;

	auto _icon_color = Colors::Black;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, -5));
	if (ImGui::CollapsingHeaderEx(sIconTextureID, ImVec2(0.3f, 0.0f), ImVec2(0.4f, 0.1f), _icon_color, "Equipments", false))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
		if (ImGui::TreeNode("\t\tArea 110"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			ImGui::TreeNodeEx((void*)(intptr_t)0, _node_flags, "\tHZ1C39050UE001");
			ImGui::TreeNodeEx((void*)(intptr_t)1, _node_flags, "\tHZ1C39050UE002");
			ImGui::TreeNodeEx((void*)(intptr_t)2, _node_flags, "\tHZ1C39050UE003");
			ImGui::TreeNodeEx((void*)(intptr_t)3, _node_flags, "\tHZ1C39050UE004");
			ImGui::TreeNodeEx((void*)(intptr_t)4, _node_flags, "\tHZ1C39050UE005");
			ImGui::TreeNodeEx((void*)(intptr_t)5, _node_flags, "\tHZ1C39050UE006");
			ImGui::TreeNodeEx((void*)(intptr_t)6, _node_flags, "\tHZ1C39050UE007");
			ImGui::TreeNodeEx((void*)(intptr_t)7, _node_flags, "\tHZ1C39050AA001");
			ImGui::TreeNodeEx((void*)(intptr_t)8, _node_flags, "\tHZ1C39050AB002");
			ImGui::TreeNodeEx((void*)(intptr_t)9, _node_flags, "\tHZ1C39050AC007");
			ImGui::TreeNodeEx((void*)(intptr_t)10, _node_flags, "\tHZ1C39050UE001");
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("\t\tArea 120"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			ImGui::TreeNodeEx((void*)(intptr_t)11, _node_flags, "\tHZ1C39054UE003UB02");
			ImGui::TreeNodeEx((void*)(intptr_t)12, _node_flags, "\tHZ1C39054UE003UB03");
			ImGui::TreeNodeEx((void*)(intptr_t)13, _node_flags, "\tHZ1C39054UE003UB04");
			ImGui::TreeNodeEx((void*)(intptr_t)14, _node_flags, "\tHZ1C39054UE003UB05");
			ImGui::TreeNodeEx((void*)(intptr_t)15, _node_flags, "\tHZ1C39054UE003UB06");
			ImGui::TreeNodeEx((void*)(intptr_t)16, _node_flags, "\tHZ1C39054UE003UB07");
			ImGui::TreeNodeEx((void*)(intptr_t)17, _node_flags, "\tHZ1C39054UE003UB08");
			ImGui::TreeNodeEx((void*)(intptr_t)18, _node_flags, "\tHZ1C39054UE003UB09");
			ImGui::TreeNodeEx((void*)(intptr_t)19, _node_flags, "\tHZ1C39054UE003UB10");
			ImGui::TreeNodeEx((void*)(intptr_t)20, _node_flags, "\tHZ1C39054UE003UB11");
			ImGui::TreeNodeEx((void*)(intptr_t)21, _node_flags, "\tHZ1C39054UE003UB12");
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		ImGui::PopStyleVar();
	}
	if (ImGui::CollapsingHeaderEx(sIconTextureID, ImVec2(0.4f, 0.0f), ImVec2(0.5f, 0.1f), _icon_color, "Cameras", false))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
		if (ImGui::TreeNode("\t\tArea 400"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			ImGui::TreeNodeEx((void*)(intptr_t)0, _node_flags, "\tCamera 01");
			if (ImGui::IsItemClicked())
			{
				this->_show_moc_debug = !this->_show_moc_debug;
			}
			if (ImGui::TreeNodeEx((void*)(intptr_t)1, _node_flags, "\tCamera 02"))
			{
				//this->_show_moc_debug = !this->_show_moc_debug;
			}
			if (ImGui::TreeNodeEx((void*)(intptr_t)2, _node_flags, "\tCamera 03"))
			{
				//this->_show_moc_debug = !this->_show_moc_debug;
			}
			if (ImGui::TreeNodeEx((void*)(intptr_t)3, _node_flags, "\tCamera 04"))
			{
				//this->_show_moc_debug = !this->_show_moc_debug;
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("\t\tArea 440"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			for (int i = 0; i < 10; ++i)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "\Cam");
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		ImGui::PopStyleVar();
	}
	if (ImGui::CollapsingHeaderEx(sIconTextureID, ImVec2(0.5f, 0.0f), ImVec2(0.6f, 0.1f), _icon_color, "P & ID", false))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 5));
		if (ImGui::TreeNode("\t\tArea 150"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			for (int i = 0; i < 10; ++i)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "\tPlan");
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("\t\tArea 160"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			for (int i = 0; i < 10; ++i)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "\tPlan");
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("\t\tArea 410"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			for (int i = 0; i < 10; ++i)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "\tPlan");
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("\t\tArea 440"))
		{
			ImGui::PushStyleColor(ImGuiCol_Header, Colors::Gray);
			for (int i = 0; i < 10; ++i)
			{
				ImGui::TreeNodeEx((void*)(intptr_t)i, _node_flags, "\tPlan");
			}
			ImGui::PopStyleColor();
			ImGui::TreePop();
		}
		ImGui::PopStyleVar();
	}
	ImGui::PopStyleVar();
end:
	ImGui::End();

	return _w_i;
}

static float sTimes[7];
scene::widget_info scene::_show_right_widget(scene::widget_info* pRelatedWidgetInfo)
{
	widget_info _w_i;
	_w_i.size = ImVec2(sWindowWidth / 4.923f, sWindowHeight - pRelatedWidgetInfo->size.y);
	_w_i.pos = ImVec2(sWindowWidth - _w_i.size.x, pRelatedWidgetInfo->size.y);

#pragma region Setting Style
	ImGuiStyle& _style = ImGui::GetStyle();
	_style.WindowPadding = ImVec2(0, 0);
	_style.WindowRounding = 0;
	_style.GrabRounding = 0;
	_style.GrabMinSize = 0;
	_style.FramePadding = ImVec2(0, 5.0);
	_style.ItemSpacing = ImVec2(0, 1);
	_style.WindowBorderSize = 1.5f;
	_style.FrameBorderSize = 0.1f;
	_style.ChildBorderSize = 0;

	//set text color
	_style.Colors[ImGuiCol_Text].x = 0.0f;
	_style.Colors[ImGuiCol_Text].y = 0.0f;
	_style.Colors[ImGuiCol_Text].z = 0.0f;
	_style.Colors[ImGuiCol_Text].w = 1.0f;

	_style.Colors[ImGuiCol_WindowBg].x = 0.615f;
	_style.Colors[ImGuiCol_WindowBg].y = 0.615f;
	_style.Colors[ImGuiCol_WindowBg].z = 0.615f;
	_style.Colors[ImGuiCol_WindowBg].w = 1.0f;

	_style.Colors[ImGuiCol_TextSelectedBg].x = 0.9098039215686275f;
	_style.Colors[ImGuiCol_TextSelectedBg].y = 0.4431372549019608f;
	_style.Colors[ImGuiCol_TextSelectedBg].z = 0.3176470588235294f;
	_style.Colors[ImGuiCol_TextSelectedBg].w = 0.6f;

	_style.Colors[ImGuiCol_Border].x = 0.4f;
	_style.Colors[ImGuiCol_Border].y = 0.392f;
	_style.Colors[ImGuiCol_Border].z = 0.388f;
	_style.Colors[ImGuiCol_Border].w = 1.0f;

	_style.Colors[ImGuiCol_BorderShadow].x = 0.4f;
	_style.Colors[ImGuiCol_BorderShadow].y = 0.392f;
	_style.Colors[ImGuiCol_BorderShadow].z = 0.388f;
	_style.Colors[ImGuiCol_BorderShadow].w = 0.5f;

#pragma endregion

	ImGuiWindowFlags _window_flags = 0;
	_window_flags |= ImGuiWindowFlags_NoTitleBar;
	_window_flags |= ImGuiWindowFlags_NoResize;
	_window_flags |= ImGuiWindowFlags_NoMove;
	_window_flags |= ImGuiWindowFlags_NoScrollbar;
	_window_flags |= ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(_w_i.size, ImGuiCond_Always);
	ImGui::SetNextWindowPos(_w_i.pos);
	if (!ImGui::Begin("Properties", 0, _window_flags))
	{
		// Early out if the window is collapsed, as an optimization.
		goto end;
	}

	static bool _live = true;
	ImGui::Checkbox("Live", &_live);

	auto _time_now = ImGui::GetTime();
	auto _array_time_len = IM_ARRAYSIZE(sTimes);
	if (_live)
	{
		//shift times
		for (size_t i = 0; i < _array_time_len - 1; ++i)
		{
			sTimes[i] = sTimes[i + 1];
		}
		sTimes[_array_time_len - 1] = _time_now;
	}
	ImGui::PlotLines("Time", sTimes, _array_time_len);

	// Create a dummy array of contiguous float values to plot
	// Tip: If your float aren't contiguous but part of a structure, you can pass a pointer to your first float and the sizeof() of your structure in the Stride parameter.
	static float values[90] = { 0 };
	static int values_offset = 0;
	static float refresh_time = 0.0f;
	if (!_live || refresh_time == 0.0f)
	{
		refresh_time = _time_now;
	}
	float _avg = 0;
	while (refresh_time < ImGui::GetTime()) // Create dummy data at fixed 120 hz rate for the demo
	{
		static float phase = 0.0f;
		values[values_offset] = cosf(phase);
		values_offset = (values_offset + 1) % IM_ARRAYSIZE(values);
		phase += 0.10f * values_offset;
		refresh_time += 1.0f / 120.0f;

		_avg += values[values_offset];
	}

	char _avg_str[32];
	_avg_str[31] = '\0';
	sprintf(_avg_str, "avg %.3f", abs(_avg));
	ImGui::PlotLines("Samples", values, IM_ARRAYSIZE(values), values_offset, _avg_str, -1.0f, 1.0f, ImVec2(0, 80));
	ImGui::PlotHistogram("Temps", sTimes, _array_time_len, 0, NULL, 0.0f, 1.0f, ImVec2(0, 80));

	// Use functions to generate output
	// FIXME: This is rather awkward because current plot API only pass in indices. We probably want an API passing floats and user provide sample rate/count.
	struct Funcs
	{
		static float Sin(void*, int i) { return sinf(i * 0.1f); }
		static float Saw(void*, int i) { return (i & 1) ? 1.0f : -1.0f; }
	};
	static int func_type = 0, display_count = 70;
	ImGui::Separator();
	ImGui::PushItemWidth(100); ImGui::Combo("Range", &func_type, "Data1\0Data2\0"); ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::SliderInt("Sample count", &display_count, 1, 400);
	float(*func)(void*, int) = (func_type == 0) ? Funcs::Sin : Funcs::Saw;
	ImGui::PlotLines("Samples", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 70));
	ImGui::PlotHistogram("Histogram", func, NULL, display_count, 0, NULL, -1.0f, 1.0f, ImVec2(0, 70));
	ImGui::Separator();

	// Animate a simple progress bar
	static float progress = 0.0f, progress_dir = 1.0f;
	if (_live)
	{
		progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
		if (progress >= +1.1f) { progress = +1.1f; progress_dir *= -1.0f; }
		if (progress <= -0.1f) { progress = -0.1f; progress_dir *= -1.0f; }
	}

	float progress_saturated = (progress < 0.0f) ? 0.0f : (progress > 1.0f) ? 1.0f : progress;
	char buf[32];
	sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
	ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::Text("Pressures");

	//ImVec2 _tex_size(256, 256);
	//ImGui::Image(sMap2DTextureID, _tex_size);

end:
	ImGui::End();

	return _w_i;
}

bool scene::_update_gui()
{
	//for debug
	{
		//_show_floating_debug_window();
		_show_floating_moc_debug_window();
	}

	auto _w_i = _show_menu(nullptr);
	_show_falcon_icon(nullptr);
	
	auto _l_w_info = _show_explorer(&_w_i);
	_show_search_widget(&_l_w_info);
	_show_right_widget(&_w_i);

	return ImGui::IsMouseHoveringAnyWindow() ? true : false;
}

#pragma endregion
