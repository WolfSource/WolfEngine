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
static bool sShowBoundingBox = true;
static bool sReBuild = true;

scene::scene(_In_z_ const std::wstring& pContentPath, _In_z_ const std::wstring& pLogPath, _In_z_ const std::wstring& pAppName) :
    w_game(pContentPath, pLogPath, pAppName)
{
	w_graphics_device_manager_configs _config;
	_config.debug_gpu = false;
	w_game::set_graphics_device_manager_configs(_config);

	w_game::set_fixed_time_step(false);

	this->_mesh = nullptr;
	this->_shape_bounding_box = nullptr;
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
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//get path of content folder
#ifdef WIN32
	auto _content_path_dir = wolf::system::io::get_current_directoryW() + L"/../../../../samples/03_advances/02_model/src/content/";
#elif defined(__APPLE__)
	auto _content_path_dir = wolf::system::io::get_current_directoryW() + L"/../../../../../samples/03_advances/02_model/src/content/";
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
		V(W_FAILED, "setting shader binding param", _trace_info, 3, true);
	}

	//loading pipeline cache
	std::string _pipeline_cache_name = "pipeline_cache";
	if (w_pipeline::create_pipeline_cache(_gDevice, _pipeline_cache_name) == W_FAILED)
	{
		logger.error("could not create pipeline cache");
		_pipeline_cache_name.clear();
	}

	w_vertex_binding_attributes _vertex_binding_attributes(w_vertex_declaration::VERTEX_POSITION);
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
		V(W_FAILED, "creating pipeline", _trace_info, 3, true);
	}

	//load collada scene
	auto _scene = w_content_manager::load<w_cpipeline_scene>(_content_path_dir + L"models/teapot.DAE");
	if (_scene)
	{
		std::vector<w_cpipeline_model*> _models;
		_scene->get_all_models(_models);

		for (auto _model : _models)
		{
			//load first model
			if (_model)
			{
				std::vector<w_cpipeline_mesh*> _meshes;
				_model->get_meshes(_meshes);

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

						//position
						_vertices.push_back(_pos[0]);//x
						_vertices.push_back(_pos[1]);//y
						_vertices.push_back(_pos[2]);//z
					}

					//merge all bounding box of meshes
					this->_mesh_bounding_box.merge(_mesh->bounding_box);
				}

				//create mesh
				this->_mesh = new (std::nothrow) wolf::graphics::w_mesh();
				if (_mesh)
				{
					auto _v_size = static_cast<uint32_t>(_vertices.size());

					//set vertex declaration
					_mesh->set_vertex_binding_attributes(_vertex_binding_attributes);
					if (_mesh->load(
						_gDevice,
						_vertices.data(),
						_v_size * sizeof(float),
						_v_size,
						_indices.data(),
						_indices.size()) == W_FAILED)
					{
						V(W_FAILED, "loading mesh", _trace_info);
					}
				}
				else
				{
					V(W_FAILED, "allocating memory of mesh", _trace_info);
				}

				//create shape
				this->_shape_bounding_box = new (std::nothrow) w_shapes(this->_mesh_bounding_box, w_color::YELLOW());
				if (this->_shape_bounding_box)
				{
					_hr = this->_shape_bounding_box->load(_gDevice, this->_draw_render_pass, this->_viewport, this->_viewport_scissor);
					if (_hr == W_FAILED)
					{
						SAFE_RELEASE(this->_shape_bounding_box);
						V(W_FAILED, "loading _shape_bounding_box", _trace_info, 3, true);
					}
				}
				else
				{
					this->_shape_bounding_box = nullptr;
					V(W_FAILED, "allocating memory for _shape_bounding_box", _trace_info, 3, true);
				}

				//clear resources
				_vertices.clear();
				_indices.clear();

				//store position and rotation of model into bounding box
				auto _t = _model->get_transform();
				_position.x = _t.position[0];
				_position.y = _t.position[1];
				_position.z = _t.position[2];


				auto _b_sphere = w_bounding_sphere::create_from_bounding_box(this->_mesh_bounding_box);
				this->_distance_from_eye = _b_sphere.radius;

				break;
			}
		}

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
				this->_pipeline.bind(_cmd, w_pipeline_bind_point::GRAPHICS);
				if (this->_mesh)
				{
					this->_mesh->draw(_cmd, nullptr, 0);
				}
				if (sShowBoundingBox && this->_shape_bounding_box)
				{
					this->_shape_bounding_box->draw(_cmd);
				}
            }
            this->_draw_render_pass.end(_cmd);
        }
        this->_draw_command_buffers.end(i);
    }

	sReBuild = false;

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
	
	auto _eye = glm::vec3(
		0,
		_position.y + this->_distance_from_eye,
		_position.z - 3 * this->_distance_from_eye);
	auto _up = glm::vec3(0, -1, 0);
	auto _look_at = glm::vec3(
		_position.x, 
		_position.y, 
		_position.z);

	auto _world = glm::mat4(1);//identity
	auto _view = glm::lookAtRH(_eye, _look_at, _up);
	auto _projection = glm::perspectiveRH(
		45.0f * glm::pi<float>() / 180.0f,
		this->_viewport.width / this->_viewport.height, 
		0.1f, 
		1000.0f);

	auto _wvp = _projection * _view * _world;
	
	this->_u0.data.wvp = _wvp;
	auto _hr = this->_u0.update();
	if (_hr == W_FAILED)
	{
		V(W_FAILED, "updating uniform WorldViewProjection", _trace_info, 3, false);
	}

	if (sShowBoundingBox && this->_shape_bounding_box)
	{
		this->_shape_bounding_box->update(_wvp);
	}
	if (sReBuild)
	{
		_build_draw_command_buffers();
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

    //release gui's objects
    w_imgui::release();

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	this->_u0.release();
	this->_pipeline.release();
	this->_shader.release();
	SAFE_RELEASE(this->_mesh);
	SAFE_RELEASE(this->_shape_bounding_box);
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

	if (ImGui::Checkbox("Show Bounding Box", &sShowBoundingBox))
	{
		sReBuild = true;
	}

    ImGui::End();

    return true;
}
