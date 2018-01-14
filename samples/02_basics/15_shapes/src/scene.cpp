#include "pch.h"
#include "scene.h"
#include <glm_extention.h>

using namespace std;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::graphics;

static uint32_t sFPS = 0;
static float sElapsedTimeInSec = 0;
static float sTotalTimeTimeInSec = 0;

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

    //initialize attachment buffers
    w_attachment_buffer_desc _color(w_texture_buffer_type::W_TEXTURE_COLOR_BUFFER);
    w_attachment_buffer_desc _depth(w_texture_buffer_type::W_TEXTURE_DEPTH_BUFFER);

    //define color and depth buffers for render pass
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
    //create render pass for gui
    //do not clear frame buffer
    _attachment_descriptions[0].desc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    _attachment_descriptions[1].desc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    _attachment_descriptions[1].desc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    _hr = this->_gui_render_pass.load(_gDevice,
        _viewport,
        _viewport_scissor,
        _attachment_descriptions);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating render pass for gui", _trace_info, 3, true);
    }

    //create frame buffers
    auto _draw_render_pass_handle = this->_draw_render_pass.get_handle();
    _hr = this->_draw_frame_buffers.load(_gDevice,
        _draw_render_pass_handle,
        _output_window);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating frame buffers", _trace_info, 3, true);
    }
    //create frame buffers for gui
    auto _gui_render_pass_handle = this->_gui_render_pass.get_handle();
    _hr = this->_gui_frame_buffers.load(_gDevice,
        _gui_render_pass_handle,
        _output_window);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating frame buffers for gui", _trace_info, 3, true);
    }

    //create semaphore
    _hr = this->_draw_semaphore.initialize(_gDevice);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating draw semaphore", _trace_info, 3, true);
    }

    //Fence for syncing
    _hr = this->_draw_fence.initialize(_gDevice);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating draw fence", _trace_info, 3, true);
    }

    //load imgui
    w_imgui::load(_gDevice,
#ifdef __WIN32
        _output_window->hwnd,
#endif
        _screen_size,
        _gui_render_pass_handle,
        nullptr,
        nullptr);

    //create two primary command buffers for clearing screen
    auto _swap_chain_image_size = _output_window->vk_swap_chain_image_views.size();
    _hr = this->_draw_command_buffers.load(_gDevice, _swap_chain_image_size);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating draw command buffers", _trace_info, 3, true);
    }

    _hr = this->_gui_command_buffers.load(_gDevice, _swap_chain_image_size);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating draw command buffers for gui", _trace_info, 3, true);
    }
	
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	_hr = this->_shapes.load(_gDevice, this->_draw_render_pass, this->_viewport, this->_viewport_scissor);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating draw command buffers for gui", _trace_info, 3, true);
	}

	this->_bounding_box.min[0] = -3.0f;
	this->_bounding_box.min[1] = -3.0f;
	this->_bounding_box.min[2] = -3.0f;

	this->_bounding_box.max[0] = 3.0f;
	this->_bounding_box.max[1] = 3.0f;
	this->_bounding_box.max[2] = 3.0f;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
}

HRESULT scene::_build_draw_command_buffers(
    _In_ const std::shared_ptr<w_graphics_device>& pGDevice,
    _In_ const wolf::system::w_game_time& pGameTime)
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
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				//The following codes have been added for this project
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				this->_shapes.add_bounding_box(this->_bounding_box, w_color::YELLOW(), w_time_span::zero());
                this->_shapes.draw(_cmd, pGameTime);
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
				//++++++++++++++++++++++++++++++++++++++++++++++++++++
            }
            this->_draw_render_pass.end(_cmd);
        }
        this->_draw_command_buffers.end(i);
    }
    return _hr;
}

HRESULT scene::_build_gui_command_buffers(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    const std::string _trace_info = this->name + "::build_gui_command_buffers";
    HRESULT _hr = S_OK;

    auto _size = this->_gui_command_buffers.get_commands_size();
    for (uint32_t i = 0; i < _size; ++i)
    {
        this->_gui_command_buffers.begin(i);
        {
            auto _cmd = this->_gui_command_buffers.get_command_at(i);
            this->_gui_render_pass.begin(_cmd, this->_gui_frame_buffers.get_frame_buffer_at(i));
            {
                w_imgui::update_buffers(this->_gui_render_pass);
                w_imgui::render(_cmd);
            }
            this->_gui_render_pass.end(_cmd);
        }
        this->_gui_command_buffers.end(i);
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
    
	w_game::update(pGameTime);
}

HRESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
	if (w_game::exiting) return S_OK;

	const std::string _trace_info = this->name + "::render";

	auto _gDevice = this->graphics_devices[0];
	auto _output_window = &(_gDevice->output_presentation_windows[0]);
	auto _frame_index = _output_window->vk_swap_chain_image_index;

    _build_gui_command_buffers(_gDevice);
    _build_draw_command_buffers(_gDevice, pGameTime);
    
	//add wait semaphores
	std::vector<VkSemaphore> _wait_semaphors = { *(_output_window->vk_swap_chain_image_is_available_semaphore.get()) };
	auto _draw_command_buffer = this->_draw_command_buffers.get_command_at(_frame_index);
    auto _gui_command_buffer = this->_gui_command_buffers.get_command_at(_frame_index);

	const VkPipelineStageFlags _wait_dst_stage_mask[] =
	{
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	};

	//reset draw fence
	this->_draw_fence.reset();
	if (_gDevice->submit(
		{ _draw_command_buffer, _gui_command_buffer },
		_gDevice->vk_graphics_queue,
		&_wait_dst_stage_mask[0],
		_wait_semaphors,
		{ *_output_window->vk_rendering_done_semaphore.get() },
		&this->_draw_fence) == S_FALSE)
	{
		V(S_FALSE, "submiting queue for drawing", _trace_info, 3, true);
	}
	this->_draw_fence.wait();

	//clear all wait semaphores
	_wait_semaphors.clear();

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
	this->_draw_fence.release();
	this->_draw_semaphore.release();

	this->_draw_command_buffers.release();
	this->_draw_render_pass.release();
	this->_draw_frame_buffers.release();

    //release gui's objects
    this->_gui_command_buffers.release();
    this->_gui_render_pass.release();
    this->_gui_frame_buffers.release();
    w_imgui::release();

	this->_shapes.release();

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

    ImGui::Text("FPS:%d\r\nFrameTime:%f\r\nTotalTime:%f\r\nMouse Position:%d,%d\r\n",
        sFPS,
        sElapsedTimeInSec,
        sTotalTimeTimeInSec,
        wolf::inputs_manager.mouse.pos_x, wolf::inputs_manager.mouse.pos_y);

    ImGui::End();

    return true;
}
