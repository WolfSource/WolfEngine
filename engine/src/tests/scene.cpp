#include "pch.h"
#include "scene.h"
#include <tbb/parallel_for.h>
#include <w_content_manager.h>
#include <w_media_core.h>    

using namespace std;
using namespace wolf;
using namespace wolf::system;
using namespace wolf::framework;
using namespace wolf::graphics;

scene::scene(_In_z_ const std::wstring& pRunningDirectory, _In_z_ const std::wstring& pAppName) :
	w_game(pRunningDirectory, pAppName)
{
	using namespace wolf;

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

	w_graphics_device_manager_configs _config;
	_config.debug_gpu = false;
	_config.off_screen_mode = false;
	w_game::set_graphics_device_manager_configs(_config);

	this->on_pixels_data_captured_signal += [&](_In_ const w_point_t pSize, _In_ const uint8_t* pPixels)->void
	{
        std::string _path;
#ifdef __WIN32
        _path = "C:\\Wolf\\a.bmp";
#elif defined(__APPLE__)
        _path = "/Users/pooyaeimandar/Documents/a.bmp";
#endif
		//w_texture::save_bmp_to_file(_path.c_str(), pSize.x, pSize.y, pPixels, 4);
	};
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
    //using namespace wolf::content_pipeline;
    //auto _scene = w_content_manager::load<w_cpipeline_scene>(L"F:\\github\\WolfSource\\Wolf.Engine\\content\\models\\wolf.wscene");
    //if (_scene)
    //{
    //    //convert to wscene
    //    //std::vector<w_cpipeline_scene> _scenes = { *_scene };
    //    //w_content_manager::save_wolf_scenes_to_file(_scenes,
    //    //    L"F:\\github\\WolfSource\\Wolf.Engine\\content\\models\\wolf.wscene");
    //    //_scene.clear();
    //}

    w_media_core::register_all();
    w_media_core _media_core;
    auto _hr = _media_core.open_stream_server("rtsp://127.0.0.1:8554/live.sdp", "rtsp", AV_CODEC_ID_H264, 60, AV_PIX_FMT_YUV420P);
    if (_hr == S_FALSE)
    {
        logger.error("error");
    }

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
	
	_color.desc.format = _output_window->vk_swap_chain_selected_format.format;

	//define attachments which has color and depth for render pass
	std::vector<w_attachment_buffer_desc> _attachment_descriptions = { _color, _depth };

	//create render pass
	_hr = this->_draw_render_pass.load(_gDevice,
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

    this->_draw_semaphore.initialize(_gDevice);
    if(_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating draw semaphore", _trace_info, 3, true);
    }
    
	//Fence for render sync
    _hr = this->_draw_fence.initialize(_gDevice);
    if(_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating draw fence", _trace_info, 3, true);
	}

	//create two primary command buffers for clearing screen
	auto _swap_chain_image_size = _output_window->vk_swap_chain_image_views.size();
	if (!_swap_chain_image_size)
	{
		//for offscreen rendering
		_swap_chain_image_size = 2;
	}
	_hr = this->_draw_command_buffers.load(_gDevice, _swap_chain_image_size);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating draw command buffers", _trace_info, 3, true);
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef WIN32
	auto _content_path_dir = wolf::system::io::get_current_directory() + L"/../../../../samples/02_basics/02_shader/src/content/";
#elif defined(__APPLE__)
	auto _content_path_dir = wolf::system::io::get_current_directory() + L"/../../../../../samples/02_basics/02_shader/src/content/";
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

	//loading pipeline cache
	std::string _pipeline_cache_name = "pipeline_cache";
	if (w_pipeline::create_pipeline_cache(_gDevice, _pipeline_cache_name) == S_FALSE)
	{
		logger.error("could not create pipeline cache");
		_pipeline_cache_name.clear();
	}

	//just we need vertex position
	w_vertex_binding_attributes _vba;
	_vba.declaration = w_vertex_declaration::VERTEX_POSITION;

	std::vector<VkDynamicState> _dynamic_states =
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	auto _descriptor_set_layout_binding = this->_shader.get_descriptor_set_layout();
	_hr = this->_pipeline.load(_gDevice,
		_vba,
		VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		this->_draw_render_pass.get_handle(),
		this->_shader.get_shader_stages(),
		_descriptor_set_layout_binding ? &_descriptor_set_layout_binding : nullptr,
		{ this->_draw_render_pass.get_viewport() },
		{ this->_draw_render_pass.get_viewport_scissor() },
		_pipeline_cache_name,
		_dynamic_states,
        {},
		0,
		nullptr,
		nullptr,
		true);
	if (_hr == S_FALSE)
	{
		release();
		V(S_FALSE, "creating pipeline", _trace_info, 3, true);
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	build_draw_command_buffers(_gDevice);
}

HRESULT scene::build_draw_command_buffers(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
	auto _size = this->_draw_command_buffers.get_commands_size();
	for (uint32_t i = 0; i < _size; ++i)
	{
		this->_draw_command_buffers.begin(i);
		{
			auto _render_pass_handle = this->_draw_render_pass.get_handle();
			auto _frame_buffer_handle = this->_draw_frame_buffers.get_frame_buffer_at(i);

			auto _cmd = this->_draw_command_buffers.get_command_at(i);
			this->_draw_render_pass.begin(_cmd,
				_frame_buffer_handle,
				w_color::CORNFLOWER_BLUE(),
				1.0f,
				0);
			{
				//drawing triangle
				//vkCmdDraw(_cmd, 3, 1, 0, 0);
			}
			this->_draw_render_pass.end(_cmd);
		}
		this->_draw_command_buffers.end(i);
	}
	return S_OK;
}

void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
	if (w_game::exiting) return;

	defer(nullptr, [&](...)
	{
		w_game::update(pGameTime);
	});

	const std::string _trace_info = this->name + "::update";
    
    logger.write(std::to_string(pGameTime.get_frames_per_second()));
    logger.write(std::to_string(pGameTime.get_elapsed_seconds()));
    logger.write(std::to_string(pGameTime.get_total_seconds()));
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
	auto _cmd = this->_draw_command_buffers.get_command_at(_frame_index);

	const VkPipelineStageFlags _stage_flags[] =
	{
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	};

	VkSubmitInfo _submit_info = {};
	_submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	_submit_info.commandBufferCount = 1;
	_submit_info.pCommandBuffers = &_cmd;
	_submit_info.pWaitDstStageMask = &_stage_flags[0];
	_submit_info.waitSemaphoreCount = static_cast<uint32_t>(_wait_semaphors.size());
	_submit_info.pWaitSemaphores = _wait_semaphors.data();
	_submit_info.signalSemaphoreCount = 1;
	_submit_info.pSignalSemaphores = _output_window->vk_rendering_done_semaphore.get(); //signal to end the render

	// Submit to queue
	auto _fence = this->_draw_fence.get();
	if (vkQueueSubmit(_gDevice->vk_graphics_queue.queue, 1, &_submit_info, *_fence))
	{
		V(S_FALSE, "submiting queue for drawing gui", _trace_info, 3, true);
		return S_FALSE;
	}
	// Wait for fence to signal that all command buffers are ready
	this->_draw_fence.wait();
	//reset draw fence
	this->_draw_fence.reset();

	auto _hr = w_game::render(pGameTime);

	auto _result = _gDevice->capture_presented_swap_chain_buffer(
        0, 
		on_pixels_data_captured_signal);
	if (_result == S_FALSE)
	{
		logger.error("something went wrong on capturing data from swap chain's buffer");
	}
	//logger.write(std::to_string(pGameTime.get_frames_per_second()));

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


	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//The following codes have been added for this project
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	this->_shader.release();
	this->_pipeline.release();
	//++++++++++++++++++++++++++++++++++++++++++++++++++++
	//++++++++++++++++++++++++++++++++++++++++++++++++++++

	return w_game::release();
}
