#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

scene::scene(): 
	_width(400), 
	_height(200)
{
	_super::set_app_name(L"01-Shapes_DX11_X.Win32");
}

scene::~scene()
{
	release();
}

void scene::initialize(std::map<int, std::vector<w_window_info>> pOutputWindowsInfo)
{
	// TODO: Add your pre-initialization logic here

	_super::initialize(pOutputWindowsInfo);

	// TODO: Add your post-initialization logic here
}

void scene::load()
{
	// TODO: load your game assets here
	auto _gDevice = this->get_graphics_device();

	//Load rectangle
	this->_rectangle_2D = make_unique<w_rectangle>(_gDevice);
	this->_rectangle_2D->set_geormetry(7, 7, _width, _height, 0, 0);
	this->_rectangle_2D->set_color(w_color::from_hex(w_color::PURPLE));
	this->_rectangle_2D->set_border_color(w_color::from_hex(w_color::WHITE));

	//Load rounded rectangle
	this->_rounded_rectangle_2D = make_unique<w_rectangle>(_gDevice);
	this->_rounded_rectangle_2D->set_geormetry(450, 150, _width / 2, _height, 50, 50);
	this->_rounded_rectangle_2D->set_color(w_color::from_hex(w_color::WHITE));
	this->_rounded_rectangle_2D->set_border_color(w_color::from_hex(w_color::WHITE));

	//Load ellipse
	this->_ellipse = make_unique<w_ellipse>(_gDevice);
	this->_ellipse->set_geormetry(400, 400, 20, 20);
	this->_ellipse->set_color(w_color::from_hex(w_color::WHEAT));
	this->_ellipse->set_border_color(w_color::from_hex(w_color::WHITE));

	//Load line
	auto _start_point_line = D2D1_POINT_2F();
	{
		_start_point_line.x = 220;
		_start_point_line.y = 220;
	}

	auto _stop_point_line = D2D1_POINT_2F();
	{
		_stop_point_line.x = 350;
		_stop_point_line.y = 350;
	}

	this->_line = make_unique<w_line>(_gDevice, _start_point_line, _stop_point_line);
	this->_line->set_color(w_color::from_hex(w_color::LIME));
	this->_line->set_border_color(w_color::from_hex(w_color::LIME));
	this->_line->set_stroke_width(3);

	_super::load();
}

void scene::update(const wolf::system::w_game_time& pGameTime)
{
	_super::update(pGameTime);
}

void scene::render(const wolf::system::w_game_time& pGameTime)
{
	this->sprite_batch->begin();
	{
		this->sprite_batch->draw(this->_rectangle_2D.get());
		this->sprite_batch->draw(this->_rounded_rectangle_2D.get());
		this->sprite_batch->draw(this->_ellipse.get());
		this->sprite_batch->draw(this->_line.get());
	}
	this->sprite_batch->end();
	
	_super::render(pGameTime);
}


HRESULT scene::on_msg_proc(HWND pHWND, UINT pMsg, WPARAM pWParam, LPARAM pLParam)
{
	return S_FALSE;
}


ULONG scene::release()
{
	if (this->is_released()) return 0;

	UNIQUE_RELEASE(this->_rectangle_2D);
	UNIQUE_RELEASE(this->_rounded_rectangle_2D);
	UNIQUE_RELEASE(this->_ellipse);
	UNIQUE_RELEASE(this->_line);

	return _super::release();
}
