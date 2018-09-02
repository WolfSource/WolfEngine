#include "pch.h"
#include "scene.h"
#include <w_content_manager.h>
#include <glm_extension.h>

#define NUM_INSTANCES 10

using namespace std;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::render::vulkan;
using namespace wolf::content_pipeline;

static uint32_t sFPS = 0;
static float sElapsedTimeInSec = 0;
static float sTotalTimeTimeInSec = 0;
static bool sCapture = false;

static float random_float(_In_ const float& a, _In_ const float& b)
{
	float _random = ((float)rand()) / (float)RAND_MAX;
	float _diff = b - a;
	return a + _random * _diff;
}

scene::scene(_In_z_ const std::wstring& pContentPath, _In_ const wolf::system::w_logger_config& pLogConfig) :
    w_game(pContentPath, pLogConfig)
{
	w_graphics_device_manager_configs _config;
	_config.debug_gpu = false;
	w_game::set_graphics_device_manager_configs(_config);

	w_game::set_fixed_time_step(false);

	this->_mesh = nullptr;

	this->on_pixels_captured_signal += [&](_In_ const w_point_t pSize, _In_ uint8_t* pPixels)->void
	{
		auto _path = wolf::system::io::get_current_directory();

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

		w_texture::save_bmp_to_file((_path + "/captured.bmp").c_str(), pSize.x, pSize.y, pPixels, 4);
	};
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
	w_point _offset;
	_offset.x = this->_viewport.x;
	_offset.y = this->_viewport.y;

	w_point_t _size;
	_size.x = this->_viewport.width;
	_size.y = this->_viewport.height;

	auto _hr = this->_draw_render_pass.load(
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
	
	//get path of content folder
#ifdef WIN32
	auto _content_path_dir = wolf::system::io::get_current_directoryW() + L"/../../../../samples/03_advances/03_instancing/src/content/";
#elif defined(__APPLE__)
	auto _content_path_dir = wolf::system::io::get_current_directoryW() + L"/../../../../../samples/03_advances/03_instancing/src/content/";
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
			"loading vertex shader. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
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
			"loading fragment shader. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}

	//load vertex shader uniform
	_hr = this->_u0.load(_gDevice);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"loading vertex shader uniform. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}

	std::vector<w_shader_binding_param> _shader_params;

	w_shader_binding_param _shader_param;
	_shader_param.index = 0;
	_shader_param.type = w_shader_binding_type::UNIFORM;
	_shader_param.stage = w_shader_stage_flag_bits::VERTEX_SHADER;
	_shader_param.buffer_info = this->_u0.get_descriptor_info();
	_shader_params.push_back(_shader_param);

	_hr = this->_shader.set_shader_binding_params(_shader_params);
	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"setting shader binding param. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
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
	std::map<uint32_t, std::vector<w_vertex_attribute>> _declaration;
	_declaration[0] = { W_VEC3, W_VEC3 }; //position per each vertex
	_declaration[1] = { W_VEC3, W_VEC3, W_FLOAT }; // position, rotation, scale per each instance
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	w_vertex_binding_attributes _vertex_binding_attributes(_declaration);
	_hr = this->_pipeline.load(
		_gDevice,
		_vertex_binding_attributes,
		w_primitive_topology::TRIANGLE_LIST,
		&this->_draw_render_pass,
		&this->_shader,
		{ this->_viewport },
		{ this->_viewport_scissor });

	if (_hr == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"creating pipeline. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}

	//load collada scene
#ifdef WIN32
	_content_path_dir = wolf::system::io::get_current_directoryW() + L"/../../../../samples/03_advances/02_model/src/content/";
#elif defined(__APPLE__)
	_content_path_dir = wolf::system::io::get_current_directoryW() + L"/../../../../../samples/03_advances/02_model/src/content/";
#endif // WIN32
	auto _scene = w_content_manager::load<w_cpipeline_scene>(_content_path_dir + L"models/teapot.DAE");
	if (_scene)
	{
		std::vector<w_cpipeline_model*> _models;
		_scene->get_all_models(_models);

		auto _cmd = this->_draw_command_buffers.get_command_at(0);
		for (auto _model : _models)
		{
			//load first model
			if (_model)
			{
				std::vector<w_cpipeline_mesh*> _meshes;
				_model->get_meshes(_meshes);

				w_bounding_box _mesh_bounding_box;

				std::vector<float> _vertices;
				std::vector<uint32_t> _indices;

				//get vertices and indices
				for (auto _mesh : _meshes)
				{
					for (size_t i = 0; i < _mesh->indices.size(); ++i)
					{
						_indices.push_back(_mesh->indices[i]);
					}

					for (auto& _v : _mesh->vertices)
					{
						//just store position
						auto _pos = _v.position;
						auto _nor = _v.normal;

						//position
						_vertices.push_back(_pos[0]);//x
						_vertices.push_back(_pos[1]);//y
						_vertices.push_back(_pos[2]);//z

						_vertices.push_back(_nor[0]);//x
						_vertices.push_back(_nor[1]);//y
						_vertices.push_back(_nor[2]);//z
					}

					//merge all bounding box of meshes
					_mesh_bounding_box.merge(_mesh->bounding_box);
				}

				//create mesh
				this->_mesh = new (std::nothrow) w_mesh();
				if (_mesh)
				{
					auto _v_size = static_cast<uint32_t>(_vertices.size());

					//set vertex declaration
					_mesh->set_vertex_binding_attributes(_vertex_binding_attributes);
					if (_mesh->load(
						_gDevice,
						_cmd,
						_vertices.data(),
						_v_size * sizeof(float),
						_v_size,
						_indices.data(),
						_indices.size()) == W_FAILED)
					{
						V(W_FAILED,
							w_log_type::W_ERROR,
							"loading mesh. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
					}
				}
				else
				{
					V(W_FAILED,
						w_log_type::W_ERROR,
						"allocating memory for mesh. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
				}

				//clear resources
				_vertices.clear();
				_indices.clear();

				//store position and rotation of model into bounding box
				auto _t = _model->get_transform();
				_position.x = _t->position[0];
				_position.y = _t->position[1];
				_position.z = _t->position[2];

				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				//The following codes have been added for this project
				//++++++++++++++++++++++++++++++++++++++++++++++++++++

				std::vector<vertex_instance_data> _vertex_instances_data(NUM_INSTANCES);
				
				//first one is ref model
				_vertex_instances_data[0].pos[0] = _t->position[0];
				_vertex_instances_data[0].pos[1] = _t->position[1];
				_vertex_instances_data[0].pos[2] = _t->position[2];

				_vertex_instances_data[0].rot[0] = _t->rotation[0];
				_vertex_instances_data[0].rot[1] = _t->rotation[1];
				_vertex_instances_data[0].rot[2] = _t->rotation[2];

				_vertex_instances_data[0].scale = 1.0f;

				for (size_t i = 1; i < NUM_INSTANCES; ++i)
				{
					_vertex_instances_data[i].pos[0] = _t->position[0] + (i + 1) * 20 * std::cos(i);
					_vertex_instances_data[i].pos[1] = _t->position[1];
					_vertex_instances_data[i].pos[2] = _t->position[2] + (i + 1) * 20 * std::sin(i);

					_vertex_instances_data[i].rot[0] = 0.0f;
					_vertex_instances_data[i].rot[1] = 0.0f;
					_vertex_instances_data[i].rot[2] = 0.0f;

					_vertex_instances_data[i].scale = random_float(0.7f, 1.5f);
				}

				auto _size = (uint32_t)(_vertex_instances_data.size() * sizeof(vertex_instance_data));
				w_buffer _staging_buffers;
				_staging_buffers.allocate_as_staging(_gDevice, _size);
				if (_staging_buffers.bind() == W_FAILED)
				{
					release();
					V(W_FAILED,
						w_log_type::W_ERROR,
						true,
						"binding to staging buffer of vertex_instance_buffer. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
				}

				if (_staging_buffers.set_data(_vertex_instances_data.data()) == W_FAILED)
				{
					release();
					V(W_FAILED,
						w_log_type::W_ERROR,
						true,
						"setting data to staging buffer of vertex_instance_buffer. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
				}

				if (this->_instances_buffer.allocate(
					_gDevice,
					_size,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					w_memory_usage_flag::MEMORY_USAGE_GPU_ONLY) == W_FAILED)
				{
					release();
					V(W_FAILED,
						w_log_type::W_ERROR,
						true,
						"loading device buffer of vertex_instance_buffer. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
				}

				if (_staging_buffers.copy_to(_cmd, this->_instances_buffer) == W_FAILED)
				{
					release();
					V(W_FAILED,
						w_log_type::W_ERROR,
						true,
						"copying to device buffer of vertex_instance_buffer. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
				}
				_vertex_instances_data.clear();
				_staging_buffers.release();
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				//++++++++++++++++++++++++++++++++++++++++++++++++++++

				auto _b_sphere = w_bounding_sphere::create_from_bounding_box(_mesh_bounding_box);
				this->_distance_from_eye = _b_sphere.radius;

				break;
			}
		}

		_scene->release();
	}

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
				this->_viewport.set(_cmd);
				this->_viewport_scissor.set(_cmd);

				this->_pipeline.bind(_cmd, w_pipeline_bind_point::GRAPHICS);
				if (this->_mesh)
				{
					//++++++++++++++++++++++++++++++++++++++++++++++++++++
					//The following codes have been added for this project
					//++++++++++++++++++++++++++++++++++++++++++++++++++++
					auto _buffer_handle = this->_instances_buffer.get_buffer_handle();
					this->_mesh->draw(_cmd, &_buffer_handle, NUM_INSTANCES, false);
					//++++++++++++++++++++++++++++++++++++++++++++++++++++
					//++++++++++++++++++++++++++++++++++++++++++++++++++++
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
    
	auto _eye = glm::vec3(
		0,
		_position.y + 5 * this->_distance_from_eye,
		_position.z - 8 * this->_distance_from_eye);
	auto _up = glm::vec3(0, -1, 0);
	auto _look_at = glm::vec3(
		_position.x, 
		_position.y, 
		_position.z);

	auto _view = glm::lookAtRH(_eye, _look_at, _up);
	auto _projection = glm::perspectiveRH(
		45.0f * glm::pi<float>() / 180.0f,
		this->_viewport.width / this->_viewport.height, 
		0.1f, 
		1000.0f);
	
	auto _cmd = this->_draw_command_buffers.get_command_at(0);
	this->_u0.data.view = _view;
	this->_u0.data.projection = _projection;
	auto _hr = this->_u0.update(_cmd);
	if (_hr == W_FAILED)
	{
		V(W_FAILED,
			w_log_type::W_ERROR,
			"updating uniform ViewProjection. trace info: {}", _trace_info);
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

	auto _draw_command_buffer = this->_draw_command_buffers.get_command_at(_frame_index);
	auto _gui_command_buffer = w_imgui::get_command_buffer_at(_frame_index);

	const std::vector<w_pipeline_stage_flag_bits> _wait_dst_stage_mask =
	{
		w_pipeline_stage_flag_bits::COLOR_ATTACHMENT_OUTPUT_BIT,
	};

	//reset draw fence
	this->_draw_fence.reset();
	if (_gDevice->submit(
		{ &_draw_command_buffer, &_gui_command_buffer },//command buffers
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
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	if (w_game::render(pGameTime) == W_FAILED)
	{
		release();
		V(W_FAILED,
			w_log_type::W_ERROR,
			true,
			"presenting to graphics device. graphics device: {} . trace info: {}", _gDevice->get_info(), _trace_info);
	}
	if (sCapture)
	{
		sCapture = false;
		//capture outputs of graphics device
		if (_gDevice->capture_presented_swap_chain_buffer(this->on_pixels_captured_signal) == W_FAILED)
		{
			V(W_FAILED,
				w_log_type::W_ERROR,
				"capturing graphics device. trace info: {}", _gDevice->get_info(), _trace_info);

			return W_FAILED;
		}
	}
	return W_PASSED;
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
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

	this->_pipeline.release();
	this->_shader.release();
	SAFE_RELEASE(this->_mesh);

	this->_u0.release();
	this->_instances_buffer.release();

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

	if (ImGui::Button("Capture"))
	{
		sCapture = true;

	}

    ImGui::End();

    return true;
}
