#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf::graphics;

scene::scene()
{
	//set the name of this application. This name will be use in log file
	w_game::set_app_name(L"04_One_Scene_Multiple_Windows_DX11_X.Win32");
}

scene::~scene()
{
	//release all resources
	release();
}

void scene::initialize(std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
	// TODO: Add your pre-initialization logic here

	_super::initialize(pOutputWindowsInfo);
}

void scene::load()
{
	// TODO: load your game assets here

	_super::load();
}

void scene::update(const wolf::system::w_game_time& pGameTime)
{
	// TODO: add your update logic code here

	_super::update(pGameTime);
}

void scene::begin_render(const wolf::system::w_game_time& pGameTime)
{
	//change the clear color 
	this->clear_color[0] = std::cos(pGameTime.get_total_seconds());
	_super::begin_render(pGameTime);
}

void scene::render(const wolf::system::w_game_time& pGameTime)
{
	// TODO: add your drawing code here

	_super::render(pGameTime);
}

//End render on all graphics devices
void scene::end_render(const wolf::system::w_game_time& pGameTime)
{
	_super::end_render(pGameTime);
}

HRESULT scene::on_msg_proc(HWND pHWND, UINT pMessage, WPARAM pWParam, LPARAM pLParam)
{
	return S_FALSE;
}

ULONG scene::release()
{
	if (this->is_released()) return 0;

	// TODO: release your assets here

	return _super::release();
}
