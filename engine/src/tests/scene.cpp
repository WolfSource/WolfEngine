#include "pch.h"
#include "scene.h"
#include <tbb/parallel_for.h>
#include <w_content_manager.h>
#include <w_task.h>

#ifdef __WIN32

#include <w_media_core.h>    

//for socket
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_IO_BUFLEN 13
#define DEFAULT_IO_PORT "8553"

#endif

using namespace wolf;
using namespace wolf::system;
using namespace wolf::framework;
using namespace wolf::graphics;

static float _time = 0;
#ifdef __WIN32

#define W 1280
#define H 720

static std::once_flag _once;
static SwsContext* ctx = nullptr;
static auto _frames = new uint8_t[W * H * 4];
#endif

static uint32_t sFPS = 0;
static float sElapsedTimeInSec = 0;
static float sTotalTimeTimeInSec = 0;

#ifdef __WIN32
static std::atomic<bool> left_button_pressed = false;
static std::atomic<bool> left_button_released = false;
static std::atomic<bool> middle_button_pressed = false;
static std::atomic<bool> middle_button_released = false;
static std::atomic<bool> right_button_pressed = false;
static std::atomic<bool> right_button_released = false;
//float                    wheel;
static std::atomic<int> MousePosX = 0;
static std::atomic<int> MousePosY = 0;
#endif

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

#ifdef __WIN32
        std::memcpy(_frames, pPixels, W * H * 4 * sizeof(uint8_t));
#endif
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

#ifdef __WIN32
w_signal<void(const w_media_core::w_stream_connection_info&)> _connection_established;
w_signal<void(const w_media_core::w_stream_frame_info&)> _filling_stream_frame_buffer;
w_signal<void(const char*)> _connection_lost;
static w_media_core _media_core;

static void recieveIO()
{
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_IO_BUFLEN];
    int recvbuflen = DEFAULT_IO_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo("172.16.5.11", DEFAULT_IO_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR)
    {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return;
    }

    // No longer need server socket
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    do {

        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
        {
            std::string _str(recvbuf);

            std::vector<std::string> _vec;
            wolf::system::convert::split_string(_str, "\n", _vec);
            for (auto& v : _vec)
            {
                if (v.find(":"))
                {
                    std::vector<int> _pos;
                    wolf::system::convert::split_string_then_convert_to<int>(v, ":", _pos);
                    if (_pos.size() > 1)
                    {
                        MousePosX = _pos[0];
                        MousePosX = _pos[1];
                    }
                    _pos.clear();
                    break;
                }
            }

            _vec.clear();
            _str.clear();
            //printf("Bytes received: %d\n", iResult);

            //// Echo the buffer back to the sender
            //iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            //if (iSendResult == SOCKET_ERROR) {
            //    printf("send failed with error: %d\n", WSAGetLastError());
            //    closesocket(ClientSocket);
            //    WSACleanup();
            //    return;
            //}
            //printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
        {
            printf("Connection closing...\n");
        }
        else
        {
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return;
        }

    } while (iResult > 0);

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();
}
#endif

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

#ifdef __WIN32
    w_task::execute_async_ppl([]() -> HRESULT
    {
        recieveIO();
        return S_OK;
    });
    _connection_established += [](const w_media_core::w_stream_connection_info&)->void
    {
        logger.write("Connection Established");
    };
    _filling_stream_frame_buffer += [](const w_media_core::w_stream_frame_info& pFrameInfo)->void
    {
        if (_frames)
        {
            std::call_once(_once, [&]()
            {
                ctx = sws_getContext(
                    W, H,
                    AV_PIX_FMT_RGBA, W, H,
                    AV_PIX_FMT_YUV420P, 0, 0, 0, 0);
            });

            int _line_size[1] = { 4 * W }; // RGBA stride
            sws_scale(ctx, &_frames, _line_size, 0, H, pFrameInfo.picture->data, pFrameInfo.picture->linesize);
        }
    };
    _connection_lost += [](const char* pURL)->void
    {
        logger.write("Connection Lost");
    };

    w_media_core::register_all();
    _media_core.open_stream_server_async(
        "rtsp://172.16.4.207:8554/live.sdp",//"rtsp://172.16.5.11:8554/live.sdp",
        "rtsp", 
        AV_CODEC_ID_H265,
        60,
        AV_PIX_FMT_YUV420P,
        W,
        H,
        _connection_established,
        _filling_stream_frame_buffer,
        _connection_lost);
