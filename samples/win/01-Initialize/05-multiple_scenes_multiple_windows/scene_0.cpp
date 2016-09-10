#include "pch.h"
#include "scene_0.h"

using namespace std;
using namespace wolf::graphics;

scene_0::scene_0()
{
	//set the name of this application. This name will be use in log file
	_super::set_app_name(L"05_Multiple_Scenes_Multiple_Windows_DX11_X.Win32");
}

scene_0::~scene_0()
{
	//release all resources
	release();
}

void scene_0::initialize(std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
	// TODO: Add your pre-initialization logic here

	_super::initialize(pOutputWindowsInfo);
}

void scene_0::load()
{
	// TODO: load your game assets here

	_super::load();
}

void scene_0::update(const wolf::system::w_game_time& pGameTime)
{
	// TODO: add your update logic code here

	_super::update(pGameTime);
}

void scene_0::begin_render(const wolf::system::w_game_time& pGameTime)
{
	//change the clear color 
	this->clear_color[1] = std::cos(pGameTime.get_total_seconds());
	_super::begin_render(pGameTime);
}

void scene_0::render(const wolf::system::w_game_time& pGameTime)
{
	// TODO: add your drawing code here

	_super::render(pGameTime);
}

//End render on all graphics devices
void scene_0::end_render(const wolf::system::w_game_time& pGameTime)
{
	_super::end_render(pGameTime);
}

HRESULT scene_0::on_msg_proc(HWND pHWND, UINT pMessage, WPARAM pWParam, LPARAM pLParam)
{
	return S_FALSE;
}


ULONG scene_0::release()
{
	if (this->is_released()) return 0;

	// TODO: release your assets here

	return _super::release();
}
