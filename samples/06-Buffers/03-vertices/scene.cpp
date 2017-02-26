#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf::content_pipeline;
using namespace wolf::graphics;

scene::scene()
{
	_super::set_app_name(L"02_Textured_Quad_DX11_X.Win32");
}

scene::~scene()
{
	release();
}

void scene::initialize(std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
	_super::initialize(pOutputWindowsInfo);
}

void scene::load()
{
	auto _gDevice = this->get_graphics_device();

	//create camera view and projection
	this->_camera.set_aspect_ratio(static_cast<float>(this->get_window_width() / this->get_window_height()));
	this->_camera.set_transform(0, 0, 2.0f);
	this->_camera.update_view();
	this->_camera.update_projection();

	//create a quad and assign a texture from "~\Content\Textures" folder
	this->_quad = std::make_unique<w_quad>();
	this->_quad->load(_gDevice->device.Get(), false, L"Textures\\WolfEngine.dds");
	
	_super::load();
}

void scene::update(const wolf::system::w_game_time& pGameTime)
{
	_super::update(pGameTime);
}

void scene::render(const wolf::system::w_game_time& pGameTime)
{
	auto _gDevice = this->get_graphics_device();

	this->_quad->set_view_projection(this->_camera.get_view_projection());
	this->_quad->render(_gDevice->context.Get());
	
	_super::render(pGameTime);
}

HRESULT scene::on_msg_proc(HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	return S_FALSE;
}

ULONG scene::release()
{
	if (this->is_released()) return 0;

	UNIQUE_RELEASE(this->_quad);

	return _super::release();
}
