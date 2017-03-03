#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf::graphics;

scene::scene()
{
    //set the name of this application. This name will be use in log file
#ifdef __WIN32
	_super::set_app_name(L"03_Scene.Win32");
#elif defined(__APPLE__)
    _super::set_app_name("03_Scene.OSX");
#endif
}

scene::~scene()
{
	//release all resources
	release();
}

void scene::initialize(_In_ std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
	// TODO: Add your pre-initialization logic here

	_super::initialize(pOutputWindowsInfo);
}

void scene::load()
{
	// TODO: load your game assets here

	_super::load();
}

void scene::update(_In_ const wolf::system::w_game_time& pGameTime)
{
	// TODO: add your update logic code here

	_super::update(pGameTime);
}

void scene::begin_render(_In_ const wolf::system::w_game_time& pGameTime)
{
    _super::begin_render(pGameTime);
}

void scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
    // TODO: add your drawing code here
    _super::render(pGameTime);
}

void scene::end_render(_In_ const wolf::system::w_game_time& pGameTime)
{
    _super::end_render(pGameTime);
}

void scene::on_window_resized(_In_ UINT pIndex)
{
    w_game::on_window_resized(pIndex);
}

void scene::on_device_lost()
{
    w_game::on_device_lost();
}

#ifdef __WIN32
HRESULT scene::on_msg_proc(HWND pHWND, UINT pMessage, WPARAM pWParam, LPARAM pLParam)
{
	// TODO: add your window message code here

	return S_FALSE;
}
#endif

ULONG scene::release()
{
	if (this->get_is_released()) return 0;
	
	// TODO: release your assets here

	return _super::release();
}
