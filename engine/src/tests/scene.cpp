#include "pch.h"
#include "scene.h"
#include <w_compress.hpp>
#include <libpng/png.h>
#include <w_url.h>

using namespace wolf;
using namespace wolf::system;
using namespace wolf::framework;
using namespace wolf::render::vulkan;

scene::scene(_In_z_ const std::wstring& pContentPath, _In_ const system::w_logger_config& pLogConfig) :
	w_game(pContentPath, pLogConfig)
{
	uint32_t _w, _h;
	uint8_t _c, _d;
	int _n;
	int _state;
	auto _pixels = wolf::system::io::read_png_from_file(
		"D:\\github\\WolfSource\\Wolf.Engine\\Logo.png",
		_w,
		_h,
		_c,
		_d,
		_n,
		_state);

	_state = 0;
	wolf::system::io::write_png_to_file(
		"D:\\github\\WolfSource\\Wolf.Engine\\Logo_new.png",
		_pixels,
		_w,
		_h,
		_c,
		_d,
		_state);

	_state = 0;
	std::ostringstream _stream;
	wolf::system::io::write_png_to_stream(
		_stream,
		_pixels,
		_w,
		_h,
		_c,
		_d,
		_state);

	logger.write("{}", _stream.str().c_str());

	int _width, _height, _sub, _col, _pass;
	_state = 0;
	wolf::system::io::read_jpeg_from_file(
		"D:\\github\\WolfSource\\Wolf.Engine\\Logo.png",
		_width,
		_height,
		_sub,
		_col,
		_pass,
		_state);


	std::string _url_result_buffer;
	w_url _url;
	_url.request_url("https://raw.githubusercontent.com/PooyaEimandar/Wolf.Engine/master/Logo.jpg", _url_result_buffer);
	
	std::istringstream _str_stream(_url_result_buffer);
	auto _pixels_ = wolf::system::io::read_jpeg_from_stream(
		_str_stream,
		_width,
		_height,
		_sub,
		_col,
		_pass,
		_state);
	_url.release();

	char* _src = "This is test. Hey there. this is test. Salam. Pooya. Poooooooooya. Ryannnnnnn. Raaaaaaaaaayyyyyyyyyy";
	int _compressed_size = 0;

	w_compress_result _com_result, _decom_result;
	_com_result.size_in = strlen(_src) + 1;

	if (wolf::system::w_compress::compress_buffer(_src, &_com_result) == W_PASSED)
	{
		_decom_result.size_in = _com_result.size_out;
		wolf::system::w_compress::decompress_buffer(_com_result.data, &_decom_result);
	}
	logger.write("A");
	
	//{
	//	FILE* const _ifile = fopen("C:/Users/nano byte/Desktop/v.mp4", "rb");
	//	FILE* const _ofile = fopen("C:/Users/nano byte/Desktop/v_com.mp4", "wb");
	//	_result = wolf::system::w_compress::compress_file(_ifile, _ofile);
	//}

	//if (!_result.error)
	//{
	//	FILE* const _ifile = fopen("C:/Users/nano byte/Desktop/v_com.mp4", "rb");
	//	FILE* const _ofile = fopen("C:/Users/nano byte/Desktop/v_decom.mp4", "wb");
	//	_result = wolf::system::w_compress::decompress_file(_ifile, _ofile);
	//}
	//w_compress::compress_file(_ifile, _ofile);
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
    const std::string _trace_info = this->name + "::load";
}

void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
    if (w_game::exiting) return;
    const std::string _trace_info = this->name + "::update";


    w_game::update(pGameTime);
}

W_RESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
    if (w_game::exiting) return W_PASSED;

    const std::string _trace_info = this->name + "::render";

    return W_PASSED;
}

void scene::on_window_resized(_In_ const uint32_t& pGraphicsDeviceIndex, _In_ const w_point& pNewSizeOfWindow)
{
	w_game::on_window_resized(pGraphicsDeviceIndex, pNewSizeOfWindow);
}

void scene::on_device_lost()
{
	w_game::on_device_lost();
}

ULONG scene::release()
{
    if (this->get_is_released()) return 1;

    return w_game::release();
}

//bool scene::_update_gui()
//{
//    //Setting Style
//    ImGuiStyle& _style = ImGui::GetStyle();
//    _style.Colors[ImGuiCol_Text].x = 1.0f;
//    _style.Colors[ImGuiCol_Text].y = 1.0f;
//    _style.Colors[ImGuiCol_Text].z = 1.0f;
//    _style.Colors[ImGuiCol_Text].w = 1.0f;
//
//    _style.Colors[ImGuiCol_WindowBg].x = 0.0f;
//    _style.Colors[ImGuiCol_WindowBg].y = 0.4f;
//    _style.Colors[ImGuiCol_WindowBg].z = 1.0f;
//    _style.Colors[ImGuiCol_WindowBg].w = 1.0f;
//
//    ImGuiWindowFlags  _window_flags = 0;;
//    ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
//    bool _is_open = true;
//    if (!ImGui::Begin("Wolf.Engine", &_is_open, _window_flags))
//    {
//        ImGui::End();
//        return false;
//    }
//
//    ImGui::Text("FPS:%d\r\nFrameTime:%f\r\nTotalTime:%f\r\nMouse Position:%d,%d\r\n",
//        sFPS,
//        sElapsedTimeInSec,
//        sTotalTimeTimeInSec,
//        wolf::inputs_manager.mouse.pos_x, wolf::inputs_manager.mouse.pos_y);
//    ImGui::End();
//
//    return true;
//}
