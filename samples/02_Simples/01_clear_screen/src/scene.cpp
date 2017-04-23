#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf::graphics;

scene::scene(_In_z_ std::string pRootDirectory, _In_z_ std::string pAppName) :
w_game(pRootDirectory, pAppName)
{
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

HRESULT scene::render(_In_ const wolf::system::w_game_time& pGameTime)
{
    // TODO: add your drawing code here
    return _super::render(pGameTime);
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
