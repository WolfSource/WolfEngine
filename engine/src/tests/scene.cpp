#include "pch.h"
#include "scene.h"

using namespace wolf;
using namespace wolf::system;
using namespace wolf::framework;
using namespace wolf::graphics;

scene::scene(_In_z_ const std::wstring& pContentPath, _In_ const system::w_logger_config& pLogConfig) :
	w_game(pContentPath, pLogConfig)
{
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
