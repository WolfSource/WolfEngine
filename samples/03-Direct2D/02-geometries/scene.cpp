#include "pch.h"
#include "scene.h"

using namespace std;
using namespace wolf::graphics;
using namespace wolf::graphics::direct2D::shapes;

scene::scene()
{
	_super::set_app_name(L"02-Geometries_DX11_X.Win32");
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

	this->_ellipse = make_unique<w_ellipse>(_gDevice);
	this->_ellipse->set_geormetry(530, 100, 70, 70);
	this->_ellipse->set_color(w_color::from_hex(w_color::WHEAT));
	this->_ellipse->set_border_color(w_color::from_hex(w_color::WHEAT));

	//Load rectangle
	this->_rectangle = make_unique<w_rectangle>(_gDevice);
	this->_rectangle->set_geormetry(0, 300, this->get_window_width(), 400, 0, 0);
	this->_rectangle->set_color(w_color::from_hex(w_color::BLUE));
	this->_rectangle->set_border_color(w_color::from_hex(w_color::BLUE));

#pragma region Create clouds

	this->_geo_cloud_0 = make_unique<w_geometry>(_gDevice);
	this->_geo_cloud_0->set_color(w_color::from_hex(w_color::LIGHT_GRAY));
	this->_geo_cloud_0->set_border_color(w_color::from_hex(w_color::LIGHT_GRAY));

	this->_geo_cloud_0->begin();
	{
		this->_geo_cloud_0->add_arc(
			D2D1::Point2F(40, 220),//Start
			D2D1::Point2F(210, 220),//End
			D2D1::SizeF(70, 70),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);

		this->_geo_cloud_0->add_arc(
			D2D1::Point2F(120, 220),//Start
			D2D1::Point2F(340, 220),//End
			D2D1::SizeF(100, 150),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);

		this->_geo_cloud_0->add_arc(
			D2D1::Point2F(150, 220),//Start
			D2D1::Point2F(400, 220),//End
			D2D1::SizeF(100, 90),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);
	}
	this->_geo_cloud_0->end();

	this->_geo_cloud_1 = make_unique<w_geometry>(_gDevice);
	this->_geo_cloud_1->set_color(w_color::from_hex(w_color::LIGHT_GRAY));
	this->_geo_cloud_1->set_border_color(w_color::from_hex(w_color::LIGHT_GRAY));
	this->_geo_cloud_1->begin();
	{
		this->_geo_cloud_1->add_arc(
			D2D1::Point2F(420, 220),//Start
			D2D1::Point2F(580, 220),//End
			D2D1::SizeF(20, 20),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);

		this->_geo_cloud_1->add_arc(
			D2D1::Point2F(520, 220),//Start
			D2D1::Point2F(770, 220),//End
			D2D1::SizeF(40, 50),//Size
			0.0f, //Rotation angle
			D2D1_SWEEP_DIRECTION_CLOCKWISE,
			D2D1_ARC_SIZE_SMALL);
	}
	this->_geo_cloud_1->end();

#pragma endregion

	_super::load();
}

void scene::update(const wolf::system::w_game_time& pGameTime)
{
	_super::update(pGameTime);
}

void scene::render(const wolf::system::w_game_time& pGameTime)
{
	this->sprite_batch->begin(D2D1::ColorF::MidnightBlue, false, false);
	{
		this->_ellipse->draw();
		this->_rectangle->draw();
		this->_geo_cloud_0->draw();
		this->_geo_cloud_1->draw();
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
	
	UNIQUE_RELEASE(this->_ellipse);
	UNIQUE_RELEASE(this->_rectangle);

	UNIQUE_RELEASE(this->_geo_cloud_0);
	UNIQUE_RELEASE(this->_geo_cloud_1);

	return _super::release();
}