#endif
    
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
    auto _hr = this->_gui_render_pass.load(_gDevice,
        _viewport,
        _viewport_scissor,
        _attachment_descriptions);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating render pass for gui", _trace_info, 3, true);
    }

    //create frame buffers
    auto _render_pass_handle = this->_gui_render_pass.get_handle();
    _hr = this->_gui_frame_buffers.load(_gDevice,
        _render_pass_handle,
        _output_window);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating frame buffers for gui", _trace_info, 3, true);
    }

    //create semaphore create info
    _hr = this->_gui_semaphore.initialize(_gDevice);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating semaphore for gui", _trace_info, 3, true);
    }

    //Fence for syncing
    _hr = this->_gui_fence.initialize(_gDevice);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating fence for gui", _trace_info, 3, true);
    }

    //create two primary command buffers for clearing screen
    auto _swap_chain_image_size = _output_window->vk_swap_chain_image_views.size();
    _hr = this->_gui_command_buffers.load(_gDevice, _swap_chain_image_size);
    if (_hr == S_FALSE)
    {
        release();
        V(S_FALSE, "creating command buffers for gui", _trace_info, 3, true);
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++
    //The following codes have been added for this project
    //++++++++++++++++++++++++++++++++++++++++++++++++++++
    w_imgui::load(_gDevice,
#ifdef __WIN32
        _output_window->hwnd,
#endif
        _screen_size,
        _render_pass_handle,
        nullptr,
        nullptr);
    //++++++++++++++++++++++++++++++++++++++++++++++++++++
    //++++++++++++++++++++++++++++++++++++++++++++++++++++
}

HRESULT scene::_build_gui_command_buffers(_In_ const std::shared_ptr<w_graphics_device>& pGDevice)
{
    auto _clear_color = w_color();
    _clear_color.r = static_cast<uint8_t>((float)(std::sin(_time)) * 255.0f);
    _clear_color.g = static_cast<uint8_t>((float)(std::cos(_time)) * 255.0f);
    _clear_color.b = 155;
    _clear_color.a = 255;

    const std::string _trace_info = this->name + "::build_gui_command_buffers";
    HRESULT _hr = S_OK;

    auto _size = this->_gui_command_buffers.get_commands_size();
    for (uint32_t i = 0; i < _size; ++i)
    {
        this->_gui_command_buffers.begin(i);
        {
            auto _cmd = this->_gui_command_buffers.get_command_at(i);
            this->_gui_render_pass.begin(_cmd, this->_gui_frame_buffers.get_frame_buffer_at(i), _clear_color);
            {
                //++++++++++++++++++++++++++++++++++++++++++++++++++++
                //The following codes have been added for this project
                //++++++++++++++++++++++++++++++++++++++++++++++++++++
                w_imgui::update_buffers(this->_gui_render_pass);
                w_imgui::render(_cmd);
                //++++++++++++++++++++++++++++++++++++++++++++++++++++
                //++++++++++++++++++++++++++++++++++++++++++++++++++++
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

    //wolf::inputs_manager.mouse.pos_x = 0;
    //wolf::inputs_manager.mouse.pos_y = 0;
    //if (MousePosX > 0)
    //{
    //    wolf::inputs_manager.mouse.pos_x = MousePosX;
    //}
    //if (MousePosY > 0)
    //{
    //    wolf::inputs_manager.mouse.pos_y = MousePosY;
    //}

    //++++++++++++++++++++++++++++++++++++++++++++++++++++
    //The following codes have been added for this project
    //++++++++++++++++++++++++++++++++++++++++++++++++++++

    sFPS = pGameTime.get_frames_per_second();
    sElapsedTimeInSec = pGameTime.get_elapsed_seconds();
    sTotalTimeTimeInSec = pGameTime.get_total_seconds();

    w_imgui::new_frame(sElapsedTimeInSec, [this]()
    {
        _update_gui();
    });
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

    _build_gui_command_buffers(_gDevice);

    //add wait semaphores
    std::vector<VkSemaphore> _wait_semaphors = { *(_output_window->vk_swap_chain_image_is_available_semaphore.get()) };
    auto _cmd = this->_gui_command_buffers.get_command_at(_frame_index);

    const VkPipelineStageFlags _wait_dst_stage_mask[] =
    {
        VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    };

    //reset draw fence
    this->_gui_fence.reset();
    if (_gDevice->submit(
    { _cmd },
        _gDevice->vk_graphics_queue,
        &_wait_dst_stage_mask[0],
        _wait_semaphors,
        { *_output_window->vk_rendering_done_semaphore.get() },
        &this->_gui_fence) == S_FALSE)
    {
        V(S_FALSE, "submiting queue for drawing gui", _trace_info, 3, true);
    }
    // Wait for fence to signal that all command buffers are ready
    this->_gui_fence.wait();

    //clear all wait semaphores
    _wait_semaphors.clear();

    auto _hr = w_game::render(pGameTime);

    return _gDevice->capture_presented_swap_chain_buffer(0, on_pixels_data_captured_signal);
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

    this->_gui_fence.release();
    this->_gui_semaphore.release();

    this->_gui_command_buffers.release();
    this->_gui_render_pass.release();
    this->_gui_frame_buffers.release();

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
    _style.Colors[ImGuiCol_WindowBg].w = 1.0f;

    ImGuiWindowFlags  _window_flags = 0;;
    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
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
